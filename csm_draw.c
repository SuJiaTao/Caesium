// <csm_draw.c>
// Bailey Jia-Tao Brown
// 2023

#include "csmint.h"
#include "csmint_pipeline.h"
#include "csm_draw.h"
#include "csm_mesh.h"
#include <stdio.h>

CSMCALL CHandle CMakeDrawContext(CHandle renderBuffer) {
	_CSyncEnter();
	if (renderBuffer == NULL) {
		_CSyncLeaveErr(NULL, "CMakeDrawContext failed because renderBuffer was invalid");
	}

	PCDrawContext dc = CInternalAlloc(sizeof(CDrawContext));
	dc->renderBuffer = renderBuffer;

	_CSyncLeave(dc);
}

CSMCALL BOOL	CDestroyDrawContext(CHandle drawContext) {
	_CSyncEnter();
	if (drawContext == NULL) {
		_CSyncLeaveErr(FALSE, "CDestroyDrawContext failed because drawContext was invalid");
	}

	PCDrawContext context = drawContext;

	// free all input data
	for (UINT32 inputID = 0; inputID < CSM_MAX_DRAW_INPUTS; inputID++) {
		PCDrawInput input = context->inputs + inputID;
		if (input->pData != NULL)
			CInternalFree(input->pData);
	}

	// free dc
	CInternalFree(context);

	_CSyncLeave(TRUE);
}

CSMCALL	CHandle	CDrawContextSetDrawInput(CHandle drawContext, UINT32 inputID, PVOID inBytes, SIZE_T size) {
	_CSyncEnter();
	if (drawContext == NULL) {
		_CSyncLeaveErr(FALSE, "CDrawContextSetDrawInput failed because drawContext was invalid");
	}
	if (inputID >= CSM_MAX_DRAW_INPUTS) {
		_CSyncLeaveErr(FALSE, "CDrawContextSetDrawInput failed because inputID was invalid");
	}

	PCDrawContext context = drawContext;
	PCDrawInput	  input = context->inputs + inputID;
	
	// free existing data if needed
	if (input->pData != NULL)
		CInternalFree(input->pData);

	// if size is 0, then skip
	if (size == 0) {
		_CSyncLeave(TRUE);
	}

	// alloc new and copy
	input->pData = CInternalAlloc(size);
	input->sizeBytes = size;
	COPY_BYTES(inBytes, input->pData, size);

	_CSyncLeave(TRUE);
}

CSMCALL BOOL	CDrawContextGetDrawInput(CHandle drawContext, UINT32 inputID, PVOID outBytes) {
	_CSyncEnter();
	if (drawContext == NULL) {
		_CSyncLeaveErr(FALSE, "CDrawContextGetDrawInput failed because drawContext was invalid");
	}
	if (inputID >= CSM_MAX_DRAW_INPUTS) {
		_CSyncLeaveErr(FALSE, "CDrawContextGetDrawInput failed because inputID was invalid");
	}
	if (outBytes == NULL) {
		_CSyncLeaveErr(FALSE, "CDrawContextGetDrawInput failed because outBytes was NULL");
	}

	PCDrawContext context = drawContext;
	PCDrawInput input = context->inputs + inputID;

	// copy bytes to outbuffer
	COPY_BYTES(input->pData, outBytes, input->sizeBytes);

	_CSyncLeave(TRUE);
}

CSMCALL SIZE_T	CDrawContextGetDrawInputSizeBytes(CHandle drawContext, UINT32 inputID) {
	_CSyncEnter();
	if (drawContext == NULL) {
		_CSyncLeaveErr(FALSE, "CDrawContextGetDrawInputSizeBytes failed because drawContext was invalid");
	}
	if (inputID >= CSM_MAX_DRAW_INPUTS) {
		_CSyncLeaveErr(FALSE, "CDrawContextGetDrawInputSizeBytes failed because inputID was invalid");
	}

	PCDrawContext context = drawContext;
	PCDrawInput input = context->inputs + inputID;

	_CSyncLeave(input->sizeBytes);
}

CSMCALL UINT64	CDrawContextGetLastDrawTimeMS(CHandle drawContext) {
	_CSyncEnter();
	if (drawContext == NULL) {
		_CSyncLeaveErr(FALSE, "CDrawContextGetLastDrawTimeMS failed because drawContext was invalid");
	}

	PCDrawContext context = drawContext;
	_CSyncLeave(context->lastDrawTimeMS);
}

CSMCALL BOOL CDraw(CHandle drawContext, CHandle rClass) {
	return CDrawInstanced(drawContext, rClass, 1);
}

