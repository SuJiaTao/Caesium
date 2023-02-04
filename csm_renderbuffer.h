// Bailey Jia-Tao Brown
// 2023
// <csm_renderbuffer.h>

#ifndef _CSM_RENDERBUFFER_INCLUDE_
#define _CSM_RENDERBUFFER_INCLUDE_ 

#include "csm.h"

typedef struct CRenderBuffer {
	UINT32 width, height;
	PCRgb  color;
	PFLOAT depth;
} CRenderBuffer, *PCRenderBuffer;

CSMCALL BOOL CMakeRenderBuffer(PCHandle pHandle, CVect2I dimensions);
CSMCALL BOOL CDestroyRenderBuffer(PCHandle pHandle);

CSMCALL BOOL CRenderBufferGetFragment(CHandle handle, CVect2I position,
	PCRgb colorOut, PFLOAT depthOut);
CSMCALL BOOL CRenderBufferSetFragment(CHandle handle, CVect2I position,
	CRgb color, FLOAT depth);
CSMCALL BOOL CRenderBufferClear(CHandle handle);

CSMCALL BOOL CRenderBufferDraw(CHandle handle, RECT drawArea, HDC deviceContext);

#endif
