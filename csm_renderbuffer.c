// Bailey Jia-Tao Brown
// 2023
// <csm_renderbuffer.c>

#include "csmint.h"
#include "csm_renderbuffer.h"
#include <float.h>

CSMCALL BOOL CMakeRenderBuffer(PCHandle pHandle, INT width, INT height) {
	_CSyncEnter();

	// check for bad params
	if (pHandle == NULL) {
		CInternalSetLastError("CMakeRenderBuffer failed because pHandle was NULL");
		_CSyncLeave(FALSE);
	}
	if (width <= 0 || height <= 0) {
		CInternalSetLastError("CMakeRenderBuffer failed because dimensions were invalid");
		_CSyncLeave(FALSE);
	}

	PCRenderBuffer rb = CInternalAlloc(sizeof(CRenderBuffer));
	rb->width = width;
	rb->height = height;
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

static __forceinline PCRgb _findColorPtr(PCRenderBuffer b, INT x, INT y) {
	PUINT8 arr = b->color;
	SIZE_T xOffset = (sizeof(CRgb) * x);
	SIZE_T yOffset = (sizeof(CRgb) * y * b->width) + y;
	return arr + (xOffset + yOffset);
}

static __forceinline PFLOAT _findDepthPtr(PCRenderBuffer b, INT x, INT y) {
	return b->depth + (x + (y * b->width));
}

static __forceinline BOOL _checkPosInRB(PCRenderBuffer b, INT x, INT y) {
	return (x >= 0 && x < b->width) && (y >= 0 && y < b->height);
}

CSMCALL BOOL CRenderBufferGetFragment(CHandle handle, INT x, INT y,
	PCRgb colorOut, PFLOAT depthOut) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderBufferGetFragment failed because handle was invalid");
	}

	PCRenderBuffer pBuffer = handle;
	if (_checkPosInRB(pBuffer, x, y) == FALSE) {
		_CSyncLeaveErr(FALSE, "CRenderBufferGetFragment failed because position was invalid");
	}


	// no err raised for NULL(s)
	if (colorOut != NULL) {
		*colorOut = *(_findColorPtr(pBuffer, x, y));
	}
	if (depthOut != NULL) {
		*depthOut = *(_findDepthPtr(pBuffer, x, y));
	}

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderBufferSetFragment(CHandle handle, INT x, INT y,
	CRgb color, FLOAT depth) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderBufferSetFragment failed because handle was invalid");
	}

	PCRenderBuffer pBuffer = handle;
	if (_checkPosInRB(pBuffer, x, y) == FALSE) {
		_CSyncLeaveErr(FALSE, "CRenderBufferSetFragment failed because position was invalid");
	}

	// do depth test
	if (*(_findDepthPtr(pBuffer, x, y)) < depth) {
		_CSyncLeave(TRUE);
	}

	*(_findColorPtr(pBuffer, x, y)) = color;
	*(_findDepthPtr(pBuffer, x, y)) = depth;

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
