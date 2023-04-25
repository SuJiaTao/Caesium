// Bailey Jia-Tao Brown
// 2023
// <csmint_error.c>

#include "csmint_error.h"
#include <stdio.h>

void  CInternalErrorPopup(PCHAR message) {
	MessageBoxA(NULL, message, "Caesium Fatal Error", MB_OK);
}

void  CInternalSetLastError(PCHAR lastError) {
	_CSyncEnter();
	
	// free last memory if applicable
	if (_csmint.lastError != NULL)
		CInternalFree(_csmint.lastError);

	// get string size and realloc lastError to len + 1 (for NULL character)
	const SIZE_T strSize = strlen(lastError);
	_csmint.lastError = CInternalAlloc(strSize + 1);

	// copy string
	COPY_BYTES(lastError, _csmint.lastError, strSize);

	// log if needed
	if (_csmint.logErrors != NULL) {
		fprintf(stderr, "Current Caesium Error: %s\n", lastError);
		fprintf(stderr, "Caesium callstack: \n");
		for (UINT32 stackID = 0; stackID <= _csmint.funcNameStackPtr; stackID++) {
			fprintf(stderr, "CALLSTACK [%02d]: %s\n", stackID, _csmint.funcNameStack[stackID]);
		}
	}
		

	_CSyncLeave();
}

void CInternalGetLastError(PCHAR errBuffer, SIZE_T maxSize) {
	_CSyncEnter();
	if (_csmint.lastError != NULL)
		strcpy_s(errBuffer, maxSize, _csmint.lastError);
	_CSyncLeave();
}
