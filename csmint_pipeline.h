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
void   CInternalPipelineRasterizeTri(UINT32 instanceID, UINT32 triangleID,
	PCRenderBuffer renderBuffer, PCVect3F triangle, PCRenderClass rClass);

#endif
