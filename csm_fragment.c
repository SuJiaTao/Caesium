// <csm_fragment.c>
// Bailey Jia-Tao Brown
// 2023

#include "csm_fragment.h"
#include "csm_vertex.h"
#include "csmint_pipeline.h"
#include <stdio.h>
#include <math.h>

static __forceinline CVect3F _convertColorToVect3F(CColor col) {
	CVect3F ret;
	ret.x = col.r;
	ret.y = col.g;
	ret.z = col.b;
	return ret;
}

static __forceinline FLOAT _clampToColorRange(FLOAT val) {
	return min(255, max(0, val));
}

static __forceinline CColor _convertVect3FToColor(CVect3F vect) {
	CColor ret;
	ret.r = _clampToColorRange(vect.x);
	ret.g = _clampToColorRange(vect.y);
	ret.b = _clampToColorRange(vect.z);
	ret.a = 255;
	return ret;
}

CSMCALL CColor	CFragmentConvertFloat3ToColor(FLOAT r, FLOAT g, FLOAT b) {
	return CFragmentConvertVect3ToColor(CMakeVect3F(r, g, b));
}

CSMCALL CColor	CFragmentConvertFloat4ToColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) {
	return CFragmentConvertVect4ToColor(CMakeVect4F(r, g, b, a));
}

CSMCALL CColor	CFragmentConvertVect3ToColor(CVect3F vect3) {
	return _convertVect3FToColor(vect3);
}

CSMCALL CColor	CFragmentConvertVect4ToColor(CVect4F vect4) {
	CColor rColor;
	rColor.r = _clampToColorRange(vect4.x);
	rColor.g = _clampToColorRange(vect4.y);
	rColor.b = _clampToColorRange(vect4.z);
	rColor.a = _clampToColorRange(vect4.w);
	return rColor;
}

CSMCALL CVect3F CFragmentConvertColorToVect3(CColor color) {
	return _convertColorToVect3F(color);
}

CSMCALL CVect4F CFragmentConvertColorToVect4(CColor color) {
	CVect4F ret;
	ret.x = color.r;
	ret.y = color.g;
	ret.z = color.b;
	ret.w = color.a;
	return ret;
}

CSMCALL CColor	CFragmentBlendColor(CColor bottom, CColor top) {
	CColor ret;
	CVect4F old_c = CFragmentConvertColorToVect4(bottom);
	CVect4F new_c = CFragmentConvertColorToVect4(top);

	CVect3F blend_c;
	FLOAT  alpha = new_c.w * 0.003921568627f; // div by 255
	blend_c.x = new_c.x * alpha + (1.0f - alpha) * old_c.x;
	blend_c.y = new_c.y * alpha + (1.0f - alpha) * old_c.y;
	blend_c.z = new_c.z * alpha + (1.0f - alpha) * old_c.z;
	
	return CFragmentConvertVect3ToColor(blend_c);
}

CSMCALL CColor	CFragmentBlendColorWeighted(CColor c1, CColor c2, FLOAT factor) {
	// clamp factor to [0, 1]
	factor = min(1.0f, max(0.0f, factor));

	c2.a = factor * 255.0f;
	return CFragmentBlendColor(c1, c2);
}

CSMCALL BOOL	CFragmentGetDrawInput(CHandle fragContext, UINT32 drawInputID, PVOID outBuffer) {
	if (fragContext == NULL) {
		CInternalSetLastError("CFragmentGetDrawInput failed because vertContext was invalid");
		return FALSE;
	}
	if (outBuffer == NULL) {
		CInternalSetLastError("CFragmentGetDrawInput failed because outBuffer was NULL");
		return FALSE;
	}
	if (drawInputID >= CSM_MAX_DRAW_INPUTS) {
		CInternalSetLastError("CFragmentGetDrawInput failed because drawInputID was invalid");
		return FALSE;
	}

	PCIPFragContext context = fragContext;

	PCDrawInput drawInput = context->parent->drawContext->inputs + drawInputID;
	COPY_BYTES(drawInput->pData, outBuffer, drawInput->sizeBytes);

	return TRUE;
}

