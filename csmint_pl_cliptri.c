// <csm_pl_cliptri.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"
#include <stdio.h>
#include <math.h>

typedef struct _clipinfo {
	BOOL clipStates[3];
	INT  numBehind;
	INT  behindTriIndexes[3];
	PCIPTriData tri;
} _clipinfo, *p_clipinfo;

static __forceinline BOOL _testVertBehindClip(CVect3F vert) {
	return vert.z > CSMINT_CLIP_PLANE_POSITION ;
}

static __forceinline _clipinfo _genTriClipInfo(PCIPTriData tri) {
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

static __forceinline FLOAT _vectDist(CVect3F v1, CVect3F v2) {
	FLOAT dx = v2.x - v1.x;
	FLOAT dy = v2.y - v1.y;
	FLOAT dz = v2.z - v1.z;
	return sqrtf(dx * dx + dy * dy + dz * dz);
}

static __forceinline void _genInterpolatedVertInputs(PCIPVertOutputList pList,
	CVect3F p1, CVect3F p2, CVect3F pm, PCIPVertOutputList vp1, PCIPVertOutputList vp2) {
	// loop all inputs
	for (UINT32 inputID = 0; inputID < CSM_CLASS_MAX_VERTEX_DATA; inputID++) {
		// get each individual input for each vert
		PCIPVertOutput input1 = vp1->outputs + inputID;
		PCIPVertOutput input2 = vp2->outputs + inputID;

		// loop all components of each input (compcount WILL be the same for both unless mem corruption)
		for (UINT32 component = 0; component < input1->componentCount; component++) {
			// get values of each componenet
			FLOAT val1 = input1->valueBuffer[component];
			FLOAT val2 = input2->valueBuffer[component];

			// generate range between two values
			FLOAT range = val2 - val1;

			// calculate distance between p1 and p2, p1 and pm
			FLOAT d12 = _vectDist(p1, p2);
			FLOAT d1m = _vectDist(p1, pm);

			// interpolate between values based on dist from clipping plane
			pList->outputs[inputID].componentCount = input1->componentCount;
			pList->outputs[inputID].valueBuffer[component] =
				val1 + (range * (d1m / d12));
		}
	}
}

static __forceinline void _perpareTriWValues(PCIPTriData tri) {
	for (INT i = 0; i < 3; i++) {
		tri->invDepths[i] = 1.0f / tri->verts[i].z;
	}
}

static __forceinline void _clipTriCase1(_clipinfo clipInfo, PCIPTriData outTriArray) {
	// depending on behind vertex, generate other 2
	CVect3F v1 = { 0 }, v2 = { 0 }, v3 = { 0 };

	// maintain all vertex input lists
	PCIPVertOutputList vl1 = NULL, vl2 = NULL, vl3 = NULL;

	// this is faster than some clever logic
	switch (clipInfo.behindTriIndexes[0])
	{
	case 0:
		v1 = clipInfo.tri->verts[0]; // v1 is behind
		v2 = clipInfo.tri->verts[1];
		v3 = clipInfo.tri->verts[2];

		vl1 = &clipInfo.tri->vertOutputs[0];
		vl2 = &clipInfo.tri->vertOutputs[1];
		vl3 = &clipInfo.tri->vertOutputs[2];

		break;
	case 1:
		v2 = clipInfo.tri->verts[0];
		v1 = clipInfo.tri->verts[1]; // v1 is behind
		v3 = clipInfo.tri->verts[2];

		vl2 = &clipInfo.tri->vertOutputs[0];
		vl1 = &clipInfo.tri->vertOutputs[1];
		vl3 = &clipInfo.tri->vertOutputs[2];

		break;
	case 2:
		v2 = clipInfo.tri->verts[0];
		v3 = clipInfo.tri->verts[1];
		v1 = clipInfo.tri->verts[2]; // v1 is behind

		vl2 = &clipInfo.tri->vertOutputs[0];
		vl3 = &clipInfo.tri->vertOutputs[1];
		vl1 = &clipInfo.tri->vertOutputs[2];

		break;
	default:
		CInternalErrorPopup("Bad clipping state");
		break;
	}

	// generate midPoints
	CVect3F mp1 = _genPlaneIntersectPoint(v1, v2);
	CVect3F mp2 = _genPlaneIntersectPoint(v1, v3);

	// interpolate inputs
	CIPVertOutputList vil1 = { 0 }, vil2 = { 0 };
	_genInterpolatedVertInputs(&vil1, v1, v2, mp1, vl1, vl2);
	_genInterpolatedVertInputs(&vil2, v1, v3, mp2, vl1, vl3);

	// generate new triangle 1
	outTriArray[0].verts[0] = v2;
	outTriArray[0].verts[1] = mp1;
	outTriArray[0].verts[2] = v3;
	_perpareTriWValues(outTriArray + 0);

	outTriArray[0].vertOutputs[0] = *vl2;
	outTriArray[0].vertOutputs[1] = vil1;
	outTriArray[0].vertOutputs[2] = *vl3;

	// generate new triangle 2
	outTriArray[1].verts[0] = mp1;
	outTriArray[1].verts[1] = mp2;
	outTriArray[1].verts[2] = v3;
	_perpareTriWValues(outTriArray + 1);

	outTriArray[1].vertOutputs[0] = vil1;
	outTriArray[1].vertOutputs[1] = vil2;
	outTriArray[1].vertOutputs[2] = *vl3;
}

static __forceinline _clipTriCase2(_clipinfo clipInfo, PCIPTriData outTriArray) {
	// depending on 2 behind vertexes, generate final one
	CVect3F v1 = { 0 }, v2 = { 0 }, v3 = { 0 }; // v3 is infront
	v1 = clipInfo.tri->verts[clipInfo.behindTriIndexes[0]];
	v2 = clipInfo.tri->verts[clipInfo.behindTriIndexes[1]];

	// maintain all vertex input lists
	PCIPVertOutputList vl1 = NULL, vl2 = NULL, vl3 = NULL;
	vl1 = &clipInfo.tri->vertOutputs[clipInfo.behindTriIndexes[0]];
	vl2 = &clipInfo.tri->vertOutputs[clipInfo.behindTriIndexes[1]];

	// clever trick
	switch (clipInfo.behindTriIndexes[0] + clipInfo.behindTriIndexes[1])
	{
	case 1:
		v3 = clipInfo.tri->verts[2]; // 0 + 1 is 1 so remainder is 2
		vl3 = &clipInfo.tri->vertOutputs[2];
		break;
	case 2:
		v3 = clipInfo.tri->verts[1]; // 0 + 2 is 2 so remainder is 1
		vl3 = &clipInfo.tri->vertOutputs[1];
		break;
	case 3:
		v3 = clipInfo.tri->verts[0]; // 1 + 2 is 3 so remainder is 0
		vl3 = &clipInfo.tri->vertOutputs[0];
		break;
	default:
		CInternalErrorPopup("Bad clipping state");
		break;
	}

	// generate midpoints
	CVect3F mp1 = _genPlaneIntersectPoint(v3, v1);
	CVect3F mp2 = _genPlaneIntersectPoint(v3, v2);

	// generate interpolated values
	CIPVertOutputList vil1 = { 0 }, vil2 = { 0 };
	_genInterpolatedVertInputs(&vil1, v3, v1, mp1, vl3, vl1);
	_genInterpolatedVertInputs(&vil2, v3, v2, mp2, vl3, vl2);

	// generate final triangle
	outTriArray[0].verts[0] = mp1;
	outTriArray[0].verts[1] = v3;
	outTriArray[0].verts[2] = mp2;
	_perpareTriWValues(outTriArray + 0);

	outTriArray[0].vertOutputs[0] = vil1;
	outTriArray[0].vertOutputs[1] = *vl3;
	outTriArray[0].vertOutputs[2] = vil2;
}

UINT32 CInternalPipelineClipTri(PCIPTriData inTri, PCIPTriData outTriArray) {
	// note: returns amt of tris generated

	_clipinfo clipInfo = _genTriClipInfo(inTri);

	// if ALL are behind, ret -1 for CULL
	if (clipInfo.numBehind == 3) {
		return -1;
	}

	// if ALL are infront, ret 0 for no change, still prepare W values
	if (clipInfo.numBehind == 0) {
		_perpareTriWValues(inTri);
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

	// bad state
	CInternalErrorPopup("Bad Clipping State");
	return ~(0);
}