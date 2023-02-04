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
} CMesh, *PCMesh;

CSMCALL BOOL CMakeMesh(PCHandle pHandle, PCMesh meshData);
CSMCALL BOOL CDestroyMesh(PCHandle handle);

#endif
