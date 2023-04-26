// Bailey Jia-Tao Brown
// 2023
// <csm_renderbuffer.h>

#ifndef _CSM_RENDERBUFFER_INCLUDE_
#define _CSM_RENDERBUFFER_INCLUDE_ 

#define CSM_RENDERBUFFER_MAX_DEPTH				(FLOAT)(-100)
#define CSM_RENDERBUFFER_DEPTH_TEST_EPSILON		(FLOAT)(-0.001)

#include "csm.h"

typedef struct CRenderBuffer {
	UINT32	width, height;
	PCColor	color;
	PFLOAT	depth;
} CRenderBuffer, *PCRenderBuffer;

typedef enum CTextureBytesFormat {
	CTextureBytesFormat_RGB,
	CTextureBytesFormat_BRG,
	CTextureBytesFormat_RGBA,
	CTextureBytesFormat_ARGB,
	CTextureBytesFormat_BRGA,
	CTextureBytesFormat_Error
} CTextureBytesFormat, *PCTextureBytesFormat;

CSMCALL BOOL CMakeRenderBuffer(PCHandle pHandle, INT width, INT height);
CSMCALL BOOL CDestroyRenderBuffer(PCHandle pHandle);

CSMCALL BOOL CRenderBufferGetFragment(CHandle handle, INT x, INT y,
	PCColor colorOut, PFLOAT depthOut);
CSMCALL BOOL CRenderBufferSetFragment(CHandle handle, INT x, INT y,
	CColor color, FLOAT depth);
CSMCALL BOOL CRenderBufferDepthTest(CHandle handle, INT x, INT y, FLOAT newDepth);
CSMCALL BOOL CRenderBufferClear(CHandle handle, BOOL color, BOOL depth);

CSMCALL BOOL CRenderBufferUnsafeGetFragment(CHandle handle, INT x, INT y,
	PCColor colorOut, PFLOAT depthOut);
CSMCALL BOOL CRenderBufferUnsafeSetFragment(CHandle handle, INT x, INT y,
	CColor color, FLOAT depth);
CSMCALL BOOL CRenderBufferUnsafeDepthTest(CHandle handle, INT x, INT y, FLOAT newDepth);

CSMCALL BOOL CMakeRenderBufferFromBytes(PCHandle pHandle, INT width, INT height, 
	PVOID inBytes, CTextureBytesFormat byteFormat, BOOL verticalInversion);

#endif
