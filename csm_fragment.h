// <csm_fragment.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSM_FRAGMENT_INCLUDE_
#define _CSM_FRAGMENT_INCLUDE_

#include "csm_renderclass.h"

CSMCALL CColor	CFragmentConvertFloat3ToColor(FLOAT r, FLOAT g, FLOAT b);
CSMCALL CColor	CFragmentConvertVect3ToColor(CVect3F vect3);
CSMCALL CVect3F CFragmentConvertColorToVect3(CColor color);

CSMCALL BOOL	CFragmentGetInput(CHandle fragContext, UINT32 inputID, PFLOAT outBuffer);

#endif
