// <csmint_pl_processmesh.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint_pipeline.h"

static __forceinline FLOAT _rcpFloat(FLOAT flt) {
	FLOAT rf;
	_mm_store_ss(&rf, _mm_rcp_ss(_mm_set_ss(flt)));
	return rf;
}

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

		// cache inverse depth
		instanceContext->inverseDepthCache[vertexID] = _rcpFloat(outVert.z);
	}
}