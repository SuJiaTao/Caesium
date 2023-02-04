// Bailey Jia-Tao Brown
// 2023
// <csm_mesh.c>

#include "csm_mesh.h"
#include "csmint.h"

CSMCALL BOOL CMakeMesh(PCHandle pHandle, PCMesh meshData) {
	_CSyncEnter();

	if (pHandle == NULL) {
		_CSyncLeaveErr(FALSE, "CMakeMesh failed because pHandle was NULL");
	}
	if (meshData == NULL) {
		_CSyncLeaveErr(FALSE, "CMakeMesh failed because meshData was NULL");
	}

	// check for invalid mesh data
	if (meshData->vertCount  <= ZERO ||
		meshData->indexCount <= ZERO ||
		meshData->verts      == NULL ||
		meshData->indexes    == NULL) {
		_CSyncLeaveErr(FALSE, "CMakeMesh failed because meshData was invalid");
	}

	// allocate mesh and copy
	PCMesh mPtr = CInternalAlloc(sizeof(CMesh));
	COPY_BYTES(meshData, mPtr, sizeof(CMesh));
	mPtr->indexes = CInternalAlloc(sizeof(INT) * mPtr->indexCount);
	mPtr->verts   = CInternalAlloc(sizeof(PCVect3F) * mPtr->vertCount);

	// set pointer to mesh
	*pHandle = mPtr;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CDestroyMesh(PCHandle handle) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyMesh failed because handle was NULL");
	}
	
	PCMesh mPtr = *handle;
	if (mPtr == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyMesh failed because handle was invalid");
	}

	// free and set hndl to NULL
	CInternalFree(mPtr->indexes);
	CInternalFree(mPtr->verts);
	CInternalFree(mPtr);
	*handle = NULL;

	_CSyncLeave(TRUE);
}
