// Bailey Jia-Tao Brown
// 2023
// <csmint_error.c>

#include "csmint_error.h"

void  CInternalErrorPopup(PCHAR title, PCHAR message) {
	MessageBoxA(NULL, message, title, MB_OK);
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

	_CSyncLeave();
}

void CInternalGetLastError(PCHAR errBuffer, SIZE_T maxSize) {
	_CSyncEnter();
	if (_csmint.lastError != NULL)
		strcpy_s(errBuffer, maxSize, _csmint.lastError);
	_CSyncLeave();
}
