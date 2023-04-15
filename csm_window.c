// Bailey Jia-Tao Brown
// 2023
// <csm_window.c>

#include "csm_window.h"
#include "csmint.h"
#include <stdio.h>

typedef PCWindow* PPCWindow;

static PVOID __forceinline _cGetCWin(HWND win) {
	for (int i = 0; i < CSM_MAX_WINDOWS; i++) {
		if (_csmint.windows[i] == NULL)
			continue;

		if (_csmint.windows[i]->wnd == win) 
			return _csmint.windows + i;
	}
	return NULL;
}

static LRESULT CALLBACK _csmWndProc(HWND win, UINT msg, WPARAM wP, LPARAM lP) {
	_CSyncEnter();

	PCWindow* cwinPtr;
	PCWindow  cwin;
	
	switch (msg)
	{
	case WM_PAINT:

		// get cwin
		cwinPtr = _cGetCWin(win);
		cwin = *cwinPtr;

		// break if no renderbuffer exists
		if (cwin->renderBuff == NULL)
			break;

		RECT drawRect;
		GetClientRect(cwin->wnd, &drawRect);
		DWORD width = drawRect.right - drawRect.left;
		DWORD height = drawRect.bottom - drawRect.top;

		PCRenderBuffer pBuffer = cwin->renderBuff;

		// setup bitmap descriptor
		BITMAPINFO bti;
		ZERO_BYTES(&bti, sizeof(bti));
		bti.bmiHeader.biBitCount = 24;
		bti.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bti.bmiHeader.biWidth = pBuffer->width;
		bti.bmiHeader.biHeight = pBuffer->height;
		bti.bmiHeader.biPlanes = 1;
		bti.bmiHeader.biCompression = BI_RGB;

		// paint buffer to window
		PAINTSTRUCT ps;
		HDC pDC = BeginPaint(cwin->wnd, &ps);

		INT drawRslt = 
			StretchDIBits(pDC, 0, 0, width, height, 0, 0,
			pBuffer->width, pBuffer->height,
			pBuffer->color, &bti, DIB_RGB_COLORS, SRCCOPY);

		EndPaint(cwin->wnd, &ps);

		break;

	case WM_ERASEBKGND:
		_CSyncLeave(TRUE);

	case WM_CLOSE:

		// get cwin
		cwinPtr = _cGetCWin(win);
		cwin = *cwinPtr;
		cwin->shouldClose = TRUE;

		_CSyncLeave(ZERO);

	case CSM_WINDOW_CLOSEMESSAGE:

		// get cwin
		cwinPtr = _cGetCWin(win);
		cwin = *cwinPtr;

		if (cwin->wnd != win) {
			CInternalErrorPopup("Fatal error occoured while updating CWindow.");
			ExitProcess(ERROR_INVALID_DATA);
		}

		// free all resources
		UnregisterClassA(cwin->wndClassName, NULL);
		CInternalFree(cwin->wndClassName);
		CInternalFree(cwin);

		*cwinPtr = NULL;
		
		// close
		_CSyncLeave(DefWindowProcA(win, WM_CLOSE, wP, lP));

	default:
		break;
	}

	_CSyncLeave(DefWindowProcA(win, msg, wP, lP));
}

static INT __forceinline _cFindWinSpot(void) {
	for (int i = 0; i < CSM_MAX_WINDOWS; i++) {
		if (_csmint.windows[i] == NULL) return i;
	}
	return -1;
}

