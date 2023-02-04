// Bailey Jia-Tao Brown
// 2023
// <csm.c>

#include "csmint.h"

CSMCALL BOOL CInitialize() {
	// check for already initailized
	if (_csmint.init == TRUE) return FALSE;

	ZERO_BYTES(&_csmint, sizeof(_csmint));

	// create unsafe heap
	_csmint.heap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, ZERO, ZERO);

	InitializeCriticalSection(&_csmint.lock);

	_csmint.init = TRUE;
	return TRUE;
}

CSMCALL BOOL CTerminate() {
	_CSyncEnter();

	if (_csmint.allocateCount > 0) {
		_CSyncLeave();
	}
	
	HeapDestroy(&_csmint.heap);
	DeleteCriticalSection(&_csmint.lock);

	_csmint.init = FALSE;
}

CSMCALL PCHAR CGetLastError() {
	return CInternalGetLastError();
}

CSMCALL void CLock() {
	_CSyncEnter();
}

CSMCALL void CUnlock() {
	_CSyncLeave();
}
