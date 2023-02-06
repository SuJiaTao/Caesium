// Bailey Jia-Tao Brown
// 2023
// <csmint_draw.c>

#include "csmint_draw.h"

void CInternalDrawProcessMesh(PCRenderInstance instance, PCMesh mesh) {
	// if no vertex shader, quit
	if (instance->vert == NULL) return;

	for (int i = 0; i < mesh->vertCount; i++) {
		// build shader input table
		PVOID* inputTable = CInternalAlloc(sizeof(PVOID) * CSM_MAX_SHADER_INPUTS);

		for (INT64 sinp = 0; sinp < CSM_MAX_SHADER_INPUTS; sinp++) {
			// get vbd and data buffer
			PCVertexDataBuffer vdb = instance->shaderInputs[sinp];

			// if empty, skip
			if (vdb == NULL)
				continue;

			// calculate byte offset and set to table
			PUINT8 pDataBytes = vdb->data;
			INT64  offset = ((INT64)vdb->walkStrideBytes * 
				(sinp % (INT64)vdb->walkCount));

			inputTable[sinp] = pDataBytes + offset;
		}

		// process verts
		instance->vert(mesh->verts + i, i, inputTable);
	}
}

void CInternalDrawProjectMesh(PCMesh mesh) {
	// project each verticie
	for (int i = 0; i < mesh->vertCount; i++) {
		mesh->verts[i].x = mesh->verts[i].x / mesh->verts[i].z;
		mesh->verts[i].y = mesh->verts[i].y / mesh->verts[i].z;
	}
}

void CInternalDrawMeshVerts(PCMesh projectedMesh);
void CInternalDrawLine(CVect4F p1, CVect4F p2);
void CInternalDrawMeshLines(PCMesh projectedMesh);
