// <csmint_pl_rasterizetri.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"
#include "csm_fragment.h"
#include <immintrin.h>
#include <math.h>
#include <stdio.h>

static __forceinline FLOAT _fltInv(FLOAT flt) {
	FLOAT rf;
	_mm_store_ss(&rf, _mm_rcp_ss(_mm_set_ss(flt)));
	return rf;
}

static __forceinline void _drawFragment(PCIPFragContext fragContext) {
	PCRenderBuffer renderBuffer = fragContext->triContext->renderBuffer;

	// cache fragment positional values
	INT fragPosX = fragContext->fragPos.x;
	INT fragPosY = fragContext->fragPos.y;

	// get below color
	CColor belowColor;
	FLOAT  unusedDepth;
	CRenderBufferUnsafeGetFragment(renderBuffer, fragPosX, fragPosY, &belowColor, &unusedDepth);

	// prepare rasterization color
	CColor fragColor = { 0 };

	// call fragment shader
	

	// if color alpha is 0, cull
	if (fragColor.a == 0) return;

	// apply alpha blend (if needed)
	if (fragColor.a < 255) {
		fragColor = CFragmentBlendColor(belowColor, fragColor);
	}

	// apply fragment to renderBuffer
	CRenderBufferUnsafeSetFragment(
		renderBuffer,
		fragPosX,
		fragPosY,
		fragColor,
		fragContext->fragPos.depth
	);
}

static __forceinline void _swapVerts(PCVect3F v1, PCVect3F v2) {
	CVect3F temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}

static __forceinline void _swapDepths(PFLOAT d1, PFLOAT d2) {
	FLOAT temp = *d1;
	*d1 = *d2;
	*d2 = temp;
}

static __forceinline void _swapInputLists(PCIPVertOutputList vil1, PCIPVertOutputList vil2) {
	CIPVertOutputList temp = *vil1;
	*vil1 = *vil2;
	*vil2 = temp;
}

static __forceinline void _sortTriVertsByVerticality(PCIPTriContext triangle) {
	// brute force sort
	// set is only 3 large so performance impact is minimal
	if (triangle->verts[0].y < triangle->verts[1].y) {
		_swapVerts(triangle->verts + 0, triangle->verts + 1);
		_swapDepths(triangle->invDepths + 0, triangle->invDepths + 1);
		_swapInputLists(triangle->vertOutputs + 0, triangle->vertOutputs + 1);
	}
	
	if (triangle->verts[0].y < triangle->verts[2].y) {
		_swapVerts(triangle->verts + 0, triangle->verts + 2);
		_swapDepths(triangle->invDepths + 0, triangle->invDepths + 2);
		_swapInputLists(triangle->vertOutputs + 0, triangle->vertOutputs + 2);
	}

	if (triangle->verts[1].y < triangle->verts[2].y) {
		_swapVerts(triangle->verts + 1, triangle->verts + 2);
		_swapDepths(triangle->invDepths + 1, triangle->invDepths + 2);
		_swapInputLists(triangle->vertOutputs + 1, triangle->vertOutputs + 2);
	}
}

// note: z is ignored for p1 & p2
static __forceinline FLOAT _fastDist(CVect3F p1, CVect3F p2) {
	FLOAT dx = p2.x - p1.x;
	FLOAT dy = p2.y - p1.y;

	// ensure abs
	dx = fabsf(dx);
	dy = fabsf(dy);

	return (0.96f * max(dx, dy)) + (0.4f * min(dx, dy));
}

FLOAT   CInternalPipelineFastDistance(CVect3F p1, CVect3F p2) {
	return _fastDist(p1, p2);
}

static __forceinline CVect3F _generateBarycentricWeights(PCIPTriContext triangle, CVect3F vert ) {
	CVect3F weights;
	CVect3F p1 = triangle->verts[0];
	CVect3F p2 = triangle->verts[1];
	CVect3F p3 = triangle->verts[2];

	// implementation taken from wikipedia

	FLOAT invDenom = _fltInv((p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y));
	FLOAT dv3x = vert.x - p3.x;
	FLOAT dv3y = vert.y - p3.y;
	weights.x = ((p2.y - p3.y) * (dv3x) + (p3.x - p2.x) * (dv3y)) * invDenom;
	weights.y = ((p3.y - p1.y) * (dv3x) + (p1.x - p3.x) * (dv3y)) * invDenom;
	weights.z = 1 - weights.x - weights.y;

	return weights;
}

