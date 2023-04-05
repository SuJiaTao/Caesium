// Bailey Jia-Tao Brown
// 2023
// <csm_renderbuffer.h>

#ifndef _CSM_RENDERBUFFER_INCLUDE_
#define _CSM_RENDERBUFFER_INCLUDE_ 

#define CSM_RENDERBUFFER_MAX_DEPTH	(FLOAT)(-100)

#include "csm.h"

typedef struct CRenderBuffer {
	UINT32 width, height;
	PCRgb  color;
	PFLOAT depth;
} CRenderBuffer, *PCRenderBuffer;

CSMCALL BOOL CMakeRenderBuffer(PCHandle pHandle, INT width, INT height);
CSMCALL BOOL CDestroyRenderBuffer(PCHandle pHandle);

CSMCALL BOOL CRenderBufferGetFragment(CHandle handle, INT x, INT y,
	PCRgb colorOut, PFLOAT depthOut);
CSMCALL BOOL CRenderBufferSetFragment(CHandle handle, INT x, INT y,
	CRgb color, FLOAT depth);
CSMCALL BOOL CRenderBufferClear(CHandle handle);

#endif
