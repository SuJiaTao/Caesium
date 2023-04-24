// <csm_fragment.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSM_FRAGMENT_INCLUDE_
#define _CSM_FRAGMENT_INCLUDE_

#include "csm_renderclass.h"

typedef enum CSampleType {
	CSampleType_Clamp,
	CSampleType_ClampToEdge,
	CSampleType_Repeat
} CSampleType;

CSMCALL CColor	CFragmentConvertFloat3ToColor(FLOAT r, FLOAT g, FLOAT b);
CSMCALL CColor	CFragmentConvertVect3ToColor(CVect3F vect3);
CSMCALL CVect3F CFragmentConvertColorToVect3(CColor color);

CSMCALL BOOL	CFragmentGetDrawInput(CHandle fragContext, UINT32 drawInputID, PVOID outBuffer);
CSMCALL SIZE_T	CFragmentGetDrawInputSizeBytes(CHandle fragContext, UINT32 drawInputID);

CSMCALL BOOL	CFragmentGetVertexOutput(CHandle fragContext, UINT32 outputID, PFLOAT outBuffer);
CSMCALL UINT32	CFragmentGetVertexOutputComponentCount(CHandle fragContext, UINT32 outputID);

CSMCALL BOOL	CFragmentGetClassStaticData(CHandle fragContext, UINT32 ID, PVOID outBuffer);
CSMCALL SIZE_T	CFragmentGetClassStaticDataSizeBytes(CHandle fragContext, UINT32 ID);

CSMCALL BOOL	CFragmentSampleRenderBuffer(PCColor inOutColor, CHandle renderBuffer, 
	CVect2F uv, CSampleType sampleType);

#endif
