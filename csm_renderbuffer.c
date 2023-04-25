// Bailey Jia-Tao Brown
// 2023
// <csm_renderbuffer.c>

#include "csmint.h"
#include "csm_renderbuffer.h"
#include <float.h>
#include <stdio.h>

CSMCALL BOOL CMakeRenderBuffer(PCHandle pHandle, INT width, INT height) {
	_CSyncEnter();

	// check for bad params
	if (pHandle == NULL) {
		CInternalSetLastError("CMakeRenderBuffer failed because pHandle was NULL");
		_CSyncLeave(FALSE);
	}
	if (width < 1 || height < 1) {
		CInternalSetLastError("CMakeRenderBuffer failed because dimensions were invalid");
		_CSyncLeave(FALSE);
	}

	PCRenderBuffer rb = CInternalAlloc(sizeof(CRenderBuffer));
	rb->width = width;
	rb->height = height;
	rb->color = CInternalAlloc(sizeof(PCColor) * rb->width * rb->height + rb->height);
	rb->depth = CInternalAlloc(sizeof(FLOAT) * rb->width * rb->height);

	// clear once
	CRenderBufferClear(rb, TRUE, TRUE);

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

static __forceinline PCColor _findColorPtr(PCRenderBuffer b, INT x, INT y) {
	return b->color + (x + (y * b->width));
}

static __forceinline PFLOAT _findDepthPtr(PCRenderBuffer b, INT x, INT y) {
	return b->depth + (x + (y * b->width));
}

static __forceinline BOOL _checkPosInRB(PCRenderBuffer b, INT x, INT y) {
	return (x >= 0 && x < b->width) && (y >= 0 && y < b->height);
}

CSMCALL BOOL CRenderBufferGetFragment(CHandle handle, INT x, INT y,
	PCColor colorOut, PFLOAT depthOut) {
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
	CColor color, FLOAT depth) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderBufferSetFragment failed because handle was invalid");
	}

	PCRenderBuffer pBuffer = handle;
	if (_checkPosInRB(pBuffer, x, y) == FALSE) {
		_CSyncLeaveErr(FALSE, "CRenderBufferSetFragment failed because position was invalid");
	}

	// do depth test
	volatile FLOAT oldDepth = *(_findDepthPtr(pBuffer, x, y));
	if (oldDepth > depth) {
		_CSyncLeave(FALSE);
	}

	*(_findColorPtr(pBuffer, x, y)) = color;
	*(_findDepthPtr(pBuffer, x, y)) = depth;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderBufferClear(CHandle handle, BOOL color, BOOL depth) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderBufferClear failed because pBuffer was invalid");
	}
	if (color == FALSE && depth == FALSE) {
		_CSyncLeaveErr(FALSE, "CRenderBufferClear failed because both color and depth flag were false");
	}

	PCRenderBuffer pBuffer = handle;
	INT elemCount = pBuffer->width * pBuffer->height;
	
	// set all colors to 0
	if (color == TRUE)
		__stosd(pBuffer->color, ZERO, elemCount);

	// set all depth
	if (depth == TRUE)
		__stosd(pBuffer->depth, CSM_RENDERBUFFER_MAX_DEPTH, elemCount);

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CMakeRenderBufferFromBytes(PCHandle pHandle, INT width, INT height,
	PVOID inBytes, CTextureBytesFormat byteFormat, BOOL verticalInversion) {
	_CSyncEnter();

	if (pHandle == NULL) {
		_CSyncLeaveErr(FALSE, "CMakeRenderBufferFromBytes failed because pHandle was invalid");
	}
	if (width < 1 || height < 1) {
		CInternalSetLastError("CMakeRenderBufferFromBytes failed because dimensions were invalid");
		_CSyncLeave(FALSE);
	}
	if (inBytes == NULL) {
		_CSyncLeaveErr(FALSE, "CMakeRenderBufferFromBytes failed because inBytes was NULL");
	}
	if (byteFormat >= CTextureBytesFormat_Error) {
		_CSyncLeaveErr(FALSE, "CMakeRenderBufferFromBytes failed because byteFormat was invalid");
	}

	// make render buffer
	CMakeRenderBuffer(pHandle, width, height);
	PBYTE  inBuffer = inBytes;
	UINT32 baseIndex;			// baseindex for inBuffer pixel
	UINT32 scanlineSizeBytes;	// scanline size
	UINT32 yActual;				// y accounting for vertical inversion

	// set all values based on inBytes
	for (UINT32 x = 0; x < width; x++) {
		for (UINT32 y = 0; y < height; y++) {

			// color to send to "fragment"
			CColor color = CMakeColor3(255, 0, 255);

			// calculate proper Y position
			if (verticalInversion)
				yActual = height - y - 1;
			else
				yActual = y;

			switch (byteFormat)
			{
			case CTextureBytesFormat_RGB:
				scanlineSizeBytes = width * 3;
				baseIndex = (scanlineSizeBytes * yActual) + (x * 3);
				color.r = inBuffer[baseIndex + 0];
				color.g = inBuffer[baseIndex + 1];
				color.b = inBuffer[baseIndex + 2];

				break;

			case CTextureBytesFormat_BRG:
				scanlineSizeBytes = width * 3;
				baseIndex = (scanlineSizeBytes * yActual) + (x * 3);
				color.r = inBuffer[baseIndex + 2];
				color.g = inBuffer[baseIndex + 1];
				color.b = inBuffer[baseIndex + 0];

				break;

			case CTextureBytesFormat_RGBA:
				scanlineSizeBytes = width * 4;
				baseIndex = (scanlineSizeBytes * yActual) + (x * 4);
				color.r = inBuffer[baseIndex + 0];
				color.g = inBuffer[baseIndex + 1];
				color.b = inBuffer[baseIndex + 2];
				color.a = inBuffer[baseIndex + 3];

				break;

			case CTextureBytesFormat_ARGB:
				scanlineSizeBytes = width * 4;
				baseIndex = (scanlineSizeBytes * yActual) + (x * 4);
				color.r = inBuffer[baseIndex + 3];
				color.g = inBuffer[baseIndex + 2];
				color.b = inBuffer[baseIndex + 1];
				color.a = inBuffer[baseIndex + 0];

				break;

			case CTextureBytesFormat_BRGA:
				scanlineSizeBytes = width * 4;
				baseIndex = (scanlineSizeBytes * yActual) + (x * 4);
				color.r = inBuffer[baseIndex + 2];
				color.g = inBuffer[baseIndex + 1];
				color.b = inBuffer[baseIndex + 0];
				color.a = inBuffer[baseIndex + 3];

				break;

			default:
				break;
			}

			CRenderBufferSetFragment(*pHandle, x, y, color, 0);
		}
	}

	_CSyncLeave(TRUE);
}
