// <csm_buffer.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint.h"
#include "csm_buffer.h"

CSMCALL CHandle CMakeVertexDataBuffer(PCHAR name, UINT32 elementCount,
	UINT32 elementComponents, PFLOAT dataIn) {
	_CSyncEnter();

	if (name == NULL) {
		_CSyncLeaveErr(NULL, "CMakeVertexDataBuffer failed because name was NULL");
	}
	if (elementCount == 0) {
		_CSyncLeaveErr(NULL, "CMakeVertexDataBuffer failed because elementCount was 0");
	}
	if (elementComponents == 0) {
		_CSyncLeaveErr(NULL, "CMakeVertexDataBuffer failed because elementComponents was 0");
	}
	if (elementComponents > CSM_VERTEX_DATA_BUFFER_MAX_COMPONENTS) {
		_CSyncLeaveErr(NULL, "CMakeVertexDataBuffer failed because elementComponents was too big");
	}
	
	// allocate
	PCVertexDataBuffer vdBuffer = CInternalAlloc(sizeof(CVertexDataBuffer));
	
	// copy name
	const SIZE_T nameSize = strlen(name);
	vdBuffer->name = CInternalAlloc(nameSize + 1); // +1 for NULL
	COPY_BYTES(name, vdBuffer->name, nameSize);

	// init metadata
	vdBuffer->elementCount = elementCount;
	vdBuffer->elementComponents = elementComponents;

	// copy buffer
	const SIZE_T dataSizeBytes = elementCount * elementComponents * sizeof(FLOAT);
	vdBuffer->data = CInternalAlloc(dataSizeBytes);
	if (dataIn != NULL) {
		COPY_BYTES(dataIn, vdBuffer->data, dataSizeBytes);
	}

	_CSyncLeave(vdBuffer);
}

