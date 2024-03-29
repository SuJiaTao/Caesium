// <csm_draw.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSM_DRAW_INCLUDE_
#define _CSM_DRAW_INCLUDE_

#include "csm_renderclass.h"

#define CSM_MAX_DRAW_INPUTS		0x20

typedef struct CDrawInput {
	SIZE_T	sizeBytes;
	PVOID	pData;
} CDrawInput, *PCDrawInput;

typedef struct CDrawContext {
	CHandle		renderBuffer;
	CDrawInput	inputs[CSM_MAX_DRAW_INPUTS];
	UINT64		lastDrawTimeMS;
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
