// Bailey Jia-Tao Brown
// 2023
// <csm_mesh.c>

#include "csm_mesh.h"
#include "csmint.h"

CSMCALL BOOL CMakeMesh(PCHandle pHandle, UINT32 vertexCount,
	PFLOAT vertPositionalArray, UINT32 indexCount, PINT indexes) {
	_CSyncEnter();

	if (pHandle == NULL) {
		_CSyncLeaveErr(FALSE, "CMakeMesh failed because pHandle was NULL");
	}
	if (vertexCount <= 0) {
		_CSyncLeaveErr(FALSE, "CMakeMesh failed because vertexCount was <= 0");
	}
	if (indexCount <= 0) {
		_CSyncLeaveErr(FALSE, "CMakeMesh failed because indexCount was <= 0");
	}
	if ((indexCount % 3) != 0) {
		_CSyncLeaveErr(FALSE, "CMakeMesh failed because indexCount was not a multiple of 3");
	}
	if (vertPositionalArray == NULL) {
		_CSyncLeaveErr(FALSE, "CMakeMesh failed because vertPositionalArray was NULL");
	}
	if (indexes == NULL) {
		_CSyncLeaveErr(FALSE, "CMakeMesh failed because indexes was NULL");
	}

	// allocate mesh
	PCMesh mPtr = CInternalAlloc(sizeof(CMesh));

	// calculate vertex data size and copy
	const SIZE_T vertexDataSize = sizeof(CVect3F) * vertexCount;
	mPtr->verts = CInternalAlloc(vertexDataSize);
	COPY_BYTES(vertPositionalArray, mPtr->verts, vertexDataSize);

	// calculate indicies size and copy
	const SIZE_T indexDataSize = sizeof(INT) * indexCount;
	mPtr->indexes = CInternalAlloc(indexDataSize);
	COPY_BYTES(indexes, mPtr->indexes, indexDataSize);

	// set other data values
	mPtr->indexCount = indexCount;
	mPtr->verts		 = vertexCount;
	mPtr->triCount   = indexCount / 3;

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

CSMCALL UINT32 CMeshGetTriCount(CHandle handle) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CMeshGetTriCount failed because handle was invalid");
	}

	PCMesh pMesh = handle;

	_CSyncLeave(pMesh->triCount);
}

CSMCALL UINT32 CMeshGetVertCount(CHandle handle) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CMeshGetVertCount failed because handle was invalid");
	}

	PCMesh pMesh = handle;

	_CSyncLeave(pMesh->vertCount);
}
