// Bailey Jia-Tao Brown
// 2023
// <csmint.h>

#ifndef _CSMINT_INCLUDE_
#define _CSMINT_INCLUDE_ 

#include "csm.h"
#include <intrin.h>

typedef struct Caesium {
	BOOL   init;
	HANDLE heap;

	PCHAR lastError;

	UINT32 allocateCount;
	CRITICAL_SECTION lock; // thread sync object

	PCWindow windows[CSM_MAX_WINDOWS];
} Caesium, *PCaesium;
Caesium _csmint;

#define _CSyncEnter( )	EnterCriticalSection(&_csmint.lock)
#define _CSyncLeave(x)	LeaveCriticalSection(&_csmint.lock); \
						return x

#define ZERO_BYTES(ptr, count) __stosb(ptr, ZERO, count) 
#define COPY_BYTES(src, dest, count) __movsb(dest, src, count)

#include "csmint_memory.h"
#include "csmint_error.h"

#endif
