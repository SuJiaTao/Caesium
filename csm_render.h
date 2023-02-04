// Bailey Jia-Tao Brown
// 2023
// <csm_render.h>

#ifndef _CSM_RENDER_INCLUDE_
#define _CSM_RENDER_INCLUDE_

#include "csm.h"

typedef struct CFragment {
	PCVect2I vertPosList[3];
	PCVect2I screenPos;
	CRgb     color;
	FLOAT    depth;
} CFragment, *PCFragment;

typedef void (*PCFVertexDataEdit)(PVOID data);
typedef void (*PCFVertexShader  )(PCVect3F vert, UINT32 vertID, PVOID input);
typedef BOOL (*PCFFragmentShader)(PCFragment frag, PVOID input);

#define CSM_MAX_SHADER_INPUTS 0x40

typedef enum CRenderMode {
	CRenderModePoints	= 0,
	CRenderModeLines	= 1,
	CRenderModeFilled	= 2
} CRenderMode, *PCRenderMode;

typedef struct CVertexDataBuffer {
	PVOID  data;
	UINT32 sizeBytes;
	UINT32 walkCount;
	UINT32 walkStrideBytes;
} CVertexDataBuffer, * PCVertexDataBuffer;

typedef struct CRenderInstance {
	CRenderMode		  renderMode;
	PCRenderBuffer    fBuff;
	PCFVertexShader   vert;
	PCFFragmentShader frag;
	CHandle shaderInputs[CSM_MAX_SHADER_INPUTS];
} CRenderInstance, * PCRenderInstance;

CSMCALL CHandle CMakeRenderInstance(void);
CSMCALL BOOL    CDestroyRenderInstance(PCHandle pInstance);

CSMCALL CHandle CMakeVertexDataBuffer(void);
CSMCALL BOOL	CVertexDataBufferSetData(CHandle handle, PVOID data, 
	UINT32 sizeBytes, UINT32 walkCount, UINT32 walkStrideBytes);
CSMCALL BOOL	CVertexDataBufferEditData(CHandle handle, PCFVertexDataEdit editFunc);
CSMCALL BOOL	CDestroyVertexDataBuffer(PCHandle pHandle);

CSMCALL BOOL CRenderInstanceSetBuffer(CHandle instance, CHandle renderBuffer);
CSMCALL BOOL CRenderInstanceSetShader(CHandle instance,
	PCFVertexShader vertShader, PCFFragmentShader fragshader);
CSMCALL BOOL CRenderInstanceClearShader(CHandle instance);
CSMCALL BOOL CRenderInstanceSetShaderInput(CHandle instance, UINT32 inputID,
	CHandle vertexDataInputBuffer);
CSMCALL BOOL CRenderInstanceClearShaderInput(CHandle instance, UINT32 inputID);
CSMCALL BOOL CRenderInstanceSetRenderMode(CHandle instance, CRenderMode renderMode);

CSMCALL BOOL CRenderInstanceDrawMesh(CHandle instance, CHandle mesh, struct CMatrix* offset);
CSMCALL BOOL CRenderInstanceDrawMeshInstanced(CHandle instance, CHandle mesh,
	struct CMatrix* offset, UINT32 numMeshes);

#endif
