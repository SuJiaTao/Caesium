// Bailey Jia-Tao Brown
// 2023
// <csm_render.h>

#ifndef _CSM_RENDER_INCLUDE_
#define _CSM_RENDER_INCLUDE_

#include "csm.h"
#include "csm_matrix.h"

#define CSM_CLASS_MAX_PER_VERTEX_DATA	0x10
#define CSM_CLASS_MAX_STATIC_DATA		0x20
#define CSM_CLASS_MAX_MATERIALS			0x08
#define CSM_CLASS_DATA_BUFFER_BAD_ID	0xFFFF

typedef CVect3F (*PCFVertexShaderProc) (
		CVect3F vertexPosition, 
		UINT32  vertexID,
		UINT32  triangleID,
		CMatrix transform,
		CHandle renderClass
	);

typedef BOOL (*PCFFragmentShaderProc) (
		UINT32   triangleID,
		PCVect3F inOutFragPos,
		PCRgb	 inOutColor
	);

typedef struct CMatrix (*PCFClusterMatrixProc) (
		UINT32  clusterID, 
		CMatrix parentTransform,
		CHandle renderClass
	);

typedef struct CVertexDataBuffer {
	PCHAR  name;
	SIZE_T elementSizeBytes;
	UINT32 elementCount;
	PVOID  data;
} CVertexDataBuffer, *PCVertexDataBuffer;

typedef struct CStaticDataBuffer {
	CRITICAL_SECTION mapLock;
	PCHAR  name;
	SIZE_T sizeBytes;
	PVOID  data;
} CStaticDataBuffer, *PCStaticDataBuffer;

typedef struct CMaterial {
	PCHAR name;
	PCFVertexShaderProc   vertexShader;
	PCFFragmentShaderProc fragmentShader;
} CMaterial, *PCMaterial;

typedef struct CRenderClass {
	PCHAR   name;
	CHandle mesh;
	CHandle vertexBuffers	 [CSM_CLASS_MAX_PER_VERTEX_DATA];
	CHandle	staticDataBuffer [CSM_CLASS_MAX_STATIC_DATA];
	CHandle materials		 [CSM_CLASS_MAX_MATERIALS];
	BOOL	singleMaterial;
	PUINT32	triMaterials;
} CRenderClass, *PCRenderClass;

typedef struct CRenderObject {
	PCRenderClass renderClass;
	CMatrix		  transform;
} CRenderObject, *PCRenderObject;

typedef struct CRenderCluster {
	PCRenderClass renderClass;
	CMatrix		  parent;
	PCFClusterMatrixProc matrixGenProc;
} CRenderCluster, *PCRenderCluster;

CSMCALL CHandle CMakeVertexDataBuffer(PCHAR name, SIZE_T elementSizeBytes,
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

CSMCALL CHandle CMakeMaterial(PCHAR name, 
	PCFVertexShaderProc vertexShader,
	PCFFragmentShaderProc fragmentShader);
CSMCALL BOOL	CDestroyMaterial(PCHandle pMatHandle);

CSMCALL CHandle CMakeRenderClass(PCHAR name, CHandle mesh, CHandle material);
CSMCALL BOOL	CDestroyRenderClass(PCHandle pClass);

CSMCALL BOOL	CRenderClassGetName(PCHAR stroutBuffer, SIZE_T maxWrite);
CSMCALL CHandle CRenderClassGetMesh(CHandle rClass);

CSMCALL BOOL	CRenderClassSetMaterial(CHandle rClass, CHandle material, UINT32 ID);
CSMCALL CHandle CRenderClassGetMaterial(CHandle rClass, UINT32 ID);
CSMCALL UINT32	CRenderClassGetMaterialID(CHandle rClass, PCHAR name);

CSMCALL BOOL	CRenderClassSetTriMaterialSingle(CHandle rClass, BOOL state);
CSMCALL BOOL	CRenderClassGetTriMaterialSingle(CHandle rClass, PBOOL outState);
CSMCALL BOOL	CRenderClassSetTriMaterials(CHandle rClass, PUINT32 triMaterialIndexes);
CSMCALL CHandle	CRenderClassGetTriMaterial(CHandle rClass, UINT32 triMaterialIndex);

CSMCALL BOOL	CRenderClassSetVertexDataBuffer(CHandle rClass, UINT32 ID);
CSMCALL CHandle CRenderClassGetVertexDataBuffer(CHandle rClass, UINT32 ID);
CSMCALL UINT32  CRenderClassGetVertexDataBufferID(CHandle rClass, PCHAR name);

CSMCALL BOOL	CRenderClassSetStaticDataBuffer(PVOID data, SIZE_T sizeBytes, UINT32 ID);
CSMCALL CHandle CRenderClassGetStaticDataBuffer(CHandle rClass, UINT32 ID);
CSMCALL UINT32  CRenderClassGetStaticDataBufferID(CHandle rClass, PCHAR name);

CSMCALL 

#endif
