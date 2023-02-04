// Bailey Jia-Tao Brown
// 2023
// <csmint_error.c>

#include "csmint_error.h"

void  CInternalErrorPopup(PCHAR title, PCHAR message) {
	MessageBoxA(NULL, message, title, MB_OK);
}

void  CInternalSetLastError(PCHAR lastError) {
	_CSyncEnter();
	_csmint.lastError = lastError;
	_CSyncLeave();
}

PCHAR CInternalGetLastError(void) {
	_CSyncEnter();
	PCHAR err = _csmint.lastError;
	_CSyncLeave(err);
}
