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
		_CSyncLeave(FALSE);
	}
	
	HeapDestroy(&_csmint.heap);
	DeleteCriticalSection(&_csmint.lock);

	_csmint.init = FALSE;

	return TRUE;
}

CSMCALL CRgb CMakeColor(INT r, INT g, INT b) {
	CRgb rCol = { b, g, r };
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

CSMCALL void CLock() {
	_CSyncEnter();
}

CSMCALL void CUnlock() {
	_CSyncLeave();
}
