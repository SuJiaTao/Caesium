// <csmint_pl_rasterizetri.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"
#include "csm_fragment.h"
#include <math.h>
#include <stdio.h>

static __forceinline void _drawFragment(PCIPTriContext triContext) {
	PCRenderBuffer renderBuffer = triContext->renderBuffer;
	CVect3F vertex = triContext->fragContext.currentFrag;

	// generate frag position
	INT fragPosX = floorf(vertex.x - 0.5f);
	INT fragPosY = floorf(vertex.y - 0.5f);

	// do early out of bounds test
	if (fragPosX < 0 || fragPosX >= renderBuffer->width ||
		fragPosY < 0 || fragPosY >= renderBuffer->height) return;

	// do early depth test and also get belowColor
	volatile FLOAT  earlyDepth;
	volatile CColor belowColor;
	CRenderBufferUnsafeGetFragment(renderBuffer, fragPosX, fragPosY, &belowColor, &earlyDepth);
	if (CRenderBufferUnsafeDepthTest(renderBuffer, fragPosX, fragPosX, vertex.z) == FALSE) return;

	// prepare rasterization color
	CColor fragColor = CMakeColor4(0, 0, 0, 0);
	BOOL keepFragment = TRUE;

	if (triContext->material != NULL) {
		// apply fragment shader
		keepFragment = triContext->material->fragmentShader(
			&triContext->fragContext,
			triContext->triangleID,
			triContext->instanceID,
			vertex,
			&fragColor
		);
	}
	else
	{
		// on no material, set to ERR purple
		fragColor = CMakeColor3(255, 0, 255);
	}

	// if color alpha is 0, cull
	if (fragColor.a == 0) return;

	// apply alpha blend
	fragColor = CFragmentBlendColor(belowColor, fragColor);

	// apply fragment to renderBuffer
	if (keepFragment) {
		CRenderBufferUnsafeSetFragment(
			renderBuffer,
			fragPosX,
			fragPosY,
			fragColor,
			vertex.z
		);
	}
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

static __forceinline void _sortTriByVerticality(PCIPTriData toEdit) {
	// brute force sort
	// set is only 3 large so performance impact is minimal
	if (toEdit->verts[0].y < toEdit->verts[1].y) {
		_swapVerts(toEdit->verts + 0, toEdit->verts + 1);
		_swapDepths(toEdit->invDepths + 0, toEdit->invDepths + 1);
		_swapInputLists(toEdit->vertOutputs + 0, toEdit->vertOutputs + 1);
	}
	
	if (toEdit->verts[0].y < toEdit->verts[2].y) {
		_swapVerts(toEdit->verts + 0, toEdit->verts + 2);
		_swapDepths(toEdit->invDepths + 0, toEdit->invDepths + 2);
		_swapInputLists(toEdit->vertOutputs + 0, toEdit->vertOutputs + 2);
	}

	if (toEdit->verts[1].y < toEdit->verts[2].y) {
		_swapVerts(toEdit->verts + 1, toEdit->verts + 2);
		_swapDepths(toEdit->invDepths + 1, toEdit->invDepths + 2);
		_swapInputLists(toEdit->vertOutputs + 1, toEdit->vertOutputs + 2);
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

static __forceinline CVect3F _generateBarycentricWeights(PCIPTriData triangle, CVect3F vert ) {
	CVect3F weights;
	CVect3F p1 = triangle->verts[0];
	CVect3F p2 = triangle->verts[1];
	CVect3F p3 = triangle->verts[2];

	// implementation taken from wikipedia

	weights.x = ((p2.y - p3.y) * (vert.x - p3.x) + (p3.x - p2.x) * (vert.y - p3.y)) /
		((p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y));
	weights.y = ((p3.y - p1.y) * (vert.x - p3.x) + (p1.x - p3.x) * (vert.y - p3.y)) /
		((p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y));
	weights.z = 1 - weights.x - weights.y;

	return weights;
}

CVect3F CInternalPipelineGenerateBarycentricWeights(PCIPTriData tri, CVect3F vert) {
	return _generateBarycentricWeights(tri, vert);
}

// note: pos.z is ignored
static __forceinline FLOAT _interpolateDepth(CVect3F weights, PCIPTriData triangle) {
	FLOAT invDepth1 = weights.x * (triangle->invDepths[0]);
	FLOAT invDepth2 = weights.y * (triangle->invDepths[1]);
	FLOAT invDepth3 = weights.z * (triangle->invDepths[2]);

	return 1.0f / (invDepth1 + invDepth2 + invDepth3);
}

static __forceinline void _prepareFragmentInputValues(PCIPVertOutputList inOutVertList, 
	PCIPTriData triData, CVect3F bWeights) {
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

static __forceinline void _drawFlatBottomTri(PCIPTriContext triContext, PCIPTriData subTri) {

	// generate each position
	CVect3F top = subTri->verts[0];
	CVect3F LBase = subTri->verts[1];
	CVect3F RBase = subTri->verts[2];

	// swap to maintain left if necessary
	if (LBase.x > RBase.x) {
		_swapVerts(&LBase, &RBase);
		_swapDepths(subTri->invDepths + 0, subTri->invDepths + 1);
	}

	// generate inverse slopes
	FLOAT invSlopeL = (top.x - LBase.x) / (top.y - LBase.y);
	FLOAT invSlopeR = (top.x - RBase.x) / (top.y - RBase.y);

	// on bad values, don't draw
	if (isinf(invSlopeL) || isinf(invSlopeR)) return;

	// walk up from bottom to top
	PCRenderBuffer renderBuff = triContext->renderBuffer;

	const INT DRAW_Y_START = max(0, LBase.y);
	const INT DRAW_Y_END   = min(renderBuff->height, top.y);

	for (INT drawY = DRAW_Y_START; drawY <= DRAW_Y_END; drawY++) {

		// get distance travelled from start Y
		// yDist has the potential to be < 0 due to floating point math, so we clamp it
		FLOAT yDist = max(0.0f, drawY - LBase.y);

		// generate start and end X positions
		const INT DRAW_X_START =
			max(0, LBase.x + (invSlopeL * yDist));
		const INT DRAW_X_END =
			min(renderBuff->width, RBase.x + (invSlopeR * yDist));

		// walk from left of triangle to right of triangle
		for (INT drawX = DRAW_X_START; drawX < DRAW_X_END; drawX++) {

			// create fragment with interpolated depth
			CVect3F drawVect =
				CMakeVect3F(drawX, drawY, 0.0f);
			CVect3F bWeights = 
				_generateBarycentricWeights(triContext->screenTriAndData, drawVect);
			drawVect.z = _interpolateDepth(bWeights, triContext->screenTriAndData);

			// prepare fragment context
			PCIPFragContext fContext		= &triContext->fragContext;
			fContext->barycentricWeightings = bWeights;
			fContext->currentFrag			= drawVect;
			_prepareFragmentInputValues(&fContext->fragInputs, triContext->screenTriAndData, bWeights);

			// draw fragment
			_drawFragment(triContext);
		}
	}
}

static __forceinline void _drawFlatTopTri(PCIPTriContext triContext, PCIPTriData subTri) {

	// generate each position
	CVect3F LBase  = subTri->verts[0];
	CVect3F RBase  = subTri->verts[1];
	CVect3F bottom = subTri->verts[2];

	// swap to maintain left if necessary
	if (LBase.x > RBase.x) {
		_swapVerts(&LBase, &RBase);
		_swapDepths(subTri->invDepths + 0, subTri->invDepths + 1);
	}

	// generate inverse slopes
	FLOAT invSlopeL = (bottom.x - LBase.x) / (bottom.y - LBase.y);
	FLOAT invSlopeR = (bottom.x - RBase.x) / (bottom.y - RBase.y);

	// on bad values, don't draw
	if (isinf(invSlopeL) || isinf(invSlopeR)) return;

	// walk down from top to bottom
	PCRenderBuffer renderBuff = triContext->renderBuffer;

	// calculate top and bottom
	// (1 below actual to not draw over flat bottom tri partition)
	const INT DRAW_Y_START = min(renderBuff->height, LBase.y - 1);
	const INT DRAW_Y_END = max(0, bottom.y);

	// note: Y walks downwards
	for (INT drawY = DRAW_Y_START; drawY >= DRAW_Y_END; drawY--) {

		// get distance travelled from start Y
		FLOAT yDist = LBase.y - drawY;

		// generate start and end X positions
		const INT DRAW_X_START =
			max(0, LBase.x - (invSlopeL * yDist));
		const INT DRAW_X_END =
			min(renderBuff->width, RBase.x - (invSlopeR * yDist));

		// walk from left of triangle to right of triangle
		for (INT drawX = DRAW_X_START; drawX < DRAW_X_END; drawX++) {

			// create fragment with interpolated depth
			// create fragment with interpolated depth
			CVect3F drawVect =
				CMakeVect3F(drawX, drawY, 0.0f);
			CVect3F bWeights = 
				_generateBarycentricWeights(triContext->screenTriAndData, drawVect);
			drawVect.z = _interpolateDepth(bWeights, triContext->screenTriAndData);

			// prepare fragment context
			PCIPFragContext fContext		= &triContext->fragContext;
			fContext->barycentricWeightings = bWeights;
			fContext->currentFrag			= drawVect;
			_prepareFragmentInputValues(&fContext->fragInputs, triContext->screenTriAndData, bWeights);

			// draw fragment
			_drawFragment(triContext);
		}
	}
}

void   CInternalPipelineRasterizeTri(PCIPTriContext triContext, PCIPTriData triangle) {
	
	// set triContext's triangle to current screen triangle
	triContext->screenTriAndData = triangle;

	// sort triangle vertically
	_sortTriByVerticality(triangle);

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

	// make both triangles and draw
	CIPTriData flatBottomTri;
	COPY_BYTES(triangle, &flatBottomTri, sizeof(CIPTriData));
	flatBottomTri.verts[2] = horzPoint; // bottom is now flat

	CIPTriData flatTopTri;
	COPY_BYTES(triangle, &flatTopTri, sizeof(CIPTriData));
	flatTopTri.verts[0] = horzPoint; // top is now flat

	_drawFlatBottomTri(triContext, &flatBottomTri); // weird NaN issue
	_drawFlatTopTri(triContext, &flatTopTri);
}