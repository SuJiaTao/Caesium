// Bailey Jia-Tao Brown
// 2023
// <csm_window.h>

#ifndef _CSM_WINDOW_INCLUDE_
#define _CSM_WINDOW_INCLUDE_

#include "csm.h"

#define CSM_WINDOW_CLASSNAME_SIZE	0xFF
#define CSM_MAX_WINDOWS				0x10
#define CSM_WINDOW_CLOSEMESSAGE		(WM_USER + 0x20)

typedef struct CWindow {
	PCHAR wndClassName;
	ATOM  wndClass;
	HWND  wnd;
	BOOL  shouldClose;

	CRenderBuffer renderBuff;
} CWindow, *PCWindow;

CSMCALL BOOL CMakeWindow(PCHandle pHandle, PCHAR title, INT width, INT height);
CSMCALL BOOL CWindowSetTitle(CHandle window, PCHAR title);
CSMCALL BOOL CWindowSetDimensions(CHandle window, INT width, INT height);
CSMCALL BOOL CWindowShouldClose(CHandle window);
CSMCALL BOOL CWindowUpdate(CHandle window);
CSMCALL BOOL CDestroyWindow(PCHandle pHandle);

#endif
