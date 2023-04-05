// <csmint_pipeline.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSMINT_PIPELINE_INCLUDE_
#define _CSMINT_PIPELINE_INCLUDE_

#include "csmint.h"
#include "csm_mesh.h"
#include "csm_draw.h"

#define CSMINT_CLIP_PLANE_POSITION	-1.0f

typedef struct CIPTri {
	CVect3F verts[3];
} CIPTri, *PCIPTri;

typedef struct CIPFragInfo {
	UINT32 instanceID;
	UINT32 triangleID;
	PCMaterial material;
} CIPFragInfo, * PCIPFragInfo;

typedef struct CIPFragContext {
	CIPFragInfo fragInfo;
	PCIPTri fragTri;
	CVect3F currentFrag;
	CVect3F barycentricWeightings;
} CIPFragContext, * PCIPFragContext;

PCMesh CInternalPipelineProcessMesh(UINT32 instanceID, PCMatrix instanceMatrix,
	PCRenderClass rClass);
UINT32 CInternalPipelineClipTri(PCIPTri inTri, PCIPTri outTriArray);
void   CInternalPipelineProjectTri(PCRenderBuffer renderBuffer, PCIPTri tri);
void   CInternalPipelineRasterizeTri(UINT32 instanceID, UINT32 triangleID,
	PCRenderBuffer renderBuffer, PCIPTri triangle, PCRenderClass rClass);

// implemented in <csmint_pl_rasterizetri.c>
CVect3F CInternalPipelineGenerateBarycentricWeights(PCIPTri tri, CVect3F vert);
FLOAT   CInternalPipelineFastDistance(CVect3F p1, CVect3F p2);

#endif
