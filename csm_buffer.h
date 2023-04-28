// Bailey Jia-Tao Brown
// 2023
// <csm_buffer.h>

#ifndef _CSM_BUFFER_INCLUDE_
#define _CSM_BUFFER_INCLUDE_

#include "csm.h"
#include "csm_matrix.h"

#define CSM_VERTEX_DATA_BUFFER_MAX_COMPONENTS	8

typedef struct CVertexDataBuffer {
	PCHAR  name;
	UINT32 elementCount;
	UINT32 elementComponents;
	PFLOAT data;
} CVertexDataBuffer, *PCVertexDataBuffer;

typedef struct CStaticDataBuffer {
	CRITICAL_SECTION mapLock;
	PCHAR  name;
	SIZE_T sizeBytes;
	PVOID  data;
} CStaticDataBuffer, *PCStaticDataBuffer;

CSMCALL CHandle CMakeVertexDataBuffer(PCHAR name, UINT32 elementCount, 
	UINT32 elementComponents, PFLOAT dataIn);
CSMCALL BOOL	CDestroyVertexDataBuffer(PCHandle pVertexDataBuffer);
CSMCALL BOOL	CVertexDataBufferGetElement(CHandle vdBuffer, UINT32 index,
	PFLOAT outBuffer);
CSMCALL BOOL	CVertexDataBufferUnsafeGetElement(CHandle vdBuffer, UINT32 index,
	PFLOAT outBuffer);
CSMCALL BOOL	CVertexDataBufferSetElement(CHandle vdBuffer, UINT32 index,
	PFLOAT inBuffer);
CSMCALL UINT32	CVertexDataBufferGetElementCount(CHandle vdBuffer);
CSMCALL UINT32	CVertexDataBufferGetComponentCount(CHandle vdBuffer);

CSMCALL CHandle CMakeStaticDataBuffer(PCHAR name, SIZE_T sizeBytes,
	PVOID dataIn);
CSMCALL BOOL	CDestroyStaticDataBuffer(PCHandle pStaticDataBuffer);
CSMCALL PVOID	CStaticDataBufferMap(CHandle sdBuffer);
CSMCALL void	CStaticDataBufferUnmap(CHandle sdBuffer);
CSMCALL SIZE_T	CStaticDataBufferGetSizeBytes(CHandle sdBuffer);

#endif
