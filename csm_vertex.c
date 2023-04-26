// <csm_vertex.c>
// Bailey JT Brown
// 2023

#include "csmint.h"
#include "csm_vertex.h"

CSMCALL BOOL	CVertexGetDrawInput(CHandle vertContext, UINT32 drawInputID, PVOID outBuffer) {
	if (vertContext == NULL) {
		CInternalSetLastError("CVertexGetDrawInput failed because vertContext was invalid");
		return FALSE;
	}
	if (outBuffer == NULL) {
		CInternalSetLastError("CVertexGetDrawInput failed because outBuffer was NULL");
		return FALSE;
	}
	if (drawInputID >= CSM_MAX_DRAW_INPUTS) {
		CInternalSetLastError("CVertexGetDrawInput failed because drawInputID was invalid");
		return FALSE;
	}

	PCIPTriContext triContext = vertContext;

	PCDrawInput drawInput = triContext->drawContext->inputs + drawInputID;
	COPY_BYTES(drawInput->pData, outBuffer, drawInput->sizeBytes);

	return TRUE;
}

CSMCALL PVOID	CVertexUnsafeGetDrawInputDirect(CHandle vertContext, UINT32 drawInputID) {
	if (vertContext == NULL) {
		CInternalSetLastError("CVertexUnsafeGetDrawInputDirect failed because vertContext was invalid");
		return FALSE;
	}
	if (drawInputID >= CSM_MAX_DRAW_INPUTS) {
		CInternalSetLastError("CVertexUnsafeGetDrawInputDirect failed because drawInputID was invalid");
		return FALSE;
	}

	PCIPTriContext triContext = vertContext;
	return triContext->drawContext->inputs[drawInputID].pData;
}

CSMCALL SIZE_T	CVertexGetDrawInputSizeBytes(CHandle vertContext, UINT32 drawInputID) {
	if (vertContext == NULL) {
		CInternalSetLastError("CVertexGetDrawInputSizeBytes failed because vertContext was invalid");
		return FALSE;
	}
	if (drawInputID >= CSM_MAX_DRAW_INPUTS) {
		CInternalSetLastError("CVertexGetDrawInputSizeBytes failed because drawInputID was invalid");
		return FALSE;
	}

	PCIPTriContext triContext = vertContext;

	PCDrawInput drawInput = triContext->drawContext->inputs + drawInputID;

	return drawInput->sizeBytes;
}

CSMCALL BOOL CVertexGetClassVertexData(CHandle vertContext, UINT32 ID, PFLOAT outBuffer) {
	if (vertContext == NULL) {
		CInternalSetLastError("CVertexGetClassVertexData failed because vertContext was invalid");
		return FALSE;
	}
	if (outBuffer == NULL) {
		CInternalSetLastError("CVertexGetClassVertexData failed because outBuffer was NULL");
		return FALSE;
	}

	PCIPTriContext triContext = vertContext;

	PCVertexDataBuffer vdb = 
		CRenderClassGetVertexDataBuffer(triContext->rClass, ID);
	if (vdb == NULL) {
		CInternalSetLastError("CVertexGetClassVertexData failed because ID was invalid");
		return FALSE;
	}

	CVertexDataBufferGetElement(vdb, triContext->vertexID, outBuffer);

	return TRUE;
}

CSMCALL UINT32	CVertexGetClassVertexDataComponentCount(CHandle vertContext, UINT32 ID) {
	if (vertContext == NULL) {
		CInternalSetLastError("CVertexGetClassVertexDataComponentCount failed because vertContext was invalid");
		return FALSE;
	}

	PCIPTriContext triContext = vertContext;

	PCVertexDataBuffer vdb =
		CRenderClassGetVertexDataBuffer(triContext->rClass, ID);
	if (vdb == NULL) {
		CInternalSetLastError("CVertexGetClassVertexDataComponentCount failed because ID was invalid");
		return FALSE;
	}

	return vdb->elementComponents;
}

