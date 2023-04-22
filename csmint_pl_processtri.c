// <csmint_pl_processtri.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"

void CInternalPipelineProcessTri(PCIPTriContext triContext, PCIPTriData inTri) {
	// loop each vertex
	for (UINT32 triVertexIndex = 0; triVertexIndex < 3; triVertexIndex++) {
		// set triContext's triVertID
		triContext->triVertID = triVertexIndex;

		// calculate new vertex position and vertex outputs
		CVect3F vertOut = triContext->material->vertexShader(
			triContext,
			(triContext->triangleID * 3) + triVertexIndex,
			triContext->triangleID,
			triContext->instanceID,
			inTri->verts[triVertexIndex]
		);

		// update triangle verticies
		inTri->verts[triVertexIndex] = vertOut;
	}
}