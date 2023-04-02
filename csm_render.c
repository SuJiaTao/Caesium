// <csm_render.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint.h"
#include "csm_mesh.h"
#include "csm_render.h"

static __forceinline _initializeAndCopyString(PCHAR source, PCHAR* destPtr) {
	const SIZE_T srcLen = strlen(source);
	*destPtr = CInternalAlloc(srcLen + 1); //+1 for NULL terminator
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
	// NOTE: Mesh is FIXED once class has been made
	rClass->mesh = mesh;

	// set default material (0) to input material (NULL is acceptable)
	rClass->materials[0] = material;

	// by default, class with use singleMaterial (only materials[0] is used for ALL tris)
	rClass->singleMaterial = TRUE;

	// triMaterial array is STILL initialized just to keep things simple
	PCMesh realMesh = mesh;
	rClass->triMaterials
		= CInternalAlloc(sizeof(UINT32) * realMesh->triCount);
	
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
		_CSyncLeaveErr(NULL, "CRenderClassGetMesh failed because rClass was invalid");
	}

	PCRenderClass cObj = rClass;
	_CSyncLeave(cObj->mesh);
}

CSMCALL BOOL	CRenderClassSetMaterial(CHandle rClass, CHandle material, UINT32 ID) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE, "CRenderClassSetMaterial failed because rClass was invalid");
	}
	if (ID >= CSM_CLASS_MAX_MATERIALS) {
		_CSyncLeaveErr(FALSE, "CRenderClassSetMaterial failed because ID was invalid");
	}

	PCRenderClass cObj = rClass;
	cObj->materials[ID] = material;

	_CSyncLeave(TRUE);
}

CSMCALL CHandle CRenderClassGetMaterial(CHandle rClass, UINT32 ID) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(NULL, "CRenderClassGetMaterial failed because rClass was invalid");
	}
	if (ID >= CSM_CLASS_MAX_MATERIALS) {
		_CSyncLeaveErr(NULL, "CRenderClassGetMaterial failed because ID was invalid");
	}

	PCRenderClass cObj = rClass;

	_CSyncLeave(cObj->materials[ID]);
}

CSMCALL UINT32	CRenderClassGetMaterialID(CHandle rClass, PCHAR name) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(CSM_BAD_ID, 
			"CRenderClassGetMaterialID failed because rClass was invalid");
	}
	if (name == NULL) {
		_CSyncLeaveErr(CSM_BAD_ID, 
			"CRenderClassGetMaterialID failed because name was NULL");
	}

	PCRenderClass cObj = rClass;

	// check each material within class
	for (UINT32 matID = 0; matID < CSM_CLASS_MAX_MATERIALS; matID++) {
		PCMaterial mat = cObj->materials[matID];

		// if material is NULL, skip
		if (mat == NULL) continue;

		// if string are the same, return ID
		if (strcmp(name, mat->name) == 0) {
			_CSyncLeave(matID);
		}
	}

	_CSyncLeaveErr(CSM_BAD_ID,
		"CRenderClassGetMaterialID failed because name could not be found");
}

CSMCALL BOOL	CRenderClassSetTriMaterialSingle(CHandle rClass, BOOL state) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE, 
			"CRenderClassSetTriMaterialSingle failed because rClass was invalid");
	}

	PCRenderClass cObj = rClass;
	cObj->singleMaterial = state;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL	CRenderClassGetTriMaterialSingle(CHandle rClass, PBOOL outState) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetTriMaterialSingle failed because rClass was invalid");
	}

	PCRenderClass cObj = rClass;
	*outState = cObj->singleMaterial;

	_CSyncLeave(TRUE);
}

CSMCALL BOOL	CRenderClassSetTriMaterials(CHandle rClass, PUINT32 triMaterialIndexes) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassSetTriMaterials failed because rClass was invalid");
	}
	if (triMaterialIndexes == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassSetTriMaterials failed because triMaterialIndexes was NULL");
	}

	PCRenderClass cObj = rClass;
	PCMesh objMesh = cObj->mesh;
	COPY_BYTES(triMaterialIndexes, cObj->triMaterials, sizeof(UINT32) * objMesh->triCount);

	_CSyncLeave(TRUE);
}

CSMCALL CHandle	CRenderClassGetTriMaterial(CHandle rClass, UINT32 triMaterialIndex) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetTriMaterial failed because rClass was invalid");
	}

	PCRenderClass cObj = rClass;
	PCMesh objMesh = cObj->mesh;

	if (triMaterialIndex >= objMesh->triCount) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetTriMaterial failed because triMaterialIndex was invalid");
	}

	// get ID from triMaterialList and check if it's valid
	UINT32 ID = cObj->triMaterials[triMaterialIndex];

	if (ID >= CSM_CLASS_MAX_MATERIALS) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetTriMaterial failed because class triMaterials had bad value");
	}

	// return handle to material that ID references
	_CSyncLeave(cObj->materials[ID]);
}

