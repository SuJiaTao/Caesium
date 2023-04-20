// <csm_fragment.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSM_FRAGMENT_INCLUDE_
#define _CSM_FRAGMENT_INCLUDE_

#include "csm_renderclass.h"

typedef enum CSampleType {
	CSampleType_Clamp,
	CSampleType_Repeat
} CSampleType;

CSMCALL CColor	CFragmentConvertFloat3ToColor(FLOAT r, FLOAT g, FLOAT b);
CSMCALL CColor	CFragmentConvertVect3ToColor(CVect3F vect3);
CSMCALL CVect3F CFragmentConvertColorToVect3(CColor color);

CSMCALL BOOL	CFragmentGetVertexInput(CHandle fragContext, UINT32 inputID, PFLOAT outBuffer);
CSMCALL BOOL	CFragmentGetStaticInput(CHandle fragContext, UINT32 inputID, PVOID outBuffer);

CSMCALL BOOL	CFragmentSampleRenderBuffer(PCColor inOutColor, CHandle renderBuffer, 
	CVect2F uv, CSampleType sampleType);

#endif
