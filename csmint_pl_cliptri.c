// <csm_pl_cliptri.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"

static const CVect3F CSMINT_CLIP_PLANE_VECTOR = { 0.0f, 0.0f, -1.0f };

typedef struct _cvertsbehindclip {
	BOOL clipStates[3];
	INT  numBehind;
	INT  behindTriIndexes[3];
	PCIPTri tri;
} _clipinfo, *p_clipinfo;

static __forceinline BOOL _testVertBehindClip(CVect3F vert) {
	return (CVect3FDot(vert, CSMINT_CLIP_PLANE_VECTOR) - CSMINT_CLIP_PLANE_POSITION) < 1.0f;
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

UINT32 CInternalPipelineClipTri(PCIPTri inTri, PCIPTri outTriArray) {
	_cver

}