CSMCALL BOOL	CDestroyVertexDataBuffer(PCHandle pVertexDataBuffer) {
	_CSyncEnter();

	if (pVertexDataBuffer == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyVertexDataBuffer failed because pVertexDataBuffer was NULL");
	}

	PCVertexDataBuffer vdBuff = *pVertexDataBuffer;
	if (vdBuff == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyVertexDataBuffer failed because pVertexDataBuffer was invalid");
	}

	// free data
	CInternalFree(vdBuff->name);
	CInternalFree(vdBuff->data);
	CInternalFree(vdBuff);

	// set handle to NULL
	*pVertexDataBuffer = NULL;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL	CVertexDataBufferGetElement(CHandle vdBuffer, UINT32 index,
	PFLOAT outBuffer) {
	_CSyncEnter();

	if (vdBuffer == NULL) {
		_CSyncLeaveErr(FALSE, "CVertexDataBufferGetElement failed bevause vdBuffer was invalid");
	}
	if (outBuffer == NULL) {
		_CSyncLeaveErr(FALSE, "CVertexDataBufferGetElement failed bevause outBuffer was NULL");
	}

	CVertexDataBufferUnsafeGetElement(vdBuffer, index, outBuffer);

	_CSyncLeave(TRUE);
}

CSMCALL BOOL	CVertexDataBufferUnsafeGetElement(CHandle vdBuffer, UINT32 index,
	PFLOAT outBuffer) {
	PCVertexDataBuffer vdBuff = vdBuffer;

	// apply modulus to index so that it rolls over
	index = index % vdBuff->elementCount;

	// get data ptr and write
	PBYTE dataBytes = vdBuff->data;
	SIZE_T elemSizeBytes = sizeof(FLOAT) * vdBuff->elementComponents;
	COPY_BYTES(
		dataBytes + (elemSizeBytes * index),
		outBuffer,
		elemSizeBytes);
}

CSMCALL BOOL	CVertexDataBufferSetElement(CHandle vdBuffer, UINT32 index,
	PFLOAT inBuffer) {
	if (vdBuffer == NULL) {
		_CSyncLeaveErr(FALSE, "CVertexDataBufferSetElement failed bevause vdBuffer was invalid");
	}
	if (inBuffer == NULL) {
		_CSyncLeaveErr(FALSE, "CVertexDataBufferSetElement failed bevause inBuffer was NULL");
	}

	PCVertexDataBuffer vdBuff = vdBuffer;
	if (index >= vdBuff->elementCount) {
		_CSyncLeaveErr(FALSE, "CVertexDataBufferSetElement failed bevause index was invalid");
	}

	// get data ptr and write
	PBYTE dataBytes = vdBuff->data;
	SIZE_T elemSizeBytes = sizeof(FLOAT) * vdBuff->elementComponents;
	COPY_BYTES(
		inBuffer,
		dataBytes + (elemSizeBytes * index),
		elemSizeBytes);

	_CSyncLeave(TRUE);
}

CSMCALL UINT32	CVertexDataBufferGetElementCount(CHandle vdBuffer) {
	_CSyncEnter();

	if (vdBuffer == NULL) {
		_CSyncLeaveErr(FALSE, "CVertexDataBufferGetElementCount failed bevause vdBuffer was invalid");
	}

	PCVertexDataBuffer vdb = vdBuffer;

	_CSyncLeave(vdb->elementCount);
}

CSMCALL UINT32	CVertexDataBufferGetComponentCount(CHandle vdBuffer) {
	_CSyncEnter();

	if (vdBuffer == NULL) {
		_CSyncLeaveErr(FALSE, "CVertexDataBufferGetComponentCount failed bevause vdBuffer was invalid");
	}

	PCVertexDataBuffer vdb = vdBuffer;

	_CSyncLeave(vdb->elementComponents);
}

CSMCALL CHandle CMakeStaticDataBuffer(PCHAR name, SIZE_T sizeBytes,
	PVOID dataIn) {
	_CSyncEnter();

	if (name == NULL) {
		_CSyncLeaveErr(NULL, "CMakeStaticDataBuffer failed because name was NULL");
	}
	if (sizeBytes == 0) {
		_CSyncLeaveErr(NULL, "CMakeStaticDataBuffer failed because sizeBytes was 0");
	}

	PCStaticDataBuffer sdBuffer = CInternalAlloc(sizeof(CStaticDataBuffer));

	// init lock
	InitializeCriticalSection(&sdBuffer->mapLock);

	// init name
	const SIZE_T nameSize = strlen(name);
	sdBuffer->name = CInternalAlloc(nameSize + 1); // +1 byte for NULL
	COPY_BYTES(name, sdBuffer->name, nameSize);

	// init data
	sdBuffer->sizeBytes = sizeBytes;
	sdBuffer->data = CInternalAlloc(sizeBytes);
	if (dataIn != NULL)
		COPY_BYTES(dataIn, sdBuffer->data, sizeBytes);

	_CSyncLeave(sdBuffer);
}

CSMCALL BOOL	CDestroyStaticDataBuffer(PCHandle pStaticDataBuffer) {
	_CSyncEnter();

	if (pStaticDataBuffer == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyStaticDataBuffer failed because pStaticDataBuffer was NULL");
	}

	PCStaticDataBuffer sdBuff = *pStaticDataBuffer;
	if (sdBuff == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyStaticDataBuffer failed because pStaticDataBuffer was invalid");
	}

	if (TryEnterCriticalSection(&sdBuff->mapLock) == FALSE) {
		_CSyncLeaveErr(FALSE, "CDestroyStaticDataBuffer failed because buffer is currently mapped");
	}

	// free data
	DeleteCriticalSection(&sdBuff->mapLock);
	CInternalFree(sdBuff->name);
	CInternalFree(sdBuff->data);
	CInternalFree(sdBuff);

	// set handle to NULL
	*pStaticDataBuffer = NULL;

	_CSyncLeave(TRUE);
}

CSMCALL PVOID	CStaticDataBufferMap(CHandle sdBuffer) {
	_CSyncEnter();

	if (sdBuffer == NULL) {
		_CSyncLeaveErr(NULL, "CStaticDataBufferMap failed because sdBuffer was invalid");
	}

	PCStaticDataBuffer sdBuff = sdBuffer;
	EnterCriticalSection(&sdBuff->mapLock); // lock data

	_CSyncLeave(sdBuff->data);
}

CSMCALL void	CStaticDataBufferUnmap(CHandle sdBuffer) {
	_CSyncEnter();

	if (sdBuffer == NULL) {
		_CSyncLeaveErr(NULL, "CStaticDataBufferUnmap failed because sdBuffer was invalid");
	}

	PCStaticDataBuffer sdBuff = sdBuffer;
	LeaveCriticalSection(&sdBuff->mapLock); // lock data

	_CSyncLeave(NULL);
}

CSMCALL SIZE_T	CStaticDataBufferGetSizeBytes(CHandle sdBuffer) {
	_CSyncEnter();

	if (sdBuffer == NULL) {
		_CSyncLeaveErr(ZERO, "CStaticDataBufferGetSizeBytes failed because sdBuffer was invalid");
	}

	PCStaticDataBuffer sdBuff = sdBuffer;

	_CSyncLeave(sdBuff->sizeBytes);
}