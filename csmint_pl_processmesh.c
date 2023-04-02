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
		UINT32 vertexID = newMesh->indexArray[indexID];

		// get current vertex
		CVect3F vertex = newMesh->vertArray[vertexID];

		// get current material
		UINT32 materialID = rClass->triMaterials[triangleID];
		PCMaterial material = rClass->materials[materialID];

		// if material is NULL, use default material (0)
		if (material == NULL)
			material = rClass->materials[0];

		// if default material is NULL, skip
		if (material == NULL)
			continue;

		// process vertex (if vertex shader exists)
		if (material->vertexShader != NULL) {
			CVect3F processedVertex = material->vertexShader(
				vertex,
				vertexID,
				triangleID,
				instanceID,
				*instanceMatrix,
				rClass
			);

			// apply processed vertex to mesh
			newMesh->vertArray[vertexID] = processedVertex;
		}

	} // END INDEX LOOP

	return newMesh;
}