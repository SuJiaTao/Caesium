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
		_CSyncLeaveErr(FALSE, "CVertexDataBufferSetData failed because handle was invalid");
	}

	PCVertexDataBuffer vdb = handle;
	
	// if data is pre-existing, clear it
	if (vdb->data != NULL) 
		CInternalFree(vdb->data);

	// allocate
	vdb->sizeBytes = sizeBytes;
	vdb->data = CInternalAlloc(vdb->sizeBytes);

	// copy passed data (if exists)
	if (data != NULL) {
		COPY_BYTES(data, vdb->data, vdb->sizeBytes);
	}

	// setup other metadata
	vdb->walkCount = walkCount;
	vdb->walkStrideBytes = walkStrideBytes;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CVertexDataBufferEditData(CHandle handle, PCFVertexDataEdit editFunc) {
	_CSyncEnter();

	if (handle == NULL) {
		_CSyncLeaveErr(FALSE, "CVertexDataBufferEditData failed because handle was invalid");
	}

	if (editFunc == NULL) {
		_CSyncLeaveErr(FALSE, "CVertexDataBufferEditData failed because editFunc was NULL");
	}

	PCVertexDataBuffer vdb = handle;
	editFunc(vdb->data);

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
		_CSyncLeaveErr(FALSE, "CRenderInstanceSetBuffer failed because instance was invalid");
	}
	
	PCRenderInstance pri = instance;
	pri->fBuff = buffer;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderInstanceSetShader(CHandle instance,
	PCFVertexShader vertShader, PCFFragmentShader fragshader) {
	_CSyncEnter();

	if (instance == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceSetShader failed because instance was invalid");
	}

	PCRenderInstance pri = instance;
	pri->vert = vertShader;
	pri->frag = fragshader;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderInstanceClearShader(CHandle instance) {
	_CSyncEnter();

	if (instance == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceClearShader failed because instance was invalid");
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
		_CSyncLeaveErr(FALSE, "CRenderInstanceSetShaderInput failed because instance was invalid");
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
		_CSyncLeaveErr(FALSE, "CRenderInstanceClearShaderInput failed because instance was invalid");
	}

	if (inputID < 0 || inputID >= CSM_MAX_SHADER_INPUTS) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceClearShaderInput failed because inputID was invalid");
	}

	PCRenderInstance pri = instance;
	pri->shaderInputs[inputID] = NULL;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderInstanceSetRenderMode(CHandle instance, CRenderMode renderMode) {
	_CSyncEnter();

	if (instance == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceSetRenderMode failed because instance was invalid");
	}

	// not the best way to go about doing this
	if (renderMode != CRenderModePoints &&
		renderMode != CRenderModeLines &&
		renderMode != CRenderModeFilled) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceSetRenderMode failed because renderMode was invalid");
	}

	PCRenderInstance pri = instance;
	pri->renderMode = renderMode;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL CRenderInstanceDrawMesh(CHandle instance, CHandle mesh, PCMatrix offset) {
	CRenderInstanceDrawMeshInstanced(instance, mesh, offset, 1);
}

CSMCALL BOOL CRenderInstanceDrawMeshInstanced(CHandle instance, CHandle mesh,
	PCMatrix offset, UINT32 numMeshes) {
	_CSyncEnter();

	if (instance == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceDrawMesh failed because instance was invalid");
	}
	if (mesh == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceDrawMesh failed because mesh was invalid");
	}
	if (numMeshes <= 0) {
		_CSyncLeaveErr(FALSE, "CRenderInstanceDrawMesh failed because numMeshes was invalid");
	}

	PCRenderInstance rendInstance = instance;
	PCMesh meshObj = mesh;

	_CSyncLeave(TRUE);
}