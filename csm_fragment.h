// <csm_fragment.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSM_FRAGMENT_INCLUDE_
#define _CSM_FRAGMENT_INCLUDE_

#include "csm_renderclass.h"

CSMCALL CRgb	CFragmentConvertFloat3ToColor(FLOAT r, FLOAT g, FLOAT b);
CSMCALL CRgb	CFragmentConvertVect3ToColor(CVect3F vect3);
CSMCALL CVect3F CFragmentConvertColorToVect3(CRgb color);

CSMCALL BOOL	CFragmentGetInput(CHandle fragContext, UINT32 inputID, PFLOAT outBuffer);

#endif
