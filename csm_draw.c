// <csm_draw.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint.h"
#include "csmint_pipeline.h"
#include "csm_draw.h"
#include "csm_mesh.h"

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

		// rasterize mesh
		CInternalPipelineRasterizeMesh(instanceID, renderBuffer, drawMesh, rClass);

		// free drawMesh and it's vertex array
		// (refer to csmin_pl_processmesh.c)
		CInternalFree(drawMesh->vertArray);
		CInternalFree(drawMesh);
	}

	_CSyncLeave(TRUE);
}