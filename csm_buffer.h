// Bailey Jia-Tao Brown
// 2023
// <csm_buffer.h>

#ifndef _CSM_BUFFER_INCLUDE_
#define _CSM_BUFFER_INCLUDE_

#include "csm.h"
#include "csm_matrix.h"

typedef enum CVertexDataBufferType {
	CVertexDataBufferType_Float,
	CVertexDataBufferType_CVect2F,
	CVertexDataBufferType_CVect3F,
	CVertexDataBufferType_CVect4F,
	CVertexDataBufferType_CRgb,
	CVertexDataBufferType_Error
} CVertexDataBufferType;

typedef struct CVertexDataBuffer {
	CVertexDataBufferType type;
	PCHAR  name;
	UINT32 elementCount;
	PVOID  data;
} CVertexDataBuffer, *PCVertexDataBuffer;

typedef struct CStaticDataBuffer {
	CRITICAL_SECTION mapLock;
	PCHAR  name;
	SIZE_T sizeBytes;
	PVOID  data;
} CStaticDataBuffer, *PCStaticDataBuffer;

CSMCALL CHandle CMakeVertexDataBuffer(PCHAR name, CVertexDataBufferType type,
	UINT32 elementCount, PVOID dataIn);
CSMCALL BOOL	CDestroyVertexDataBuffer(PCHandle pVertexDataBuffer);
CSMCALL BOOL	CVertexDataBufferGetElement(CHandle vdBuffer, UINT32 index,
	PVOID outBuffer);
CSMCALL BOOL	CVertexDataBufferSetElement(CHandle vdBuffer, UINT32 index,
	PVOID inBuffer);

CSMCALL CHandle CMakeStaticDataBuffer(PCHAR name, SIZE_T sizeBytes,
	PVOID dataIn);
CSMCALL BOOL	CDestroyStaticDataBuffer(PCHandle pStaticDataBuffer);
CSMCALL PVOID	CStaticDataBufferMap(CHandle sdBuffer);
CSMCALL void	CStaticDataBufferUnmap(CHandle sdBuffer);

#endif
