// <csm_render.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSM_RENDERCLASS_INCLUDE_
#define _CSM_RENDERCLASS_INCLUDE_

#include "csm_buffer.h"

#define CSM_CLASS_MAX_VERTEX_DATA		0x10
#define CSM_CLASS_MAX_STATIC_DATA		0x20
#define CSM_CLASS_MAX_MATERIALS			0x08
#define CSM_CLASS_DATA_BUFFER_BAD_ID	0xFFFF

typedef CVect3F(*PCFVertexShaderProc) (
	CVect3F vertexPosition,
	UINT32  vertexID,
	UINT32  triangleID,
	CMatrix transform,
	CHandle renderClass
	);

typedef BOOL(*PCFFragmentShaderProc) (
	UINT32   triangleID,
	PCVect3F inOutFragPos,
	PCRgb	 inOutColor
	);

typedef struct CMatrix(*PCFClusterMatrixProc) (
	UINT32  clusterID,
	CMatrix parentTransform,
	CHandle renderClass
	);

typedef struct CMaterial {
	PCHAR name;
	PCFVertexShaderProc   vertexShader;
	PCFFragmentShaderProc fragmentShader;
} CMaterial, * PCMaterial;

typedef struct CRenderClass {
	PCHAR   name;
	CHandle mesh;
	CHandle vertexBuffers[CSM_CLASS_MAX_VERTEX_DATA];
	CHandle	staticDataBuffer[CSM_CLASS_MAX_STATIC_DATA];
	CHandle materials[CSM_CLASS_MAX_MATERIALS];
	BOOL	singleMaterial;
	PUINT32	triMaterials;
} CRenderClass, * PCRenderClass;

typedef struct CRenderObject {
	PCRenderClass renderClass;
	CMatrix		  transform;
} CRenderObject, * PCRenderObject;

typedef struct CRenderCluster {
	PCRenderClass renderClass;
	CMatrix		  parent;
	PCFClusterMatrixProc matrixGenProc;
} CRenderCluster, * PCRenderCluster;

#endif
