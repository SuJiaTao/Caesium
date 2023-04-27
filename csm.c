// Bailey Jia-Tao Brown
// 2023
// <csm.c>

#include "csmint.h"
#include <stdio.h>

CSMCALL BOOL CInitialize() {
	// check for already initailized
	if (_csmint.init == TRUE) return FALSE;

	ZERO_BYTES(&_csmint, sizeof(_csmint));

	// create unsafe heap
	_csmint.heap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, ZERO, ZERO);

	InitializeCriticalSection(&_csmint.lock);

	// setup perf freq
	QueryPerformanceFrequency(&_csmint.perfCounterHzMs);
	_csmint.perfCounterHzMs.QuadPart /= 1000; // adjust for miliseconds

	// initialize raster threads
	for (UINT32 threadID = 0; threadID < CSMINT_RASTERTHREAD_COUNT; threadID++) {
		_csmint.rasterThreads[threadID].thread =
			CreateThread(
				NULL,
				ZERO,
				CInternalPipelineRasterThreadProc,
				_csmint.rasterThreads + threadID,
				ZERO, NULL
			);
	}

	_csmint.init = TRUE;
	return TRUE;
}

CSMCALL BOOL CTerminate() {
	_CSyncEnter();

	if (_csmint.allocateCount > 0) {
		CHAR errorStrBuff[0xFF];
		sprintf_s(errorStrBuff, 0xFF,
			"CTerminated failed because there were still %d allocations unfreed",
			_csmint.allocateCount
		);
		_CSyncLeaveErr(FALSE, errorStrBuff);
	}
	
	HeapDestroy(&_csmint.heap);
	DeleteCriticalSection(&_csmint.lock);

	_csmint.init = FALSE;

	return TRUE;
}

CSMCALL PVOID CAlloc(SIZE_T size) {
	CInternalAlloc(size);
}

CSMCALL void  CFree(PVOID block) {
	CInternalFree(block);
}

CSMCALL CColor CMakeColor3(INT r, INT g, INT b) {
	CColor rCol;
	rCol.r = r;
	rCol.g = g;
	rCol.b = b;
	rCol.a = 255;
	return rCol;
}

CSMCALL CColor CMakeColor4(INT r, INT g, INT b, INT a) {
	CColor rCol;
	rCol.r = r;
	rCol.g = g;
	rCol.b = b;
	rCol.a = a;
	return rCol;
}

CSMCALL PCHAR CGetLastError(void) {
	const SIZE_T errBufferSize = 0xFF;
	PCHAR buffer = CInternalAlloc(errBufferSize);
	CInternalGetLastError(buffer, errBufferSize);
	return buffer;
}

CSMCALL void  CFreeError(PCHAR error) {
	CInternalFree(error);
}

CSMCALL void  CLogErrors(BOOL state) {
	_CSyncEnter();
	_csmint.logErrors = state;
	_CSyncLeave();
}

CSMCALL void CLock() {
	_CSyncEnter();
}

CSMCALL void CUnlock() {
	_CSyncLeave();
}
