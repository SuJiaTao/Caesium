// <csm_pl_cliptri.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"
#include <stdio.h>

static const CVect3F CSMINT_CLIP_PLANE_VECTOR = { 0.0f, 0.0f, -1.0f };

typedef struct _cvertsbehindclip {
	BOOL clipStates[3];
	INT  numBehind;
	INT  behindTriIndexes[3];
	PCIPTri tri;
} _clipinfo, *p_clipinfo;

static __forceinline BOOL _testVertBehindClip(CVect3F vert) {
	FLOAT val = (CVect3FDot(CSMINT_CLIP_PLANE_VECTOR, vert) + CSMINT_CLIP_PLANE_POSITION);
	return val < 0.0f;
}

static __forceinline _clipinfo _genTriClipInfo(PCIPTri tri) {
	_clipinfo clipInfo = { 0 };

	clipInfo.tri = tri;

	// gen infor for each vert
	for (int i = 0; i < 3; i++) {
		clipInfo.clipStates[i] = _testVertBehindClip(tri->verts[i]); // true for behind, false for infront
		// if vert is behind
		if (clipInfo.clipStates[i] == TRUE) {
			clipInfo.behindTriIndexes[clipInfo.numBehind] = i; // add to list of behind
			clipInfo.numBehind++; // increment behind counter
		}
	}

	return clipInfo;
}

static __forceinline CVect3F _genPlaneIntersectPoint(CVect3F p1, CVect3F p2) {
	FLOAT xzSlope = (p2.x - p1.x) / (p2.z - p1.z);
	FLOAT yzSlope = (p2.y - p1.y) / (p2.z - p1.z);
	return CMakeVect3F(
		p1.x + xzSlope * (CSMINT_CLIP_PLANE_POSITION - p1.z),
		p1.y + yzSlope * (CSMINT_CLIP_PLANE_POSITION - p1.z),
		CSMINT_CLIP_PLANE_POSITION
	);
}

static __forceinline void _clipTriCase1(_clipinfo clipInfo, PCIPTri outTriArray) {
	// depending on behind vertex, generate other 2
	CVect3F v1 = { 0 }, v2 = { 0 }, v3 = { 0 };

	// this is faster than some clever logic
	switch (clipInfo.behindTriIndexes[0])
	{
	case 0:
		v1 = clipInfo.tri->verts[0]; // v1 is behind
		v2 = clipInfo.tri->verts[1];
		v3 = clipInfo.tri->verts[2];
		break;
	case 1:
		v2 = clipInfo.tri->verts[0];
		v1 = clipInfo.tri->verts[1]; // v1 is behind
		v3 = clipInfo.tri->verts[2];
		break;
	case 2:
		v2 = clipInfo.tri->verts[0];
		v3 = clipInfo.tri->verts[1];
		v1 = clipInfo.tri->verts[2]; // v1 is behind
		break;
	default:
		CInternalErrorPopup("Caesium Fatal Error", "Bad clipping state");
		break;
	}

	// generate midPoints
	CVect3F mp1 = _genPlaneIntersectPoint(v1, v2);
	CVect3F mp2 = _genPlaneIntersectPoint(v1, v3);

	// generate new triangle 1
	outTriArray[0].verts[0] = v2;
	outTriArray[0].verts[1] = mp1;
	outTriArray[0].verts[2] = v3;

	// generate new triangle 2
	outTriArray[1].verts[0] = mp1;
	outTriArray[1].verts[1] = mp2;
	outTriArray[1].verts[2] = v3;
}

static __forceinline _clipTriCase2(_clipinfo clipInfo, PCIPTri outTriArray) {
	// depending on 2 behind vertexes, generate final one
	CVect3F v1 = { 0 }, v2 = { 0 }, v3 = { 0 }; // v3 is infront
	v1 = clipInfo.tri->verts[clipInfo.behindTriIndexes[0]];
	v2 = clipInfo.tri->verts[clipInfo.behindTriIndexes[1]];

	// clever trick
	switch (clipInfo.behindTriIndexes[0] + clipInfo.behindTriIndexes[1])
	{
	case 1:
		v3 = clipInfo.tri->verts[2]; // 0 + 1 is 1 so remainder is 2
		break;
	case 2:
		v3 = clipInfo.tri->verts[1]; // 0 + 2 is 2 so remainder is 1
		break;
	case 3:
		v3 = clipInfo.tri->verts[0]; // 1 + 2 is 3 so remainder is 0
		break;
	default:
		break;
	}

	// generate midpoints
	CVect3F mp1 = _genPlaneIntersectPoint(v3, v1);
	CVect3F mp2 = _genPlaneIntersectPoint(v3, v2);

	// generate final triangle
	outTriArray[0].verts[0] = mp1;
	outTriArray[0].verts[1] = v3;
	outTriArray[0].verts[2] = mp2;
}

UINT32 CInternalPipelineClipTri(PCIPTri inTri, PCIPTri outTriArray) {
	// note: returns amt of tris generated

	_clipinfo clipInfo = _genTriClipInfo(inTri);

	// if ALL are behind, ret -1 for CULL
	if (clipInfo.numBehind == 3) {
		return -1;
	}

	// if ALL are infront, ret 0 for no change
	if (clipInfo.numBehind == 0) {
		return 0;
	}

	// if ONLY 1 vert behind
	if (clipInfo.numBehind == 1) {
		_clipTriCase1(clipInfo, outTriArray);
		return 2; // 2 tris generated
	}

	// if 2 verts behind
	if (clipInfo.numBehind == 2) {
		_clipTriCase2(clipInfo, outTriArray);
		return 1; // 1 tri generated
	}
}