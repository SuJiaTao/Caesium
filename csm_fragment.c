// <csm_fragment.c>
// Bailey Jia-Tao Brown
// 2023

#include "csm_fragment.h"
#include "csm_vertex.h"
#include "csmint_pipeline.h"
#include <stdio.h>
#include <math.h>
#include <immintrin.h>

static __forceinline void _clampFloatToColorRange(PFLOAT f) {
	_mm_store_ss(
		f, 
		_mm_max_ss(
			_mm_set_ss(0.0f), 
			_mm_min_ss(
				_mm_set_ss(255.0f), 
				_mm_load_ss(f)
			)
		)
	);
}

CSMCALL CColor	CFragmentConvertFloat3ToColor(FLOAT r, FLOAT g, FLOAT b) {
	_clampFloatToColorRange(&r);
	_clampFloatToColorRange(&g);
	_clampFloatToColorRange(&b);
	CColor rc;
	rc.r = r;
	rc.g = g;
	rc.b = b;
	rc.a = 255;
	return rc;
}

CSMCALL CColor	CFragmentConvertFloat4ToColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) {
	_clampFloatToColorRange(&r);
	_clampFloatToColorRange(&g);
	_clampFloatToColorRange(&b);
	_clampFloatToColorRange(&a);
	CColor rc;
	rc.r = r;
	rc.g = g;
	rc.b = b;
	rc.a = a;
	return rc;
}

CSMCALL CColor	CFragmentConvertVect3ToColor(CVect3F vect3) {
	_clampFloatToColorRange(&vect3.x);
	_clampFloatToColorRange(&vect3.y);
	_clampFloatToColorRange(&vect3.z);
	CColor rc;
	rc.r = vect3.x;
	rc.g = vect3.y;
	rc.b = vect3.z;
	rc.a = 255;
	return rc;
}

CSMCALL CColor	CFragmentConvertVect4ToColor(CVect4F vect4) {
	_clampFloatToColorRange(&vect4.x);
	_clampFloatToColorRange(&vect4.y);
	_clampFloatToColorRange(&vect4.z);
	_clampFloatToColorRange(&vect4.w);
	CColor rc;
	rc.r = vect4.x;
	rc.g = vect4.y;
	rc.b = vect4.z;
	rc.a = vect4.w;
	return rc;
}

CSMCALL CVect3F CFragmentConvertColorToVect3(CColor color) {
	CVect3F ret;
	ret.x = color.r;
	ret.y = color.g;
	ret.z = color.b;
	return ret;
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

	PCDrawInput drawInput = context->triContext->drawContext->inputs + drawInputID;
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
	return context->triContext->drawContext->inputs[drawInputID].pData;
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

	PCDrawInput drawInput = context->triContext->drawContext->inputs + drawInputID;

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

	PCStaticDataBuffer sdb = CRenderClassGetStaticDataBuffer(context->triContext->rClass, ID);
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

	PCStaticDataBuffer sdb = CRenderClassGetStaticDataBuffer(context->triContext->rClass, ID);
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
	UINT32 fb_x = (INT)(index_x);
	UINT32 fb_y = (INT)(index_y);

	// ensure valid
	fb_x = max(0, min(fb_x, texWidth  - 1));
	fb_y = max(0, min(fb_y, texHeight - 1));

	FLOAT unusedDepth;
	CRenderBufferUnsafeGetFragment(renderBuffer, fb_x, fb_y, inOutColor, &unusedDepth);

	return TRUE;
}
