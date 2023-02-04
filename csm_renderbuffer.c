// Bailey Jia-Tao Brown
// 2023
// <csm_renderbuffer.c>

#include "csmint.h"
#include "csm_renderbuffer.h"
#include <float.h>

CSMCALL BOOL CMakeRenderBuffer(PCHandle pHandle, CVect2I dimensions) {
	_CSyncEnter();

	// check for bad params
	if (pHandle == NULL) {
		CInternalSetLastError("CMakeRenderBuffer failed because pHandle was NULL");
		_CSyncLeave(FALSE);
	}
	if (dimensions.x <= 0 || dimensions.y <= 0) {
		CInternalSetLastError("CMakeRenderBuffer failed because dimensions was invalid");
		_CSyncLeave(FALSE);
	}

	PCRenderBuffer rb = CInternalAlloc(sizeof(CRenderBuffer));
	rb->width = dimensions.x;
	rb->height = dimensions.y;
	rb->color = CInternalAlloc(sizeof(PCRgb) * rb->width * rb->height);
	rb->depth = CInternalAlloc(sizeof(FLOAT) * rb->width * rb->height);

	*pHandle = rb;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CDestroyRenderBuffer(PCHandle pHandle) {
	_CSyncEnter();

	// check for bad params
	if (pHandle == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyRenderBuffer failed because pHandle was NULL");
	}

	// get buffer from pointer
	PCRenderBuffer buffer = *pHandle;
	if (buffer == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyRenderBuffer failed because pHandle was invalid");
	}

	// free values
	CInternalFree(buffer->color);
	CInternalFree(buffer->depth);
	CInternalFree(buffer);

	*pHandle = NULL;

	_CSyncLeave(TRUE);
}

static __forceinline INT _calculateRBIndex(PCRenderBuffer b, CVect2I p) {
	return p.x + (p.y * b->width);
}

static __forceinline BOOL _checkPosInRB(PCRenderBuffer b, CVect2I p) {
	return (p.x >= 0 && p.x <= b->width) && (p.y >= 0 && p.y <= b->height);
}

CSMCALL BOOL CRenderBufferGetFragment(CHandle handle, CVect2I position,
	PCRgb colorOut, PFLOAT depthOut) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderBufferGetFragment failed because handle was invalid");
	}

	PCRenderBuffer pBuffer = handle;
	if (_checkPosInRB(pBuffer, position) == FALSE) {
		_CSyncLeaveErr(FALSE, "CRenderBufferGetFragment failed because position was invalid");
	}

	INT rbi = _calculateRBIndex(pBuffer, position);

	// no err raised for NULL(s)
	if (colorOut != NULL) {
		*colorOut = pBuffer->color[rbi];
	}
	if (depthOut != NULL) {
		*depthOut = pBuffer->depth[rbi];
	}

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderBufferSetFragment(CHandle handle, CVect2I position,
	CRgb color, FLOAT depth) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderBufferSetFragment failed because handle was invalid");
	}

	PCRenderBuffer pBuffer = handle;
	if (_checkPosInRB(pBuffer, position) == FALSE) {
		_CSyncLeaveErr(FALSE, "CRenderBufferSetFragment failed because position was invalid");
	}

	INT rbi = _calculateRBIndex(pBuffer, position);

	// do depth test
	if (pBuffer->depth[rbi] < depth) {
		_CSyncLeave(TRUE);
	}

	pBuffer->color[rbi] = color;
	pBuffer->depth[rbi] = depth;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderBufferClear(CHandle handle) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderBufferClear failed because pBuffer was invalid");
	}

	PCRenderBuffer pBuffer = handle;
	INT elemCount = pBuffer->width * pBuffer->height;
	
	// set all colors to 0
	__stosb(pBuffer->color, ZERO, elemCount);

	// set all depth to FLT_MAX
	__stosd(pBuffer->depth, FLT_MAX, elemCount);

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderBufferDraw(CHandle handle, RECT drawArea, HDC deviceContext) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderBufferDraw failed because handle was invalid");
	}

	if (deviceContext == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderBufferDraw failed because deviceContext was invalid");
	}

	PCRenderBuffer pBuffer = handle;

	// get HDC resoultion
	int resX = GetDeviceCaps(deviceContext, HORZRES);
	int resY = GetDeviceCaps(deviceContext, VERTRES);

	// setup bitmap descriptor
	BITMAPINFO bti;
	ZERO_BYTES(&bti, sizeof(bti));
	bti.bmiHeader.biBitCount	= 24;
	bti.bmiHeader.biSize		= pBuffer->width * pBuffer->height * sizeof(CRgb);
	bti.bmiHeader.biWidth		= pBuffer->width;
	bti.bmiHeader.biHeight		= pBuffer->height;
	bti.bmiHeader.biPlanes		= 1;
	bti.bmiHeader.biCompression = BI_RGB;

	// draw to HDC
	// this is terrible
	int dResult = 
		StretchDIBits(deviceContext, 0, 0, pBuffer->width, pBuffer->height,
		0, 0, resX, resY, pBuffer->color, &bti, DIB_RGB_COLORS, SRCCOPY);

	if (dResult == FALSE) {
		_CSyncLeaveErr(FALSE, "CRenderBufferDraw failed. StretchDIBits returned false");
	}

	_CSyncLeave(TRUE);
}