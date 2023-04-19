// <csm_fragment.c>
// Bailey Jia-Tao Brown
// 2023

#include "csm_fragment.h"
#include "csmint_pipeline.h"
#include <stdio.h>

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

CSMCALL CColor	CFragmentConvertVect3ToColor(CVect3F vect3) {
	return _convertVect3FToColor(vect3);
}

CSMCALL CVect3F CFragmentConvertColorToVect3(CColor color) {
	return _convertColorToVect3F(color);
}

CSMCALL BOOL	CFragmentGetInput(CHandle fragContext, UINT32 inputID, PFLOAT outBuffer) {
	if (outBuffer == NULL) {
		CInternalSetLastError("CFragmentGetInput failed because outBuffer was NULL");
		return FALSE;
	}
	if (fragContext == NULL) {
		CInternalSetLastError("CFragmentGetInput failed because fragContext was invalid");
		return FALSE;
	}

	PCIPFragContext context = fragContext;

	COPY_BYTES(context->fragInputs.inputs[inputID].valueBuffer, outBuffer,
		sizeof(FLOAT) * context->fragInputs.inputs[inputID].componentCount);

	return TRUE;
}
