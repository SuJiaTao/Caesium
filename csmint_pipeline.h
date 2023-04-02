// <csmint_pipeline.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSMINT_PIPELINE_INCLUDE_
#define _CSMINT_PIPELINE_INCLUDE_

#include "csmint.h"
#include "csm_mesh.h"
#include "csm_draw.h"

PCMesh CInternalPipelineProcessMesh(UINT32 instanceID, PCMatrix instanceMatrix,
	PCRenderClass rClass);
void   CInternalPipelineProjectMesh(PCRenderBuffer renderBuffer, PCMesh processedMesh);
void   CInternalPipelineRasterizeMesh(UINT32 instanceID, PCRenderBuffer renderBuffer, 
	PCMesh projectedMesh, PCRenderClass rClass);

#endif
