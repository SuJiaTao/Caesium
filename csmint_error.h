// Bailey Jia-Tao Brown
// 2023
// <csmint_error.h>

#ifndef _CSMINT_ERROR_INCLUDE_
#define _CSMINT_ERROR_INCLUDE_ 

#include "csmint.h"

void  CInternalErrorPopup(PCHAR message);
void  CInternalSetLastError(PCHAR lastError);
void  CInternalGetLastError(PCHAR errBuffer, SIZE_T maxSize);

#define _CSyncLeaveErr(x, err)	CInternalSetLastError(err); \
								_CSyncLeave(x)

#endif