CVect3F CInternalPipelineGenerateBarycentricWeights(PCIPTriContext tri, CVect3F vert) {
	return _generateBarycentricWeights(tri, vert);
}

// note: pos.z is ignored
static __forceinline FLOAT _interpolateDepth(CVect3F weights, PCIPTriContext triangle) {
	weights.x *= (triangle->invDepths[0]);
	weights.y *= (triangle->invDepths[1]);
	weights.z *= (triangle->invDepths[2]);

	return _fltInv(weights.x + weights.y + weights.z);
}

static __forceinline void _prepareFragmentInputValues(PCIPVertOutputList inOutVertList, 
	PCIPTriContext triData, CVect3F bWeights) {
	PCIPVertOutputList fragInputList1 = &triData->vertOutputs[0];
	PCIPVertOutputList fragInputList2 = &triData->vertOutputs[1];
	PCIPVertOutputList fragInputList3 = &triData->vertOutputs[2];

	// interpolate all input values based on fragment
	for (UINT32 inputID = 0; inputID < CSM_CLASS_MAX_VERTEX_DATA; inputID++) {
		// get frag inputs
		PCIPVertOutput vertOutput1 = fragInputList1->outputs + inputID;
		PCIPVertOutput vertOutput2 = fragInputList2->outputs + inputID;
		PCIPVertOutput vertOutput3 = fragInputList3->outputs + inputID;
		PCIPVertOutput outVertOutput = inOutVertList->outputs + inputID;

		// if componentcount is 0, skip
		if (vertOutput1->componentCount == 0) continue;

		// loop each component and interpolate
		for (UINT32 comp = 0; comp < vertOutput1->componentCount; comp++) {
			// get each value for verts
			FLOAT val1 = vertOutput1->valueBuffer[comp];
			FLOAT val2 = vertOutput2->valueBuffer[comp];
			FLOAT val3 = vertOutput3->valueBuffer[comp];

			// get W of each vert multiplied by barycentric weights
			FLOAT w1 = (triData->invDepths[0]) * bWeights.x;
			FLOAT w2 = (triData->invDepths[1]) * bWeights.y;
			FLOAT w3 = (triData->invDepths[2]) * bWeights.z;

			// generate perspective correct values
			// implementation is taken from:
			// https://stackoverflow.com/questions/24441631/how-exactly-does-opengl-do-perspectively-correct-linear-interpolation
			FLOAT finalVal =
				(val1 * w1 + val2 * w2 + val3 * w3) / (w1 + w2 + w3);

			// assign
			outVertOutput->valueBuffer[comp] = finalVal;
			outVertOutput->componentCount = vertOutput1->componentCount;
		}
	}
}

static __forceinline void _prepareAndDrawFragment(PCIPTriContext triContext, INT drawX, INT drawY) {
	// create fragment with interpolated depth
	CVect3F drawVect =
		CMakeVect3F(drawX, drawY, 0.0f);
	CVect3F bWeights =
		_generateBarycentricWeights(triContext, drawVect);
	drawVect.z = _interpolateDepth(bWeights, triContext);

	// early depth test
	if (CRenderBufferUnsafeDepthTest(triContext->renderBuffer, drawX, drawY, drawVect.z) == FALSE) return;

	// create fragment context
	PCIPFragContext fContext		= CInternalAlloc(sizeof(CIPFragContext));
	fContext->triContext			= triContext;
	fContext->barycentricWeightings = bWeights;
	fContext->fragPos.x				= drawX;
	fContext->fragPos.y				= drawY;
	fContext->fragPos.depth			= drawVect.z;

	_prepareFragmentInputValues(&fContext->fragInputs, triContext, bWeights);

	// draw fragment
	_drawFragment(triContext);

	CInternalFree(fContext);
}

