// Bailey Jia-Tao Brown
// 2023
// <csm.h>
// Caesium Software Renderer

#ifndef _CSM_INCLUDE_
#define _CSM_INCLUDE_ 

#ifdef CAESIUM_EXPORTS
#define CSMCALL __declspec(dllexport)
#else
#define CSMCALL __declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define BIT_WITH_LEFT_OFFSET(x)			  1 << x
#define ANTI_BIT_WITH_LEFT_OFFSET(x)	~(1 << x)

#define ZERO 0

typedef PVOID* PPVOID;

typedef PVOID CHandle;
typedef CHandle* PCHandle;

typedef struct CVect2I {
	INT32 x, y;
} CVect2I, *PCVect2I;

typedef struct CVect2F {
	FLOAT x, y;
} CVect2F, *PCVect2F;

typedef struct CVect3F {
	FLOAT x, y, z;
} CVect3F, *PCVect3F;

typedef struct CVect4F {
	FLOAT x, y, z, w;
} CVect4F, *PCVect4F;

typedef struct CRgb {
	BYTE r, g, b;
} CRgb, *PCRgb;

CSMCALL BOOL CInitialize();
CSMCALL BOOL CTerminate();

CSMCALL PCHAR CGetLastError();

CSMCALL void CLock();
CSMCALL void CUnlock();

#include "csm_renderbuffer.h"
#include "csm_window.h"
#include "csm_mesh.h"
#include "csm_matrix.h"
#include "csm_render.h"

#endif
