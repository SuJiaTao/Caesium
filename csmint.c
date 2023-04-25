// <csmint.c>
// Bailey JT Brown
// 2023

#include "csmint.h"

void CInternalPushFuncNameStack(PCHAR funcname) {
	// check stack over/underflow
	if (_csmint.funcNameStackPtr >= CSMINT_FUNCNAMESTACK_SIZE)
		CInternalErrorPopup("Faulty FuncNameStack State");

	_csmint.funcNameStack[_csmint.funcNameStackPtr] = funcname;
	_csmint.funcNameStackPtr++;
}

void CInternalPopFuncNameStack(void) {
	// check stack over/underflow
	if (_csmint.funcNameStackPtr >= CSMINT_FUNCNAMESTACK_SIZE)
		CInternalErrorPopup("Faulty FuncNameStack State");

	_csmint.funcNameStack[_csmint.funcNameStackPtr] = NULL;

	_csmint.funcNameStackPtr--;
}