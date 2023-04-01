// <csm_render.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint.h"
#include "csm_render.h"

static __forceinline _initializeAndCopyString(PCHAR source, PCHAR* destPtr) {
	const SIZE_T srcLen = strlen(source);
	*destPtr = CInternalAlloc(srcLen);
	COPY_BYTES(source, *destPtr, srcLen);
}

CSMCALL CHandle CMakeMaterial(PCHAR name,
	PCFVertexShaderProc vertexShader,
	PCFFragmentShaderProc fragmentShader) {
	_CSyncEnter();

	if (name == NULL) {
		_CSyncLeaveErr(NULL, "CMakeMaterial failed because name was NULL");
	}

	PCMaterial mat = CInternalAlloc(sizeof(CMaterial));
	_initializeAndCopyString(name, &mat->name);

	mat->vertexShader = vertexShader;
	mat->fragmentShader = fragmentShader;

	_CSyncLeave(mat);
}

CSMCALL BOOL	CDestroyMaterial(PCHandle pMatHandle) {
	if (pMatHandle == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyMaterial failed because pMatHandle was NULL");
	}

	PCMaterial mat = *pMatHandle;
	if (mat == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyMaterial failed because pMatHandle was invalid");
	}

	CInternalFree(mat->name);
	CInternalFree(mat);
	*pMatHandle = NULL;

	_CSyncLeave(TRUE);
}

CSMCALL CHandle CMakeRenderClass(PCHAR name, CHandle mesh, CHandle material) {
	_CSyncEnter();

	if (name == NULL) {
		_CSyncLeaveErr(NULL, "CMakerRenderClass failed because name was NULL");
	}

	if (mesh == NULL) {
		_CSyncLeaveErr(NULL, "CMakerRenderClass failed because mesh was NULL");
	}

	PCRenderClass rClass = CInternalAlloc(sizeof(CRenderClass));
	_initializeAndCopyString(name, &rClass->name); // init name

	// apply mesh
	rClass->mesh = mesh;

	// set default material (0) to input material (NULL is acceptable)
	rClass->materials[0] = material;

	// by default, class with use singleMaterial (only materials[0] is used for ALL tris)
	rClass->singleMaterial = TRUE;
	
	_CSyncLeave(rClass);
}

CSMCALL BOOL	CDestroyRenderClass(PCHandle pClass) {
	_CSyncEnter();

	if (pClass == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyRenderClass failed because pClass was NULL");
	}

	PCRenderClass rClass = *pClass;

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyRenderClass failed because pClass was invalid");
	}

	CInternalFree(rClass->name);
	if (rClass->triMaterials != NULL)
		CInternalFree(rClass->triMaterials);
	CInternalFree(rClass);

	// set handle to NULL
	*pClass = NULL;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL	CRenderClassGetName(CHandle rClass,
	PCHAR stroutBuffer, SIZE_T maxWrite) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderClassGetName failed because rClass was invalid");
	}

	PCRenderClass cObj = rClass;

	COPY_BYTES(
		cObj->name, 
		stroutBuffer, 
		min(strlen(cObj->name), maxWrite)
	);

	_CSyncLeave(TRUE);
}

CSMCALL CHandle CRenderClassGetMesh(CHandle rClass) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderClassGetMesh failed because rClass was invalid");
	}

	PCRenderClass cObj = rClass;
	_CSyncLeave(cObj->mesh);
}