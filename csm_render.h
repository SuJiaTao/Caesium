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

typedef void (*PCFVertexShader  )(PCVect3F vert, PVOID input);
typedef BOOL (*PCFFragmentShader)(PCFragment frag, PVOID input);

#define CSM_MAX_SHADER_INPUTS 0x40

typedef struct CVertexDataBuffer {
	PVOID  data;
	UINT32 sizeBytes;
	UINT32 walkCount;
	UINT32 walkStrideBytes;
} CVertexDataBuffer, * PCVertexDataBuffer;

typedef struct CRenderInstance {
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
CSMCALL BOOL	CDestroyVertexDataBuffer(PCHandle pHandle);

CSMCALL BOOL CRenderInstanceSetBuffer(CHandle instance, CHandle renderBuffer);
CSMCALL BOOL CRenderInstanceSetShader(CHandle instance,
	PCFVertexShader vertShader, PCFFragmentShader fragshader);
CSMCALL BOOL CRenderInstanceClearShader(CHandle instance);
CSMCALL BOOL CRenderInstanceSetShaderInput(CHandle instance, UINT32 inputID,
	CHandle vertexDataInputBuffer);
CSMCALL BOOL CRenderInstanceClearShaderInput(CHandle instance, UINT32 inputID);

CSMCALL CRenderVertex(CHandle instance, CVect3F vert, CRgb color);
CSMCALL CRenderVertexArray(PCRenderBuffer prB, UINT32 count, PCVect3F verts, CRgb color);
CSMCALL CRenderLines(PCRenderBuffer prB, UINT32 count, PCVect3F verts, CRgb color);
CSMCALL CRenderTriangle(PCRenderBuffer prB, PCVect3F verts, CRgb color);
CSMCALL CRenderMesh(PCRenderBuffer prB, CHandle mesh, CRgb color);
CSMCALL CRenderMeshEx(PCRenderBuffer prB, CHandle mesh, PCFVertexShader vert,
	PCFFragmentShader frag, PVOID input);

#endif