CSMCALL PVOID	CFragmentUnsafeGetDrawInputDirect(CHandle fragContext, UINT32 drawInputID) {
	if (fragContext == NULL) {
		CInternalSetLastError("CFragmentUnsafeGetDrawInputDirect failed because vertContext was invalid");
		return FALSE;
	}
	if (drawInputID >= CSM_MAX_DRAW_INPUTS) {
		CInternalSetLastError("CFragmentUnsafeGetDrawInputDirect failed because drawInputID was invalid");
		return FALSE;
	}

	PCIPFragContext context = fragContext;
	return context->parent->drawContext->inputs[drawInputID].pData;
}

CSMCALL SIZE_T	CFragmentGetDrawInputSizeBytes(CHandle fragContext, UINT32 drawInputID) {
	if (fragContext == NULL) {
		CInternalSetLastError("CFragmentGetDrawInputSizeBytes failed because vertContext was invalid");
		return FALSE;
	}
	if (drawInputID >= CSM_MAX_DRAW_INPUTS) {
		CInternalSetLastError("CFragmentGetDrawInputSizeBytes failed because drawInputID was invalid");
		return FALSE;
	}

	PCIPFragContext context = fragContext;

	PCDrawInput drawInput = context->parent->drawContext->inputs + drawInputID;

	return drawInput->sizeBytes;
}

CSMCALL BOOL	CFragmentGetVertexOutput(CHandle fragContext, UINT32 outputID, PFLOAT outBuffer) {
	if (outBuffer == NULL) {
		CInternalSetLastError("CFragmentGetVertexOutput failed because outBuffer was NULL");
		return FALSE;
	}
	if (fragContext == NULL) {
		CInternalSetLastError("CFragmentGetVertexOutput failed because fragContext was invalid");
		return FALSE;
	}
	if (outputID >= CSM_MAX_VERTEX_OUTPUTS) {
		CInternalSetLastError("CFragmentGetVertexOutput failed because outputID was invalid");
		return FALSE;
	}

	PCIPFragContext context = fragContext;

	COPY_BYTES(context->fragInputs.outputs[outputID].valueBuffer, outBuffer,
		sizeof(FLOAT) * context->fragInputs.outputs[outputID].componentCount);

	return TRUE;
}

CSMCALL PFLOAT	CFragmentUnsafeGetVertexOutputDirect(CHandle fragContext, UINT32 outputID) {
	if (fragContext == NULL) {
		CInternalSetLastError("CFragmentUnsafeGetVertexOutputDirect failed because fragContext was invalid");
		return FALSE;
	}
	if (outputID >= CSM_MAX_VERTEX_OUTPUTS) {
		CInternalSetLastError("CFragmentUnsafeGetVertexOutputDirect failed because outputID was invalid");
		return FALSE;
	}

	PCIPFragContext context = fragContext;
	return context->fragInputs.outputs[outputID].valueBuffer;
}

CSMCALL UINT32	CFragmentGetVertexOutputComponentCount(CHandle fragContext, UINT32 outputID) {
	if (fragContext == NULL) {
		CInternalSetLastError("CFragmentGetVertexOutputComponentCount failed because fragContext was invalid");
		return FALSE;
	}
	if (outputID >= CSM_MAX_VERTEX_OUTPUTS) {
		CInternalSetLastError("CFragmentGetVertexOutputComponentCount failed because outputID was invalid");
		return FALSE;
	}

	PCIPFragContext context = fragContext;

	return context->fragInputs.outputs[outputID].componentCount;
}

