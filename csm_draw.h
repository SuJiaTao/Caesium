// <csm_draw.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSM_DRAW_INCLUDE_
#define _CSM_DRAW_INCLUDE_

#include "csm_renderclass.h"

#define CSM_MAX_DRAW_INPUTS		0x20
#define CSM_DRAWTHREADS_COUNT	0x10

typedef struct CDrawInput {
	SIZE_T	sizeBytes;
	PVOID	pData;
} CDrawInput, *PCDrawInput;

typedef struct CDrawThreadContext {
	HANDLE	thread;
	BOOL	signal_m_kill;
	BOOL	signal_m_requestRasterTask;
	BOOL	signal_t_awaitingRasterTask;
	CHandle	triContext;
	INT		drawY;
	INT		rasterStartX;
	INT		rasterEndX;
} CDrawThreadContext, * PCDrawThreadContext;

typedef struct CDrawContext {
	CHandle		renderBuffer;
	CDrawInput	inputs[CSM_MAX_DRAW_INPUTS];
	UINT64		lastDrawTimeMS;
	CDrawThreadContext drawThreads[CSM_DRAWTHREADS_COUNT];
} CDrawContext, *PCDrawContext;

CSMCALL CHandle CMakeDrawContext(CHandle renderBuffer);
CSMCALL BOOL	CDestroyDrawContext(CHandle drawContext);

CSMCALL	CHandle	CDrawContextSetDrawInput(CHandle drawContext, UINT32 inputID, PVOID inBytes, SIZE_T size);
CSMCALL BOOL	CDrawContextGetDrawInput(CHandle drawContext, UINT32 inputID, PVOID outBytes);
CSMCALL SIZE_T	CDrawContextGetDrawInputSizeBytes(CHandle drawContext, UINT32 inputID);
CSMCALL UINT64	CDrawContextGetLastDrawTimeMS(CHandle drawContext);

CSMCALL BOOL CDraw(CHandle drawContext, CHandle rClass);
CSMCALL BOOL CDrawInstanced(CHandle drawContext, CHandle rClass,
	UINT32 instanceCount);

#endif
