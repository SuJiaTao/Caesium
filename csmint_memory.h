// Bailey Jia-Tao Brown
// 2023
// <csmint_memory.h>

#ifndef _CSMINT_MEMORY_INCLUDE_
#define _CSMINT_MEMORY_INCLUDE_ 

#include "csm.h"
#include "csmint.h"

PVOID CInternalAlloc(SIZE_T size);
void  CInternalFree(PVOID ptr);

#endif
