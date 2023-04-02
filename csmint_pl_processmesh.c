// <csmint_pl_processmesh.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"

PCMesh CInternalPipelineProcessMesh(UINT32 instanceID, PCMatrix instanceMatrix, 
	PCRenderClass rClass) {
	// take mesh from render class and make a copy
	PCMesh newMesh = CInternalAlloc(sizeof(CMesh));
	COPY_BYTES(rClass->mesh, newMesh, sizeof(CMesh));

	// loop each index
	for (UINT32 indexID = 0; indexID < newMesh->indexCount; indexID++) {
		// determine which triangle we are currently on
		UINT32 triangleID = indexID / 3;

		// determine which vertex we are currently on

	}

	return newMesh;
}