CSMCALL UINT32  CRenderClassGetTriMaterialID(CHandle rClass, UINT32 triMaterialIndex) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetTriMaterial failed because rClass was invalid");
	}

	PCRenderClass cObj = rClass;
	PCMesh objMesh = cObj->mesh;

	if (triMaterialIndex >= objMesh->triCount) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetTriMaterial failed because triMaterialIndex was invalid");
	}

	// get ID from triMaterialList and return
	UINT32 ID = cObj->triMaterials[triMaterialIndex];
	_CSyncLeave(ID);
}

CSMCALL BOOL	CRenderClassSetVertexDataBuffer(CHandle rClass, CHandle vdBuffer, UINT32 ID) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassSetVertexDataBuffer failed because rClass was invalid");
	}

	if (vdBuffer == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassSetVertexDataBuffer failed because vdBuffer was invalid");
	}

	if (ID >= CSM_CLASS_MAX_VERTEX_DATA) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassSetVertexDataBuffer failed because ID was invalid");
	}

	PCRenderClass cObj = rClass;
	cObj->vertexBuffers[ID] = vdBuffer;

	_CSyncLeave(TRUE);
}

CSMCALL CHandle CRenderClassGetVertexDataBuffer(CHandle rClass, UINT32 ID) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetVertexDataBuffer failed because rClass was invalid");
	}

	if (ID >= CSM_CLASS_MAX_VERTEX_DATA) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetVertexDataBuffer failed because ID was invalid");
	}

	PCRenderClass cObj = rClass;

	_CSyncLeave(cObj->vertexBuffers[ID]);
}

CSMCALL UINT32  CRenderClassGetVertexDataBufferID(CHandle rClass, PCHAR name) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetVertexDataBufferID failed because rClass was invalid");
	}

	if (name == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetVertexDataBufferID failed because name was NULL");
	}

	PCRenderClass cObj = rClass;

	// loop all vertex buffers of cObj
	for (UINT32 vbID = 0; vbID < CSM_CLASS_MAX_VERTEX_DATA; vbID++) {
		PCVertexDataBuffer vBuffer = cObj->vertexBuffers[vbID];
		// skip if NULL
		if (vBuffer == NULL) continue;

		// if names are matching, return ID
		if (strcmp(vBuffer->name, name) == 0) {
			_CSyncLeave(vbID);
		}
	}

	// on reached end, return err
	_CSyncLeaveErr(FALSE,
		"CRenderClassGetVertexDataBufferID failed because name could not be found");
}

CSMCALL BOOL	CRenderClassSetStaticDataBuffer(CHandle rClass, CHandle sdBuffer, UINT32 ID) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassSetStaticDataBuffer failed because rClass was invalid");
	}

	if (sdBuffer == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassSetStaticDataBuffer failed because sdBuffer was invalid");
	}

	if (ID >= CSM_CLASS_MAX_STATIC_DATA) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassSetStaticDataBuffer failed because ID was invalid");
	}

	PCRenderClass cObj = rClass;
	cObj->staticBuffers[ID] = sdBuffer;

	_CSyncLeave(TRUE);
}

CSMCALL CHandle CRenderClassGetStaticDataBuffer(CHandle rClass, UINT32 ID) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetStaticDataBuffer failed because rClass was invalid");
	}

	if (ID >= CSM_CLASS_MAX_STATIC_DATA) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetStaticDataBuffer failed because ID was invalid");
	}

	PCRenderClass cObj = rClass;

	_CSyncLeave(cObj->staticBuffers[ID]);
}

CSMCALL UINT32  CRenderClassGetStaticDataBufferID(CHandle rClass, PCHAR name) {
	_CSyncEnter();

	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetStaticDataBufferID failed because rClass was invalid");
	}

	if (name == NULL) {
		_CSyncLeaveErr(FALSE,
			"CRenderClassGetStaticDataBufferID failed because name was NULL");
	}

	PCRenderClass cObj = rClass;

	// loop all static buffers of cObj
	for (UINT32 sbID = 0; sbID < CSM_CLASS_MAX_STATIC_DATA; sbID++) {
		PCStaticDataBuffer sBuffer = cObj->staticBuffers[sbID];
		// skip if NULL
		if (sBuffer == NULL) continue;

		// if names are matching, return ID
		if (strcmp(sBuffer->name, name) == 0) {
			_CSyncLeave(sbID);
		}
	}

	// on reached end, return err
	_CSyncLeaveErr(FALSE,
		"CRenderClassGetStaticDataBufferID failed because name could not be found");
}
