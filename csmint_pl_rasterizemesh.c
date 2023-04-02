// <csmint_pl_rasterizemesh.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"
#include <stdio.h>

void   CInternalPipelineRasterizeMesh(UINT32 instanceID, PCRenderBuffer renderBuffer, 
	PCMesh projectedMesh, PCRenderClass rClass) {

	// smaller name lmao
	PCMesh mesh = projectedMesh;

	// loop each index
	for (UINT32 indexID = 0; indexID < mesh->indexCount; indexID++) {
		// determine which triangle we are currently on
		UINT32 triangleID = indexID / 3;

		// determine which vertex we are currently on
		UINT32 vertexID = mesh->indexArray[indexID];

		// get current vertex
		CVect3F vertex = mesh->vertArray[vertexID];

		// get current material
		UINT32 materialID = rClass->triMaterials[triangleID];
		PCMaterial material = rClass->materials[materialID];

		// if material is NULL, use default material (0)
		if (material == NULL)
			material = rClass->materials[0];

		// prepare rasterization color
		// (allocate to the heap in case the user wants to do weird stuff with it)
		PCRgb fragColor = CInternalAlloc(sizeof(CRgb));
		BOOL  keepFragment = TRUE;

		// if material is NULL, set color to ERR PURPLE
		if (material == NULL) {
			*fragColor = CMakeColor(255, 0, 255);
		}
		else {
			// apply fragment shader
			keepFragment = material->fragmentShader(
				triangleID,
				instanceID,
				vertex,
				fragColor
			);
		}

		// apply fragment to renderBuffer
		if (keepFragment) {
			CRenderBufferSetFragment(
				renderBuffer,
				(INT)vertex.x,
				(INT)vertex.y,
				*fragColor,
				vertex.z
			);
		}

		// free color
		CInternalFree(fragColor);

	} // END INDEX LOOP
}