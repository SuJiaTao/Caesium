// Bailey Jia-Tao Brown
// 2023
// <csmint.h>

#ifndef _CSMINT_INCLUDE_
#define _CSMINT_INCLUDE_ 

#include "csm_window.h"
#include <intrin.h>

#define CSMINT_FUNCNAMESTACK_SIZE	0x80
#define CSMINT_RASTERTHREAD_COUNT	0x10

typedef struct CIPRasterThreadContext {
	HANDLE		thread;

	BOOL		m_signal_kill;
	BOOL		m_signal_rasterTask;
	BOOL		t_signal_awaitingRasterTask;

	struct CIPTriContext*	triContext;
	INT				drawY;
	INT				rasterStartX;
	INT				rasterEndX;
} CIPRasterThreadContext, * PCIPRasterThreadContext;

typedef struct Caesium {
	BOOL   init;
	HANDLE heap;

	PCHAR lastError;
	BOOL  logErrors;

	UINT32 allocateCount;
	CRITICAL_SECTION lock; // thread sync object

	PCWindow windows[CSM_MAX_WINDOWS];

	PCHAR	funcNameStack[CSMINT_FUNCNAMESTACK_SIZE];
	UINT32	funcNameStackPtr;

	LARGE_INTEGER perfCounterHzMs;

	CIPRasterThreadContext rasterThreads[CSMINT_RASTERTHREAD_COUNT];
} Caesium, *PCaesium;
Caesium _csmint;

void CInternalPushFuncNameStack(PCHAR funcname);
void CInternalPopFuncNameStack(void);

#define _CSyncEnter( )	CInternalPushFuncNameStack(__func__); \
						EnterCriticalSection(&_csmint.lock)

#define _CSyncLeave(x)	CInternalPopFuncNameStack();		 \
						LeaveCriticalSection(&_csmint.lock); \
						return x

#define ZERO_BYTES(ptr, count) __stosb(ptr, ZERO, count) 
#define COPY_BYTES(src, dest, count) __movsb(dest, src, count)

#include "csmint_memory.h"
#include "csmint_error.h"
#include "csmint_pipeline.h"

#endif
