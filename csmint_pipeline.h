// <csmint_pipeline.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSMINT_PIPELINE_INCLUDE_
#define _CSMINT_PIPELINE_INCLUDE_

#include "csmint.h"
#include "csm_mesh.h"
#include "csm_draw.h"

#define CSMINT_CLIP_PLANE_POSITION	-1.0f

typedef struct CIPVertInput {
	UINT32 componentCount;
	FLOAT  valueBuffer[CSM_VERTEX_DATA_BUFFER_MAX_COMPONENTS];
} CIPVertInput, *PCIPVertInput;

typedef struct CIPVertInputList {
	CIPVertInput inputs[CSM_CLASS_MAX_VERTEX_DATA];
} CIPVertInputList, *PCIPVertInputList;

typedef struct CIPTriData {
	CIPVertInputList vertInputs[3];
	CVect3F verts[3];
	FLOAT	invDepths[3]; // cache W val to avoid per-fragment float division
} CIPTriData, *PCIPTriData;

typedef struct CIPFragContext {
	struct CIPTriContext*	parent;
	CIPVertInputList		fragInputs;
	CVect3F					currentFrag;
	CVect3F					barycentricWeightings;
} CIPFragContext, * PCIPFragContext;

typedef struct CIPTriContext {
	UINT32			instanceID;
	UINT32			triangleID;
	PCRenderClass	rClass;
	PCIPTriData		screenTriAndData;
	CIPFragContext  fragContext;
	PCRenderBuffer	renderBuffer;
	PCMaterial		material;
} CIPTriContext, * PCIPTriContext;

PCMesh CInternalPipelineProcessMesh(UINT32 instanceID, PCMatrix instanceMatrix,
	PCRenderClass rClass);
UINT32 CInternalPipelineClipTri(PCIPTriData inTri, PCIPTriData outTriArray);
void   CInternalPipelineProjectTri(PCRenderBuffer renderBuffer, PCIPTriData tri);
void   CInternalPipelineRasterizeTri(PCIPTriContext triContext, PCIPTriData subTri);

// implemented in <csmint_pl_rasterizetri.c>
CVect3F CInternalPipelineGenerateBarycentricWeights(PCIPTriData tri, CVect3F vert);
FLOAT   CInternalPipelineFastDistance(CVect3F p1, CVect3F p2);

#endif
