// <csmint_pl_projecttri.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"
#include "csm_renderbuffer.h"

void   CInternalPipelineProjectTri(PCRenderBuffer renderBuffer, PCIPTriData tri) {
	// loop all vertexs of tri
	for (UINT32 vertexID = 0; vertexID < 3; vertexID++) {
		// get vertex
		PCVect3F pVert = 
			tri->verts + vertexID;

		// project vertex (since forward is -Z, flip Z value)
		pVert->x = pVert->x / -pVert->z;
		pVert->y = pVert->y / -pVert->z;

		// convert position from "unit-space" [-1, 1] to screenspace [0, h], [0, w]
		
		// get half-way point
		FLOAT fhScreenWidth  = (FLOAT)renderBuffer->width  * 0.5f;
		FLOAT fhScreenHeight = (FLOAT)renderBuffer->height * 0.5f;

		FLOAT fAspect = fhScreenWidth / fhScreenHeight;

		// scale each "unit-position" by half and offset by half
		// height will remain mapped from [-1, 1] to [0, h] but
		// width will map from [-aspect, aspect] to [0, w]
		pVert->x = (pVert->x * (fhScreenWidth / fAspect)) + fhScreenWidth;
		pVert->y = (pVert->y * fhScreenHeight) + fhScreenHeight;
	}
}