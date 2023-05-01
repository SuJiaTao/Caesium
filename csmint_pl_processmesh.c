// <csmint_pl_processmesh.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"

PCMesh CInternalPipelineProcessMesh(PCDrawContext drawContext, PCRenderClass rClass) {
	// get class mesh
	PCMesh classMesh = rClass->mesh;

	// loop each vertex
	for (UINT32 vertexID = 0; vertexID < classMesh->vertCount; vertexID++) {
		
	}
}