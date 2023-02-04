// Bailey Jia-Tao Brown
// 2023
// <csm_render.c>

#include "csm_render.h"
#include "csmint.h"

CSMCALL CHandle CMakeRenderInstance(void) {
	_CSyncEnter();
	PCRenderInstance pRi = CInternalAlloc(sizeof(CRenderInstance));
	_CSyncLeave(pRi);
}

CSMCALL BOOL CDestroyRenderInstance(PCHandle pInstance) {
	_CSyncEnter();
	if (pInstance == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyRenderInstance failed because pInstance was NULL");
	}

	CInternalFree(*pInstance);
	*pInstance = NULL;

	_CSyncLeave(TRUE);
}

CSMCALL CHandle CMakeVertexDataBuffer(void) {
	_CSyncEnter();
	PCVertexDataBuffer vDb = CInternalAlloc(sizeof(CVertexDataBuffer));
	_CSyncLeave(vDb);
}

CSMCALL BOOL CVertexDataBufferSetData(CHandle handle, PVOID data,
	UINT32 sizeBytes, UINT32 walkCount, UINT32 walkStrideBytes) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CVertexDataBufferSetData failed because handle was NULL");
	}

	PCVertexDataBuffer vdb = handle;
	vdb->data = data;
	vdb->sizeBytes = sizeBytes;
	vdb->walkCount = walkCount;
	vdb->walkStrideBytes = walkStrideBytes;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CDestroyVertexDataBuffer(PCHandle pHandle) {
	_CSyncEnter();

	if (pHandle == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyVertexDataBuffer failed because pHandle was NULL");
	}

	CInternalFree(*pHandle);
	*pHandle = NULL;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderInstanceSetBuffer(CHandle instance, CHandle buffer) {
	_CSyncEnter();

	if (instance == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceSetBuffer failed because instance was NULL");
	}
	
	PCRenderInstance pri = instance;
	pri->fBuff = buffer;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderInstanceSetShader(CHandle instance,
	PCFVertexShader vertShader, PCFFragmentShader fragshader) {
	_CSyncEnter();

	if (instance == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceSetShader failed because instance was NULL");
	}

	PCRenderInstance pri = instance;
	pri->vert = vertShader;
	pri->frag = fragshader;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderInstanceClearShader(CHandle instance) {
	_CSyncEnter();

	if (instance == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceClearShader failed because instance was NULL");
	}

	PCRenderInstance pri = instance;
	pri->vert = NULL;
	pri->frag = NULL;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderInstanceSetShaderInput(CHandle instance, UINT32 inputID,
	CHandle vertexDataInputBuffer) {
	_CSyncEnter();

	if (instance == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceSetShaderInput failed because instance was NULL");
	}

	if (inputID < 0 || inputID >= CSM_MAX_SHADER_INPUTS) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceSetShaderInput failed because inputID was invalid");
	}

	PCRenderInstance pri = instance;
	pri->shaderInputs[inputID] = vertexDataInputBuffer;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderInstanceClearShaderInput(CHandle instance, UINT32 inputID) {
	_CSyncEnter();

	if (instance == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceClearShaderInput failed because instance was NULL");
	}

	if (inputID < 0 || inputID >= CSM_MAX_SHADER_INPUTS) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceClearShaderInput failed because inputID was invalid");
	}

	PCRenderInstance pri = instance;
	pri->shaderInputs[inputID] = NULL;

	_CSyncLeave(TRUE);
}

CSMCALL CRenderVertex(CHandle instance, CVect3F vert, CRgb color);
CSMCALL CRenderVertexArray(PCRenderBuffer prB, UINT32 count, PCVect3F verts, CRgb color);
CSMCALL CRenderLines(PCRenderBuffer prB, UINT32 count, PCVect3F verts, CRgb color);
CSMCALL CRenderTriangle(PCRenderBuffer prB, PCVect3F verts, CRgb color);
CSMCALL CRenderMesh(PCRenderBuffer prB, CHandle mesh, CRgb color);
CSMCALL CRenderMeshEx(PCRenderBuffer prB, CHandle mesh, PCFVertexShader vert,
	PCFFragmentShader frag, PVOID input);