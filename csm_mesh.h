// Bailey Jia-Tao Brown
// 2023
// <csm_mesh.h>

#ifndef _CSM_MESH_INCLUDE_
#define _CSM_MESH_INCLUDE_ 

#include "csm.h"

typedef struct CMesh {
	UINT32   vertCount;
	PCVect3F verts;

	UINT32 indexCount;
	PINT   indexes;

	UINT32 triCount;
} CMesh, *PCMesh;

CSMCALL BOOL   CMakeMesh(PCHandle pHandle, UINT32 vertexCount,
	PFLOAT vertPositionalArray, UINT32 indexCount, PINT indexes);
CSMCALL BOOL   CDestroyMesh(PCHandle handle);
CSMCALL UINT32 CMeshGetTriCount(CHandle handle);
CSMCALL UINT32 CMeshGetVertCount(CHandle handle);

#endif
