// Bailey Jia-Tao Brown
// 2023
// <csm_mesh.c>

#include "csm_mesh.h"
#include "csmint.h"

CSMCALL CHandle CMakeMesh(UINT32 vertexCount,
	PFLOAT vertPositionalArray, UINT32 indexCount, PINT indexes) {
	_CSyncEnter();

	if (vertexCount <= 0) {
		_CSyncLeaveErr(NULL, "CMakeMesh failed because vertexCount was <= 0");
	}
	if (indexCount <= 0) {
		_CSyncLeaveErr(NULL, "CMakeMesh failed because indexCount was <= 0");
	}
	if ((indexCount % 3) != 0) {
		_CSyncLeaveErr(NULL, "CMakeMesh failed because indexCount was not a multiple of 3");
	}
	if (vertPositionalArray == NULL) {
		_CSyncLeaveErr(NULL, "CMakeMesh failed because vertPositionalArray was NULL");
	}
	if (indexes == NULL) {
		_CSyncLeaveErr(NULL, "CMakeMesh failed because indexes was NULL");
	}

	// allocate mesh
	PCMesh mPtr = CInternalAlloc(sizeof(CMesh));

	// calculate vertex data size and copy
	const SIZE_T vertexDataSize = sizeof(CVect3F) * vertexCount;
	mPtr->vertArray = CInternalAlloc(vertexDataSize);
	COPY_BYTES(vertPositionalArray, mPtr->vertArray, vertexDataSize);

	// calculate indicies size and copy
	const SIZE_T indexDataSize = sizeof(INT) * indexCount;
	mPtr->indexArray = CInternalAlloc(indexDataSize);
	COPY_BYTES(indexes, mPtr->indexArray, indexDataSize);

	// set other data values
	mPtr->indexCount = indexCount;
	mPtr->vertCount	 = vertexCount;
	mPtr->triCount   = indexCount / 3;

	_CSyncLeave(mPtr);
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
	CInternalFree(mPtr->indexArray);
	CInternalFree(mPtr->vertArray);
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
