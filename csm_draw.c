// <csm_draw.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint.h"
#include "csmint_pipeline.h"
#include "csm_draw.h"
#include "csm_mesh.h"
#include <stdio.h>

CSMCALL BOOL CDraw(CHandle renderBuffer, CHandle rClass, PCMatrix pMatrix) {
	return CDrawInstanced(renderBuffer, rClass, 1, pMatrix);
}

CSMCALL BOOL CDrawInstanced(CHandle renderBuffer, CHandle rClass,
	UINT32 instanceCount, PCMatrix matrixArray) {
	_CSyncEnter();

	// check for bad params
	if (renderBuffer == NULL) {
		_CSyncLeaveErr(FALSE, "CDrawInstanced failed because renderBuffer was invalid");
	}
	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE, "CDrawInstanced failed because renderBuffer was invalid");
	}
	if (instanceCount == 0) {
		_CSyncLeaveErr(FALSE, "CDrawInstanced failed because instanceCount was 0");
	}
	if (matrixArray == NULL) {
		_CSyncLeaveErr(FALSE, "CDrawInstanced failed because matrixArray was 0");
	}

	// loop all instances
	for (UINT32 instanceID = 0; instanceID < instanceCount; instanceID++) {
		// process mesh vertexes (creates drawMesh on the heap)
		PCMesh drawMesh =
			CInternalPipelineProcessMesh(
				instanceID, 
				matrixArray + instanceID,
				rClass
			);

		// loop each triangle of mesh and rasterize triangle
		// this is done by walking indexes in groups of 3
		UINT32 triangleID = 0;
		for (UINT32 meshIndex = 0; meshIndex < drawMesh->indexCount; meshIndex += 3) {
			// alloc triangle to heap
			PCIPTri triangle = CInternalAlloc(sizeof(CIPTri));

			// get triangle from mesh
			triangle->verts[0] = 
				drawMesh->vertArray[drawMesh->indexArray[meshIndex + 0]];
			triangle->verts[1] =
				drawMesh->vertArray[drawMesh->indexArray[meshIndex + 1]];
			triangle->verts[2] =
				drawMesh->vertArray[drawMesh->indexArray[meshIndex + 2]];

			// clip triangle
			PCIPTri clippedTris = CInternalAlloc(sizeof(CIPTri) * 2);
			UINT32 triCount = CInternalPipelineClipTri(triangle, clippedTris);

			// change based on clip output
			switch (triCount)
			{
			case -1: // CULL
				break;

			case 0: // default case. no extra tris used
				// project triangle
				CInternalPipelineProjectTri(renderBuffer, triangle);

				// rasterize triangle
				CInternalPipelineRasterizeTri(
					instanceID,
					triangleID,
					renderBuffer,
					triangle,
					rClass
				);

				break;

			case 1: // clipped original tri into 1 tri
				// project triangle
				CInternalPipelineProjectTri(renderBuffer, clippedTris + 0);

				// rasterize triangle
				CInternalPipelineRasterizeTri(
					instanceID,
					triangleID,
					renderBuffer,
					clippedTris + 0,
					rClass
				);

				break;

			case 2: // clipped original tri into 2 tris
				// project triangle 1
				CInternalPipelineProjectTri(renderBuffer, clippedTris + 0);

				// rasterize triangle 1
				CInternalPipelineRasterizeTri(
					instanceID,
					triangleID,
					renderBuffer,
					clippedTris + 0,
					rClass
				);

				// project triangle 2
				CInternalPipelineProjectTri(renderBuffer, clippedTris + 1);

				// rasterize triangle 2
				CInternalPipelineRasterizeTri(
					instanceID,
					triangleID,
					renderBuffer,
					clippedTris + 1,
					rClass
				);

				break;

			default:
				CInternalErrorPopup("Caesium Fatal Error", "Bad clipping state");
				break;
			}

			// free extra triangles
			CInternalFree(clippedTris);

			// free triangle data
			CInternalFree(triangle);

			// increment triangleID
			triangleID++;
		}

		// free drawMesh and it's vertex array
		// (refer to csmin_pl_processmesh.c)
		CInternalFree(drawMesh->vertArray);
		CInternalFree(drawMesh);
	}

	_CSyncLeave(TRUE);
}