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

		// project mesh onto plane
		CInternalPipelineProjectMesh(renderBuffer, drawMesh);

		// loop each triangle of mesh and rasterize triangle
		// this is done by walking indexes in groups of 3
		UINT32 triangleID = 0;
		for (UINT32 meshIndex = 0; meshIndex < drawMesh->indexCount; meshIndex += 3) {
			// alloc triangle to heap
			PCVect3F triangle = CInternalAlloc(sizeof(CVect3F) * 3);

			triangle[0] = 
				drawMesh->vertArray[drawMesh->indexArray[meshIndex + 0]];
			triangle[1] = 
				drawMesh->vertArray[drawMesh->indexArray[meshIndex + 1]];
			triangle[2] = 
				drawMesh->vertArray[drawMesh->indexArray[meshIndex + 2]];
			
			// rasterize triangle
			CInternalPipelineRasterizeTri(
				instanceID,
				triangleID,
				renderBuffer,
				triangle,
				rClass
			);

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