CSMCALL BOOL	CFragmentGetClassStaticData(CHandle fragContext, UINT32 ID, PVOID outBuffer) {
	if (outBuffer == NULL) {
		CInternalSetLastError("CFragmentGetClassStaticData failed because outBuffer was NULL");
		return FALSE;
	}
	if (fragContext == NULL) {
		CInternalSetLastError("CFragmentGetClassStaticData failed because fragContext was invalid");
		return FALSE;
	}

	PCIPFragContext context = fragContext;

	PCStaticDataBuffer sdb = CRenderClassGetStaticDataBuffer(context->parent->rClass, ID);
	if (sdb == NULL) {
		CInternalSetLastError("CFragmentGetClassStaticData failed because ID was invalid");
		return FALSE;
	}

	// copy to outbuffer
	EnterCriticalSection(&sdb->mapLock);
	COPY_BYTES(sdb->data, outBuffer, sdb->sizeBytes);
	LeaveCriticalSection(&sdb->mapLock);

	return TRUE;
}

CSMCALL SIZE_T	CFragmentGetClassStaticDataSizeBytes(CHandle fragContext, UINT32 ID) {
	if (fragContext == NULL) {
		CInternalSetLastError("CFragmentGetClassStaticDataSizeBytes failed because fragContext was invalid");
		return FALSE;
	}

	PCIPFragContext context = fragContext;

	PCStaticDataBuffer sdb = CRenderClassGetStaticDataBuffer(context->parent->rClass, ID);
	if (sdb == NULL) {
		CInternalSetLastError("CFragmentGetClassStaticDataSizeBytes failed because ID was invalid");
		return FALSE;
	}

	return sdb->sizeBytes;
}

CSMCALL BOOL	CFragmentSampleRenderBuffer(PCColor inOutColor, CHandle renderBuffer, 
	CVect2F uv, CSampleType sampleType) {
	if (inOutColor == NULL) {
		CInternalSetLastError("CFragmentSampleRenderBuffer failed because inOutColor was NULL");
		return FALSE;
	}
	if (renderBuffer == NULL) {
		CInternalSetLastError("CFragmentSampleRenderBuffer failed because renderBuffer was invalid");
		return FALSE;
	}

	PCRenderBuffer rb = renderBuffer;

	*inOutColor = CMakeColor4(0, 0, 0, 0); // set default to fully transparent

	const INT texWidth  = rb->width;
	const INT texHeight = rb->height;

	// change UV based on sample type
	switch (sampleType)
	{
	case CSampleType_Clamp:
		if (uv.x < 0.0f || uv.x >= 1.0f) return TRUE;
		if (uv.y < 0.0f || uv.y >= 1.0f) return TRUE;
		break;

	case CSampleType_ClampToEdge:
		uv.x = max(0.0f, min(uv.x, 1.0f));
		uv.y = max(0.0f, min(uv.y, 1.0f));
		break;

	case CSampleType_Repeat:
		uv.x = fmodf(uv.x, 1.0f);
		uv.y = fmodf(uv.y, 1.0f);

		// account for negative
		if (uv.x < 0.0f) {
			uv.x = 1.0f + uv.x;
		}
		if (uv.y < 0.0f) {
			uv.y = 1.0f + uv.y;
		}

		break;

	default:
		CInternalSetLastError("CFragmentSampleRenderBuffer failed because sampleType was invalid");
		return FALSE;
	}

	// generate framebuffer float index
	FLOAT index_x = (uv.x * (FLOAT)texWidth);
	FLOAT index_y = (uv.y * (FLOAT)texHeight);

	// convert to integer
	UINT32 fb_x = lroundf(index_x - 0.5f);
	UINT32 fb_y = lroundf(index_y - 0.5f);

	// ensure valid
	fb_x = max(0, min(fb_x, texWidth  - 1));
	fb_y = max(0, min(fb_y, texHeight - 1));

	CRenderBufferGetFragment(renderBuffer, fb_x, fb_y, inOutColor, NULL);

	return TRUE;
}
