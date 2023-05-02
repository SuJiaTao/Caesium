// <csm_renderclass.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSM_RENDERCLASS_INCLUDE_
#define _CSM_RENDERCLASS_INCLUDE_

#include "csm_buffer.h"

#define CSM_CLASS_MAX_VERTEX_DATA		0x10
#define CSM_CLASS_MAX_STATIC_DATA		0x20
#define CSM_CLASS_MAX_MATERIALS			0x08
#define CSM_BAD_ID						~(0x0)

typedef CVect3F (*PCFVertexShaderProc) (
	CHandle vertContext,
	UINT32  instanceID,
	UINT32  vertexID,
	CVect3F vertexPosition
	);

typedef struct CFragPos {
	INT   x;
	INT	  y;
	FLOAT depth;
} CFragPos, *PCFragPos;

typedef BOOL (*PCFFragmentShaderProc) (
	CHandle  fragContext,
	UINT32   triangleID,
	UINT32   instanceID,
	CFragPos inFragPos,
	PCColor	 inOutColor
	);

typedef struct CMaterial {
	PCFVertexShaderProc   vertexShader;
	PCFFragmentShaderProc fragmentShader;
} CMaterial, * PCMaterial;

typedef struct CRenderClass {
	PCHAR   name;
	CHandle mesh;
	CHandle vertexBuffers[CSM_CLASS_MAX_VERTEX_DATA];
	CHandle	staticBuffers[CSM_CLASS_MAX_STATIC_DATA];
	CHandle material;
} CRenderClass, * PCRenderClass;

CSMCALL CHandle CMakeMaterial(PCFVertexShaderProc vertexShader,
	PCFFragmentShaderProc fragmentShader);
CSMCALL BOOL	CDestroyMaterial(PCHandle pMatHandle);

CSMCALL CHandle CMakeRenderClass(PCHAR name, CHandle mesh, CHandle material);
CSMCALL BOOL	CDestroyRenderClass(PCHandle pClass);

CSMCALL BOOL	CRenderClassGetName(CHandle rClass,
	PCHAR stroutBuffer, SIZE_T maxWrite);
CSMCALL CHandle CRenderClassGetMesh(CHandle rClass);

CSMCALL BOOL	CRenderClassSetMaterial(CHandle rClass, CHandle material);
CSMCALL CHandle CRenderClassGetMaterial(CHandle rClass);

CSMCALL BOOL	CRenderClassSetVertexDataBuffer(CHandle rClass, CHandle vdBuffer, UINT32 ID);
CSMCALL CHandle CRenderClassGetVertexDataBuffer(CHandle rClass, UINT32 ID);
CSMCALL UINT32  CRenderClassGetVertexDataBufferID(CHandle rClass, PCHAR name);

CSMCALL BOOL	CRenderClassSetStaticDataBuffer(CHandle rClass, CHandle sdBuffer, UINT32 ID);
CSMCALL CHandle CRenderClassGetStaticDataBuffer(CHandle rClass, UINT32 ID);
CSMCALL UINT32  CRenderClassGetStaticDataBufferID(CHandle rClass, PCHAR name);

#endif
