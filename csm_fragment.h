// <csm_fragment.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSM_FRAGMENT_INCLUDE_
#define _CSM_FRAGMENT_INCLUDE_

#include "csm_renderclass.h"

CSMCALL FLOAT   CFragmentInterpolateFloat(CHandle fragContext,
	FLOAT p1f, FLOAT p2f, FLOAT p3f);
CSMCALL void	CFragmentInterpolateFloats(CHandle fragContext,
	UINT32 count, PFLOAT pa1f, PFLOAT pa2f, PFLOAT pa3f, PFLOAT result);
CSMCALL CVect2F CFragmentInterpolateVect2F(CHandle fragContext,
	CVect2F p1v2, CVect2F p2v2, CVect2F p3v3);
CSMCALL CVect3F CFragmentInterpolateVect3F(CHandle fragContext,
	CVect3F p1v3, CVect3F p2v3, CVect3F p3v3);
CSMCALL CRgb	CFragmentInterpolateColor(CHandle fragContext,
	CRgb p1c, CRgb p2c, CRgb p3c);

#endif