CSMCALL BOOL CVertexGetClassStaticData(CHandle vertContext, UINT32 ID, PFLOAT outBuffer) {
	if (vertContext == NULL) {
		CInternalSetLastError("CVertexGetClassStaticData failed because vertContext was invalid");
		return FALSE;
	}
	if (outBuffer == NULL) {
		CInternalSetLastError("CVertexGetClassStaticData failed because outBuffer was NULL");
		return FALSE;
	}

	PCIPTriContext triContext = vertContext;

	PCStaticDataBuffer sdb = CRenderClassGetStaticDataBuffer(triContext->rClass, ID);
	if (sdb == NULL) {
		CInternalSetLastError("CVertexGetClassStaticData failed because ID was invalid");
		return FALSE;
	}

	PVOID data = CStaticDataBufferMap(sdb);
	COPY_BYTES(data, outBuffer, sdb->sizeBytes);
	CStaticDataBufferUnmap(sdb);

	return TRUE;
}

CSMCALL SIZE_T	CVertexGetClassStaticDataSizeBytes(CHandle vertContext, UINT32 ID) {
	if (vertContext == NULL) {
		CInternalSetLastError("CVertexGetClassStaticDataSizeBytes failed because vertContext was invalid");
		return FALSE;
	}

	PCIPTriContext triContext = vertContext;

	PCStaticDataBuffer sdb = CRenderClassGetStaticDataBuffer(triContext->rClass, ID);
	if (sdb == NULL) {
		CInternalSetLastError("CVertexGetClassStaticDataSizeBytes failed because ID was invalid");
		return FALSE;
	}

	return sdb->sizeBytes;
}

CSMCALL BOOL CVertexSetVertexOutput(CHandle vertContext, UINT32 outputID,
	PFLOAT inBuffer, UINT32 components) {
	if (vertContext == NULL) {
		CInternalSetLastError("CVertexSetVertexOutput failed because vertContext was invalid");
		return FALSE;
	}
	if (outputID >= CSM_MAX_VERTEX_OUTPUTS) {
		CInternalSetLastError("CVertexSetVertexOutput failed because outputID was invalid");
		return FALSE;
	}
	if (inBuffer == NULL) {
		CInternalSetLastError("CVertexSetVertexOutput failed because inBuffer was NULL");
		return FALSE;
	}
	if (components >= CSM_VERTEX_DATA_BUFFER_MAX_COMPONENTS) {
		CInternalSetLastError("CVertexSetVertexOutput failed because components was too large");
		return FALSE;
	}

	PCIPTriContext context = vertContext;

	// get vertex output ptr
	PCIPVertOutput pOut = 
		context->screenTriAndData->vertOutputs[context->triVertexID].outputs + outputID;
	
	// set value
	pOut->componentCount = components;
	COPY_BYTES(inBuffer, pOut->valueBuffer, sizeof(FLOAT) * components);

	return TRUE;
}

CSMCALL BOOL CVertexSetVertexOutputFromClassVertexData(CHandle vertContext,
	UINT32 classVertID, UINT32 outputID) {
	if (vertContext == NULL) {
		CInternalSetLastError("CVertexSetVertexOutputFromClassVertexData failed because vertContext was invalid");
		return FALSE;
	}
	if (classVertID >= CSM_CLASS_MAX_VERTEX_DATA) {
		CInternalSetLastError("CVertexSetVertexOutputFromClassVertexData failed because classVertID was invalid");
		return FALSE;
	}
	if (outputID >= CSM_MAX_VERTEX_OUTPUTS) {
		CInternalSetLastError("CVertexSetVertexOutputFromClassVertexData failed because outputID was invalid");
		return FALSE;
	}

	PCIPTriContext context = vertContext;


	// get vertex data from class
	FLOAT outBuff[CSM_VERTEX_DATA_BUFFER_MAX_COMPONENTS];
	
	UINT32 components = CVertexGetClassVertexDataComponentCount(context, classVertID);

	// if DNE
	if (components == 0) {
		CInternalSetLastError("CVertexSetVertexOutputFromClassVertexData failed because vertex data could not be found");
		return FALSE;
	}

	CVertexGetClassVertexData(context, classVertID, outBuff);

	// set output value
	CVertexSetVertexOutput(context, outputID, outBuff, components);

	return TRUE;
}