CSMCALL BOOL CMakeWindow(PCHandle pHandle, PCHAR title, INT width, INT height) {
	_CSyncEnter();

	// make process DPI aware
	SetProcessDPIAware();

	if (pHandle == NULL) {
		_CSyncLeaveErr(FALSE, "CMakeWindow failed because pHandle was invalid");
	}
	if (width <= 0 || height <= 0) {
		_CSyncLeaveErr(FALSE, "CMakeWindow failed because dimensions were invalid");
	}

	INT wIndx = _cFindWinSpot();
	if (wIndx == -1) {
		_CSyncLeaveErr(FALSE, "CMakeWindow failed because MAX WINDOWS were already created");
	}

	// make window and assign
	PCWindow cwin = CInternalAlloc(sizeof(CWindow));
	_csmint.windows[wIndx] = cwin;

	// make unique window class name
	cwin->wndClassName = CInternalAlloc(sizeof(CHAR) * CSM_WINDOW_CLASSNAME_SIZE);
	sprintf_s(cwin->wndClassName, CSM_WINDOW_CLASSNAME_SIZE,
		"Caesium Window %p", cwin);

	// create window class
	WNDCLASSA wClass;
	ZERO_BYTES(&wClass, sizeof(WNDCLASSA));
	wClass.lpszClassName = cwin->wndClassName;
	wClass.lpfnWndProc = _csmWndProc;

	DWORD wFlags = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
	RECT clientRect = { 0, 0, width, height };
	AdjustWindowRectExForDpi(&clientRect, wFlags, TRUE, wFlags,
		GetDpiForSystem());
	DWORD realWinWidth = clientRect.right - clientRect.left;
	DWORD realWinHeight = clientRect.bottom - clientRect.top;

	RegisterClassA(&wClass);
	cwin->wnd = CreateWindowExA(0L, cwin->wndClassName, title,
		wFlags, CW_USEDEFAULT, CW_USEDEFAULT,
		realWinWidth, realWinHeight, NULL, NULL, NULL, NULL);

	ShowWindow(cwin->wnd, SW_SHOW);

	*pHandle = cwin;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CWindowSetTitle(CHandle window, PCHAR title) {
	_CSyncEnter();

	if (window == NULL) {
		_CSyncLeaveErr(FALSE, "CWindowSetTitle failed because window was invalid");
	}

	PCWindow cwin = window;
	SetWindowTextA(cwin->wnd, title);

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CWindowSetDimensions(CHandle window, INT width, INT height) {
	_CSyncEnter();

	if (window == NULL) {
		_CSyncLeaveErr(FALSE, "CWindowSetDimensions failed because window was invalid");
	}

	if (width <= 0 || height <= 0) {
		_CSyncLeaveErr(FALSE, "CWindowSetDimensions failed because dimensions were invalid");
	}

	PCWindow cwin = window;
	SetWindowPos(cwin->wnd, NULL, 0, 0, width, height, SWP_NOMOVE);

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CWindowSetRenderBuffer(CHandle window, CHandle rHandle) {
	_CSyncEnter();

	if (window == NULL) {
		_CSyncLeaveErr(FALSE, "CWindowSetRenderBuffer failed because window was invalid");
	}

	PCRenderBuffer crb = rHandle;
	PCWindow cwin = window;

	cwin->renderBuff = crb;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CWindowShouldClose(CHandle window) {
	_CSyncEnter();
	if (window == NULL) {
		_CSyncLeaveErr(FALSE, "CWindowShouldClose failed because window was invalid");
	}

	PCWindow cwin = window;
	_CSyncLeave(cwin->shouldClose);
}

CSMCALL BOOL CWindowUpdate(CHandle window) {
	_CSyncEnter();

	if (window == NULL) {
		_CSyncLeaveErr(FALSE, "CWindowUpdate failed because window was invalid");
	}

	PCWindow win = window;

	// force window refresh
	InvalidateRect(win->wnd, NULL, FALSE);
	UpdateWindow(win->wnd);

	// update messages
	MSG messageBuff;
	PeekMessageA(&messageBuff, win->wnd, ZERO, ZERO, PM_REMOVE);
	DispatchMessageA(&messageBuff);

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CDestroyWindow(PCHandle pHandle) {
	_CSyncEnter();

	if (pHandle == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyWindow failed because pHandle was NULL");
	}

	PCWindow win = *pHandle;
	if (win == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyWindow failed because pHandle was invalid");
	}

	SendMessageA(win->wnd, CSM_WINDOW_CLOSEMESSAGE, ZERO, ZERO);

	_CSyncLeave(TRUE);
}
