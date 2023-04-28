// Bailey Jia-Tao Brown
// 2023
// <csm.h>

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

typedef struct CVect2F {
	FLOAT x, y;
} CVect2F, *PCVect2F;

typedef struct CVect3F {
	FLOAT x, y, z;
} CVect3F, *PCVect3F;

typedef struct CVect4F {
	FLOAT x, y, z, w;
} CVect4F, *PCVect4F;

typedef struct CColor {
	BYTE b, g, r, a;
} CColor, *PCColor;

CSMCALL BOOL CInitialize();
CSMCALL BOOL CTerminate();

CSMCALL PVOID CAlloc(SIZE_T size);
CSMCALL void  CFree(PVOID block);

CSMCALL PCHAR CGetLastError(void);
CSMCALL void  CFreeError(PCHAR error);
CSMCALL void  CLogErrors(BOOL state);

CSMCALL CColor CMakeColor3(INT r, INT g, INT b);
CSMCALL CColor CMakeColor4(INT r, INT g, INT b, INT a);

CSMCALL void CThreadSafe(BOOL state);
CSMCALL void CLock();
CSMCALL void CUnlock();

#endif
