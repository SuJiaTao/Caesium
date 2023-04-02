// <csmint_pl_projectmesh.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"
#include "csm_renderbuffer.h"

void CInternalPipelineProjectMesh(PCRenderBuffer renderBuffer, PCMesh processedMesh) {
	// loop all vertex of mesh
	for (UINT32 vertexID = 0; vertexID < processedMesh->vertCount; vertexID++) {
		// get vertex
		PCVect3F pVert = 
			processedMesh->vertArray + vertexID;

		// project vertex
		pVert->x = pVert->x / pVert->z;
		pVert->y = pVert->y / pVert->z;

		// convert position from "unit-space" [-1, 1] to screenspace [0, h], [0, w]
		
		// get half-way point
		FLOAT fhScreenWidth  = (FLOAT)renderBuffer->width  * 0.5f;
		FLOAT fhScreenHeight = (FLOAT)renderBuffer->height * 0.5f;

		// scale each "unit-position" by half and offset by half
		// height will remain mapped from [-1, 1] to [0, h] but
		// width will map from [-aspect, aspect] to [0, w]
		pVert->x = (pVert->x * fhScreenWidth) + fhScreenWidth;
		pVert->y = (pVert->y * fhScreenHeight) + fhScreenHeight;
	}
}