CSMCALL BOOL CDrawInstanced(CHandle drawContext, CHandle rClass, UINT32 instanceCount) {
	_CSyncEnter();

	// get start tick
	LARGE_INTEGER counterStartTick;
	QueryPerformanceCounter(&counterStartTick);

	// check for bad params
	if (drawContext == NULL) {
		_CSyncLeaveErr(FALSE, "CDrawInstanced failed because drawContext was invalid");
	}
	if (rClass == NULL) {
		_CSyncLeaveErr(FALSE, "CDrawInstanced failed because rClass was invalid");
	}
	if (instanceCount == 0) {
		_CSyncLeaveErr(FALSE, "CDrawInstanced failed because instanceCount was 0");
	}

	// loop all instances
	for (UINT32 instanceID = 0; instanceID < instanceCount; instanceID++) {
		// prepare instance context
		PCIPInstanceContext instanceContext = CInternalAlloc(sizeof(CIPInstanceContext));
		instanceContext->drawContext = drawContext;
		instanceContext->renderClass = rClass;
		instanceContext->instanceID = instanceID;
		instanceContext->originalMesh = ((PCRenderClass)rClass)->mesh;
		instanceContext->processedMesh = CInternalAlloc(sizeof(CMesh));

		// processed mesh should be copy of original mesh
		COPY_BYTES(instanceContext->originalMesh, instanceContext->processedMesh, sizeof(CMesh));

		// make vertex output list
		instanceContext->outputListArray =
			CInternalAlloc(sizeof(CIPVertOutputList) * instanceContext->processedMesh->vertCount);

		// make inverse depth cache
		instanceContext->inverseDepthCache =
			CInternalAlloc(sizeof(FLOAT) * instanceContext->processedMesh->vertCount);

		// process mesh
		CInternalPipelineProcessMesh(instanceContext);

		// loop each triangle by walking index array in threes
		UINT32 triangleID					= 0;
		PCMesh drawMesh						= instanceContext->processedMesh;
		PCIPVertOutputList vertOutputLists	= instanceContext->outputListArray;
		for (UINT32 indexID = 0; indexID < drawMesh->indexCount; indexID += 3) {

			// prepare triangle context
			PCIPTriContext triContext	= CInternalAlloc(sizeof(CIPTriContext));
			triContext->instanceContext = instanceContext;
			triContext->triangleID		= triangleID;
			triContext->renderBuffer	= instanceContext->drawContext->renderBuffer;

			// get vertex indicies
			const UINT32 VERTEX_ID_0 = drawMesh->indexArray[indexID + 0];
			const UINT32 VERTEX_ID_1 = drawMesh->indexArray[indexID + 1];
			const UINT32 VERTEX_ID_2 = drawMesh->indexArray[indexID + 2];

			// construct triangle verticies from indicies
			triContext->verts[0] = 
				drawMesh->vertArray[VERTEX_ID_0];
			triContext->verts[1] =
				drawMesh->vertArray[VERTEX_ID_1];
			triContext->verts[2] =
				drawMesh->vertArray[VERTEX_ID_2];

			// construct triangle vertex outputs from indicies
			triContext->vertOutputs[0] =
				vertOutputLists[VERTEX_ID_0];
			triContext->vertOutputs[1] =
				vertOutputLists[VERTEX_ID_1];
			triContext->vertOutputs[2] =
				vertOutputLists[VERTEX_ID_2];

			// construct depths from indicies
			triContext->invDepths[0] =
				instanceContext->inverseDepthCache[VERTEX_ID_0];
			triContext->invDepths[1] =
				instanceContext->inverseDepthCache[VERTEX_ID_1];
			triContext->invDepths[2] =
				instanceContext->inverseDepthCache[VERTEX_ID_2];

			// prepare triangle clipping output
			PCIPTriContext clippedTris = CInternalAlloc(sizeof(CIPTriContext) * 2);
			UINT32 triCount = CInternalPipelineClipTri(triContext, clippedTris);

			// change rasterization behavior based on clip output
			switch (triCount)
			{
			case -1: // CULL
				break;

			case 0: // default case. no extra tris used
				// project triangle
				CInternalPipelineProjectTri(triContext);

				// rasterize triangle
				CInternalPipelineRasterizeTri(triContext);

				break;

			case 1: // clipped original tri into 1 tri
				// project triangle
				CInternalPipelineProjectTri(clippedTris + 0);

				// rasterize clipped triangle
				CInternalPipelineRasterizeTri(clippedTris + 0);

				break;

			case 2: // clipped original tri into 2 tris
				// project triangle 1
				CInternalPipelineProjectTri(clippedTris + 0);

				// rasterize clipped triangle 1
				CInternalPipelineRasterizeTri(clippedTris + 0);

				// project triangle 2
				CInternalPipelineProjectTri(clippedTris + 1);

				// rasterize triangle 2
				CInternalPipelineRasterizeTri(clippedTris + 1);

				break;

			default:
				CInternalErrorPopup("Bad clipping state");
				break;
			}

			// free extra clipped triangles
			CInternalFree(clippedTris);

			// free triangle context
			CInternalFree(triContext);

			// increment triangleID
			triangleID++;
		}

		// free inverse depth cahce
		CInternalFree(instanceContext->inverseDepthCache);

		// free vertex output values
		CInternalFree(instanceContext->outputListArray);

		// free processed mesh
		CInternalFree(instanceContext->processedMesh);

		// free instance context
		CInternalFree(instanceContext);
	}

	// get end tick
	LARGE_INTEGER counterEndTick;
	QueryPerformanceCounter(&counterEndTick);

	// calculate elapsed time
	LONGLONG counterTicksElapsed = counterEndTick.QuadPart - counterStartTick.QuadPart;

	// convert to MS
	LONGLONG elapsedMS = (counterTicksElapsed / _csmint.perfCounterHzMs.QuadPart);
	((PCDrawContext)drawContext)->lastDrawTimeMS = (UINT64)elapsedMS;

	_CSyncLeave(TRUE);
}