// <csmint_pl_rasterizetri.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"
#include <stdio.h>

void   CInternalPipelineRasterizeTri(UINT32 instanceID, UINT32 triangleID,
	PCRenderBuffer renderBuffer, PCVect3F triangle, PCRenderClass rClass) {
	// loop each vertex
	for (UINT32 triIndex = 0; triIndex < 3; triIndex++) {
		// get current vertex
		CVect3F vertex = triangle[triIndex];

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

	} // END TRI INDEX LOOP
}