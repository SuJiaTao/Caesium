// <csm_vertex.h>
// Bailey JT Brown
// 2023

#ifndef _CSM_VERTEX_INCLUDE_
#define _CSM_VERTEX_INCLUDE_ 

#include "csm_renderclass.h"
#include "csm_draw.h"

#define CSM_MAX_VERTEX_OUTPUTS				0x10

CSMCALL BOOL	CVertexGetDrawInput(CHandle vertContext, UINT32 drawInputID, PVOID outBuffer);
CSMCALL SIZE_T	CVertexGetDrawInputSizeBytes(CHandle vertContext, UINT32 drawInputID);

CSMCALL BOOL	CVertexGetClassVertexData(CHandle vertContext, UINT32 ID, PFLOAT outBuffer);
CSMCALL UINT32	CVertexGetClassVertexDataComponentCount(CHandle vertContext, UINT32 ID);

CSMCALL BOOL	CVertexGetClassStaticData(CHandle vertContext, UINT32 ID, PFLOAT outBuffer);
CSMCALL SIZE_T	CVertexGetClassStaticDataSizeBytes(CHandle vertContext, UINT32 ID);

CSMCALL BOOL	CVertexSetVertexOutput(CHandle vertContext, UINT32 outputID,
	PFLOAT inBuffer, UINT32 components);
CSMCALL BOOL	CVertexSetVertexOutputFromClassVertexData(CHandle vertContext,
	UINT32 classVertID, UINT32 outputID);

#endif
