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

		// process mesh
		CInternalPipelineProcessMesh(instanceContext);

		// loop each triangle of mesh and rasterize triangle
		// this is done by walking indexes in groups of 3
		UINT32 triangleID = 0;
		for (UINT32 meshIndex = 0; meshIndex < drawMesh->indexCount; meshIndex += 3) {
			// alloc triangle to heap
			PCIPTriData triData = CInternalAlloc(sizeof(CIPTriData));

			// get triangle from mesh
			triData->verts[0] = 
				drawMesh->vertArray[drawMesh->indexArray[meshIndex + 0]];
			triData->verts[1] =
				drawMesh->vertArray[drawMesh->indexArray[meshIndex + 1]];
			triData->verts[2] =
				drawMesh->vertArray[drawMesh->indexArray[meshIndex + 2]];

			// generate tri context for rasterization
			// note: tContext->fragContext is untouched because it is determined per-fragment
			// note: with the exception of tContext->fragContext.parent which points to tContext
			PCIPTriContext tContext = CInternalAlloc(sizeof(CIPTriContext));
			tContext->drawContext			= drawContext;
			tContext->instanceID			= instanceID;
			tContext->triangleID			= triangleID;
			tContext->rClass				= rClass;
			tContext->renderBuffer			= renderBuffer;
			tContext->fragContext.parent	= tContext;
			tContext->screenTriAndData = triData; // temporary, will be replaced when clipped

			// setup material
			if (pClass->singleMaterial == TRUE) {
				tContext->material = pClass->materials[0];
			}
			else {
				tContext->material = pClass->materials[pClass->triMaterials[triangleID]];
					
				// set to default material
				if (tContext->material == NULL) {
					tContext->material = pClass->materials[0];
				}

				// check for bad state
				if (tContext->material == NULL) {
					CInternalErrorPopup("Bad material state. No materials exist in class.");
				}
			}

			// process triangle vertex inputs/outputs
			CInternalPipelineProcessVert(tContext, triData);
			

			// clip triangle
			PCIPTriData clippedTris = CInternalAlloc(sizeof(CIPTriData) * 2);
			UINT32 triCount = CInternalPipelineClipTri(triData, clippedTris);

			// setup materials
			PCRenderClass tempRClass = rClass;
			tContext->material = tempRClass->materials[tempRClass->triMaterials[triangleID]];

			// if material is NULL, use default material (0)
			if (tContext->material == NULL)
				tContext->material = tempRClass->materials[0];

			// change based on clip output
			switch (triCount)
			{
			case -1: // CULL
				break;

			case 0: // default case. no extra tris used
				// project triangle
				CInternalPipelineProjectTri(renderBuffer, triData);

				// rasterize triangle
				CInternalPipelineRasterizeTri(tContext, triData);

				break;

			case 1: // clipped original tri into 1 tri
				// project triangle
				CInternalPipelineProjectTri(renderBuffer, clippedTris + 0);

				// rasterize clipped triangle
				CInternalPipelineRasterizeTri(tContext, clippedTris + 0);

				break;

			case 2: // clipped original tri into 2 tris
				// project triangle 1
				CInternalPipelineProjectTri(renderBuffer, clippedTris + 0);

				// rasterize clipped triangle 1
				CInternalPipelineRasterizeTri(tContext, clippedTris + 0);

				// project triangle 2
				CInternalPipelineProjectTri(renderBuffer, clippedTris + 1);

				// rasterize triangle 2
				CInternalPipelineRasterizeTri(tContext, clippedTris + 1);

				break;

			default:
				CInternalErrorPopup("Bad clipping state");
				break;
			}

			// free extra triangles
			CInternalFree(clippedTris);

			// free triangle data
			CInternalFree(triData);

			// free triangle context
			CInternalFree(tContext);

			// increment triangleID
			triangleID++;
		}
	}

	// get end tick
	LARGE_INTEGER counterEndTick;
	QueryPerformanceCounter(&counterEndTick);

	// calculate elapsed time
	LONGLONG counterTicksElapsed = counterEndTick.QuadPart - counterStartTick.QuadPart;

	// convert to MS
	LONGLONG elapsedMS = (counterTicksElapsed / _csmint.perfCounterHzMs.QuadPart);
	context->lastDrawTimeMS = (UINT64)elapsedMS;

	_CSyncLeave(TRUE);
}