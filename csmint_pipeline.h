// <csmint_pipeline.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSMINT_PIPELINE_INCLUDE_
#define _CSMINT_PIPELINE_INCLUDE_

#include "csmint.h"
#include "csm_mesh.h"
#include "csm_draw.h"
#include "csm_vertex.h"

#define CSMINT_CLIP_PLANE_POSITION	-1.0f

typedef struct CIPVertOutput {
	UINT32 componentCount;
	FLOAT  valueBuffer[CSM_VERTEX_DATA_BUFFER_MAX_COMPONENTS];
} CIPVertOutput, *PCIPVertOutput;

typedef struct CIPVertOutputList {
	CIPVertOutput outputs[CSM_MAX_VERTEX_OUTPUTS];
} CIPVertOutputList, *PCIPVertOutputList;

typedef struct CIPInstanceContext {
	UINT32				instanceID;
	PCDrawContext		drawContext;
	PCRenderClass		renderClass;
	PCMesh				originalMesh;
	PCMesh				processedMesh;
	PCIPVertOutputList	outputListArray;
} CIPInstanceContext, *PCIPInstanceContext;

typedef struct CIPVertContext {
	UINT32				vertexID;
	PCIPInstanceContext	instanceContext;

	PCIPVertOutputList	pOutputList;
} CIPVertContext, *PCIPVertContext;

typedef struct CIPTriContext {
	UINT32				triangleID;
	CIPInstanceContext	instanceContext;

	PCIPVertOutputList	vertOutputs[3];
	CVect3F				verts[3];
	FLOAT				invDepths[3]; // cache W val to avoid per-fragment float division
} CIPTriContext, *PCIPTriContext;

typedef struct CIPFragContext {
	PCIPTriContext			triContext;

	CIPVertOutputList		fragInputs;
	CFragPos				fragPos;
	CVect3F					barycentricWeightings;
} CIPFragContext, * PCIPFragContext;

void	CInternalPipelineProcessMesh(PCIPInstanceContext instanceContext);
UINT32	CInternalPipelineClipTri(PCIPTriContext inTri, PCIPTriContext outTriArray);
void	CInternalPipelineProjectTri(PCIPTriContext projectTri);
void	CInternalPipelineRasterizeTri(PCIPTriContext rasterTri);

// implemented in <csmint_pl_rasterizetri.c>
CVect3F CInternalPipelineGenerateBarycentricWeights(PCIPTriContext tri, CVect3F vert);
FLOAT   CInternalPipelineFastDistance(CVect3F p1, CVect3F p2);

#endif
