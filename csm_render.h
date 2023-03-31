// Bailey Jia-Tao Brown
// 2023
// <csm_render.h>

#ifndef _CSM_RENDER_INCLUDE_
#define _CSM_RENDER_INCLUDE_

#include "csm.h"
#include "csm_matrix.h"

#define CSM_CLASS_MAX_PER_VERTEX_DATA 0x10
#define CSM_CLASS_MAX_GLOBAL_DATA	  0x20
#define CSM_CLASS_MAX_MATERIALS		  0x10

typedef CVect3F (*PCFVertexShaderProc)  (CVect3F vertexIn, UINT32 vertexID, 
	CMatrix transform, struct CRenderClass* renderClass);
typedef BOOL    (*PCFFragmentShaderProc)(struct CFragment* fragment);
typedef struct CMatrix	(*PCFClusterMatrixProc) (UINT32 clusterID, CMatrix transform,
	struct CRenderClass* renderClass, PVOID userData);

typedef struct CFragment {
	CVect3F fragPos;
	CRgb	color;
} CFragment, *PCFragment;

typedef struct CDataBuffer {
	PCHAR  name;
	SIZE_T elementSizeBytes;
	UINT32 elementCount;
	PVOID  data;
} CDataBuffer, *PCDataBuffer;

typedef struct CMaterial {
	PCFVertexShaderProc   vertexShader;
	PCFFragmentShaderProc fragmentShader;
} CMaterial, *PCMaterial;

typedef struct CRenderClass {
	CHandle mesh;
	CHandle materials	[CSM_CLASS_MAX_MATERIALS];
	CHandle dataBuffers [CSM_CLASS_MAX_PER_VERTEX_DATA];
	BOOL	singleMaterial;
	UINT32	triMaterialCount;
	PUINT8	triMaterials;
} CRenderClass, *PCRenderClass;

typedef struct CRenderObject {
	PCRenderClass  renderClass;
	CMatrix		   transform;
} CRenderObject, *PCRenderObject;

typedef struct CRenderCluster {
	PCRenderClass  renderClass;
	PVOID		   userData;
	PCFClusterMatrixProc matrixGenProc;
} CRenderCluster, *PCRenderCluster;

#endif
