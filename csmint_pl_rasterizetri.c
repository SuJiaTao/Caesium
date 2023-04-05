// <csmint_pl_rasterizetri.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"
#include <math.h>
#include <stdio.h>

static __forceinline _drawFragment(PCIPFragContext context, PCIPFragInfo dInfo, 
	PCRenderBuffer renderBuffer, CVect3F vertex) {
	// do early out of bounds test
	if (vertex.x < 0 || vertex.x >= renderBuffer->width ||
		vertex.y < 0 || vertex.y >= renderBuffer->height) return;

	// generate frag position
	INT fragPosX = floorf(vertex.x - 0.5f);
	INT fragPosY = floorf(vertex.y - 0.5f);

	// do early depth test
	volatile FLOAT earlyDepth;
	CRenderBufferGetFragment(renderBuffer, fragPosX, fragPosY, NULL, &earlyDepth);
	if (earlyDepth > vertex.x) return;

	// prepare rasterization color
	CRgb fragColor;
	BOOL keepFragment = TRUE;

	// if material is NULL, set color to ERR PURPLE
	if (dInfo->material == NULL) {
		fragColor = CMakeColor(255, 0, 255);
	}
	else {
		// apply fragment shader
		keepFragment = dInfo->material->fragmentShader(
			context,
			dInfo->triangleID,
			dInfo->instanceID,
			vertex,
			&fragColor
		);
	}

	// apply fragment to renderBuffer
	if (keepFragment) {
		CRenderBufferSetFragment(
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

static __forceinline void _sortTriByVerticality(PCIPTri toEdit) {
	// brute force sort
	// set is only 3 large so performance impact is minimal
	if (toEdit->verts[0].y < toEdit->verts[1].y)
		_swapVerts(toEdit->verts + 0, toEdit->verts + 1);

	if (toEdit->verts[0].y < toEdit->verts[2].y)
		_swapVerts(toEdit->verts + 0, toEdit->verts + 2);

	if (toEdit->verts[1].y < toEdit->verts[2].y)
		_swapVerts(toEdit->verts + 1, toEdit->verts + 2);
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

static __forceinline CVect3F _generateBarycentricWeights(PCIPTri triangle, CVect3F vert ) {
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

CVect3F CInternalPipelineGenerateBarycentricWeights(PCIPTri tri, CVect3F vert) {
	return _generateBarycentricWeights(tri, vert);
}

// note: pos.z is ignored
static __forceinline FLOAT _interpolateDepth(CVect3F weights, PCIPTri triangle, CVect3F pos) {
	FLOAT invDepth1 = weights.x * (1.0f / triangle->verts[0].z);
	FLOAT invDepth2 = weights.y * (1.0f / triangle->verts[1].z);
	FLOAT invDepth3 = weights.z * (1.0f / triangle->verts[2].z);

	return 1.0f / (invDepth1 + invDepth2 + invDepth3);
}

static __forceinline void _drawFlatBottomTri(PCIPFragInfo dInfo, 
	PCRenderBuffer renderBuff, PCIPTri triangle) {

	// generate each position
	CVect3F top = triangle->verts[0];
	CVect3F LBase = triangle->verts[1];
	CVect3F RBase = triangle->verts[2];

	// swap to maintain left if necessary
	if (LBase.x > RBase.x) {
		_swapVerts(&LBase, &RBase);
	}

	// generate inverse slopes
	FLOAT invSlopeL = (top.x - LBase.x) / (top.y - LBase.y);
	FLOAT invSlopeR = (top.x - RBase.x) / (top.y - RBase.y);

	// on bad values, don't draw
	if (isinf(invSlopeL) || isinf(invSlopeR)) return;

	// walk up from bottom to top
	const INT DRAW_Y_START = max(0, LBase.y);
	const INT DRAW_Y_END   = min(renderBuff->height, top.y);

	for (INT drawY = DRAW_Y_START; drawY <= DRAW_Y_END; drawY++) {

		// get distance travelled from start Y
		FLOAT yDist = drawY - LBase.y;

		// generate start and end X positions
		const INT DRAW_X_START =
			max(0, LBase.x + (invSlopeL * yDist));
		const INT DRAW_X_END =
			min(renderBuff->width, RBase.x + (invSlopeR * yDist));

		// walk from left of triangle to right of triangle
		for (INT drawX = DRAW_X_START; drawX <= DRAW_X_END; drawX++) {

			// create fragment with interpolated depth
			CVect3F drawVect =
				CMakeVect3F(drawX, drawY, 0.0f);
			CVect3F bWeights = _generateBarycentricWeights(triangle, drawVect);
			drawVect.z = _interpolateDepth(bWeights, triangle, drawVect);

			// prepare fragment context
			CIPFragContext context;
			context.barycentricWeightings = bWeights;
			context.currentFrag = drawVect;
			context.fragTri = triangle;
			context.fragInfo = *dInfo;

			// draw fragment
			_drawFragment(
				&context,
				dInfo,
				renderBuff,
				drawVect
			);
		}
	}
}

static __forceinline void _drawFlatTopTri(PCIPFragInfo dInfo, 
	PCRenderBuffer renderBuff, PCIPTri triangle) {

	// generate each position
	CVect3F LBase = triangle->verts[0];
	CVect3F RBase = triangle->verts[1];
	CVect3F bottom = triangle->verts[2];

	// swap to maintain left if necessary
	if (LBase.x > RBase.x) {
		_swapVerts(&LBase, &RBase);
	}

	// generate inverse slopes
	FLOAT invSlopeL = (bottom.x - LBase.x) / (bottom.y - LBase.y);
	FLOAT invSlopeR = (bottom.x - RBase.x) / (bottom.y - RBase.y);

	// on bad values, don't draw
	if (isinf(invSlopeL) || isinf(invSlopeR)) return;

	// walk down from top to bottom
	const INT DRAW_Y_START = min(renderBuff->height, LBase.y);
	const INT DRAW_Y_END   = max(0, bottom.y);

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
		for (INT drawX = DRAW_X_START; drawX <= DRAW_X_END; drawX++) {

			// create fragment with interpolated depth
			// create fragment with interpolated depth
			CVect3F drawVect =
				CMakeVect3F(drawX, drawY, 0.0f);
			CVect3F bWeights = _generateBarycentricWeights(triangle, drawVect);
			drawVect.z = _interpolateDepth(bWeights, triangle, drawVect);

			// prepare fragment context
			CIPFragContext context;
			context.barycentricWeightings = bWeights;
			context.currentFrag = drawVect;
			context.fragTri = triangle;
			context.fragInfo = *dInfo;

			// draw fragment
			_drawFragment(
				&context,
				dInfo,
				renderBuff,
				drawVect
			);
		}
	}
}

void   CInternalPipelineRasterizeTri(UINT32 instanceID, UINT32 triangleID,
	PCRenderBuffer renderBuffer, PCIPTri triangle, PCRenderClass rClass) {
	// get current material
	UINT32 materialID = rClass->triMaterials[triangleID];
	PCMaterial material = rClass->materials[materialID];

	// if material is NULL, use default material (0)
	if (material == NULL)
		material = rClass->materials[0];

	// prepare draw info
	CIPFragInfo drawInfo;
	drawInfo.instanceID = instanceID;
	drawInfo.triangleID = triangleID;
	drawInfo.material = material;
	
	// sort triangle vertically
	_sortTriByVerticality(triangle);

	// now triangle is:
	// p0 -> top
	// p1 -> middle
	// p2 -> bottom

	// generate point given a horizontal cut of the triangle
	FLOAT invslope =
		(triangle->verts[0].x - triangle->verts[2].x) /
		(triangle->verts[0].y - triangle->verts[2].y);
	FLOAT horzPointx =
		triangle->verts[2].x + (invslope * (triangle->verts[1].y - triangle->verts[2].y));
	CVect3F horzPoint =
		CMakeVect3F(horzPointx, triangle->verts[1].y, 0);

	// assign depth
	CVect3F baryWeightings = _generateBarycentricWeights(triangle, horzPoint);
	horzPoint.z = _interpolateDepth(baryWeightings, triangle, horzPoint);

	// make both triangles and draw
	CIPTri flatBottomTri;
	flatBottomTri = *triangle; // copy sorted triangle
	flatBottomTri.verts[2] = horzPoint; // bottom is now flat

	CIPTri flatTopTri;
	flatTopTri = *triangle;
	flatTopTri.verts[0] = horzPoint; // top is now flat

	_drawFlatBottomTri(&drawInfo, renderBuffer, &flatBottomTri);
	_drawFlatTopTri(&drawInfo, renderBuffer, &flatTopTri);
}