static __forceinline void _drawFlatBottomTri(PCIPTriContext triangle) {

	// generate each position
	CVect3F top		= triangle->verts[0];
	CVect3F LBase	= triangle->verts[1];
	CVect3F RBase	= triangle->verts[2];

	// swap to maintain left if necessary
	if (LBase.x > RBase.x) {
		_swapVerts(&LBase, &RBase);
		_swapDepths(triangle->invDepths + 0, triangle->invDepths + 1);
	}

	// generate inverse slopes
	FLOAT invDY		= _fltInv(top.y - LBase.y);
	FLOAT invSlopeL = (top.x - LBase.x) * invDY;
	FLOAT invSlopeR = (top.x - RBase.x) * invDY;

	// on bad values, don't draw
	if (isinf(invSlopeL) || isinf(invSlopeR)) return;

	// walk up from bottom to top
	PCRenderBuffer renderBuff = triangle->renderBuffer;

	const INT DRAW_Y_START = max(0, LBase.y);
	const INT DRAW_Y_END   = min(renderBuff->height - 1, top.y);

	for (INT drawY = DRAW_Y_START; drawY <= DRAW_Y_END; drawY++) {

		// get distance travelled from start Y
		// yDist has the potential to be < 0 due to floating point math, so we clamp it
		FLOAT yDist = max(0.0f, drawY - LBase.y);

		// generate start and end X positions
		const INT DRAW_X_START =
			max(0, LBase.x + (invSlopeL * yDist));
		const INT DRAW_X_END =
			min(renderBuff->width - 1, RBase.x + (invSlopeR * yDist));

		// walk from left of triangle to right of triangle
		for (INT drawX = DRAW_X_START; drawX <= DRAW_X_END; drawX++) {
			// prepare and draw fragment
			_prepareAndDrawFragment(triangle, drawX, drawY);
		}
	}
}

static __forceinline void _drawFlatTopTri(PCIPTriContext triangle) {

	// generate each position
	CVect3F LBase  = triangle->verts[0];
	CVect3F RBase  = triangle->verts[1];
	CVect3F bottom = triangle->verts[2];

	// swap to maintain left if necessary
	if (LBase.x > RBase.x) {
		_swapVerts(&LBase, &RBase);
		_swapDepths(triangle->invDepths + 0, triangle->invDepths + 1);
	}

	// generate inverse slopes
	FLOAT invDY		= _fltInv(bottom.y - LBase.y);
	FLOAT invSlopeL = (bottom.x - LBase.x) * invDY;
	FLOAT invSlopeR = (bottom.x - RBase.x) * invDY;

	// on bad values, don't draw
	if (isinf(invSlopeL) || isinf(invSlopeR)) return;

	// walk down from top to bottom
	PCRenderBuffer renderBuff = triangle->renderBuffer;

	// calculate top and bottom
	const INT DRAW_Y_START = min(renderBuff->height - 1, LBase.y);
	const INT DRAW_Y_END = max(0, bottom.y);

	// note: Y walks downwards
	for (INT drawY = DRAW_Y_START; drawY >= DRAW_Y_END; drawY--) {

		// get distance travelled from start Y
		FLOAT yDist = LBase.y - drawY;

		// generate start and end X positions
		const INT DRAW_X_START =
			max(0, LBase.x - (invSlopeL * yDist));
		const INT DRAW_X_END =
			min(renderBuff->width - 1, RBase.x - (invSlopeR * yDist));

		// walk from left of triangle to right of triangle
		for (INT drawX = DRAW_X_START; drawX <= DRAW_X_END; drawX++) {
			// prepare and draw fragment
			_prepareAndDrawFragment(triangle, drawX, drawY);
		}
	}
}

void   CInternalPipelineRasterizeTri(PCIPTriContext triangle) {
	// sort triangle vertically
	_sortTriVertsByVerticality(triangle);

	// now triangle is:
	// p0 -> top
	// p1 -> middle
	// p2 -> bottom

	// cull if triangle is squashed
	if ((INT)triangle->verts[0].x == (INT)triangle->verts[1].x &&
		(INT)triangle->verts[0].x == (INT)triangle->verts[2].x) return;

	// generate point given a horizontal cut of the triangle
	FLOAT invslope =
		(triangle->verts[0].x - triangle->verts[2].x) /
		(triangle->verts[0].y - triangle->verts[2].y);
	FLOAT horzPointx =
		triangle->verts[2].x + (invslope * (triangle->verts[1].y - triangle->verts[2].y));
	CVect3F horzPoint =
		CMakeVect3F(horzPointx, triangle->verts[1].y, 0);

	// assign depth
	CVect3F baryWeightings = 
		_generateBarycentricWeights(triangle, horzPoint);
	horzPoint.z = 
		_interpolateDepth(baryWeightings, triangle, horzPoint);

	// make top and bottom tris from copies of original triangle
	CIPTriContext flatBottomTri	= *triangle;
	CIPTriContext flatTopTri	= *triangle;
	flatBottomTri.verts[2]		= horzPoint; // bottom is now flat
	flatTopTri.verts[0]			= horzPoint; // top is now flat

	_drawFlatBottomTri(triangle, &flatBottomTri);
	_drawFlatTopTri(triangle, &flatTopTri);
}