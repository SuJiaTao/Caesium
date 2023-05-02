// <csmint_pl_processmesh.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"

void	CInternalPipelineProcessMesh(PCIPInstanceContext instanceContext) {

	// loop each vertex
	for (UINT32 vertexID = 0; vertexID < instanceContext->processedMesh->vertCount; vertexID++) {
		// prepare vertex context
		PCIPVertContext vertContext		= CInternalAlloc(sizeof(CIPVertContext));
		vertContext->instanceContext	= instanceContext;
		vertContext->vertexID			= vertexID;
		vertContext->pOutputList		= instanceContext->outputListArray + vertexID;

		// call vertex shader
		CVect3F outVert = ((PCMaterial)instanceContext->renderClass->material)->vertexShader(
			vertContext,
			instanceContext->instanceID,
			vertContext->vertexID,
			instanceContext->originalMesh->vertArray[vertexID]
		);

		// apply processed vertex to processed mesh
		instanceContext->processedMesh->vertArray[vertexID] = outVert;
	}
}