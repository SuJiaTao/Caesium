// <csm_fragment.c>
// Bailey Jia-Tao Brown
// 2023

#include "csm_fragment.h"
#include "csmint_pipeline.h"
#include <stdio.h>

static __forceinline CVect3F _convertColorToVect3F(CRgb col) {
	CVect3F ret;
	ret.x = col.r;
	ret.y = col.g;
	ret.z = col.b;
	return ret;
}

static __forceinline CRgb _convertVect3FToColor(CVect3F vect) {
	CRgb ret;
	ret.r = vect.x;
	ret.g = vect.y;
	ret.b = vect.z;
	return ret;
}

CSMCALL FLOAT   CFragmentInterpolateFloat(CHandle fragContext,
	FLOAT p1f, FLOAT p2f, FLOAT p3f) {
	PCIPFragContext fCtx = fragContext; // get context
	CVect3F weights = fCtx->barycentricWeightings; // grab weights

	// multiply each by weighting and assign to output
	FLOAT flt1 = weights.x * (p1f);
	FLOAT flt2 = weights.y * (p2f);
	FLOAT flt3 = weights.z * (p3f);

	return (flt1 + flt2 + flt3);
}

CSMCALL void	CFragmentInterpolateFloats(CHandle fragContext,
	UINT32 count, PFLOAT pa1f, PFLOAT pa2f, PFLOAT pa3f, PFLOAT result)
{
	for (int i = 0; i < count; i++) {
		result[i] = CFragmentInterpolateFloat(fragContext, pa1f[i], pa2f[i], pa3f[i]);
	}
}

CSMCALL CVect2F CFragmentInterpolateVect2F(CHandle fragContext,
	CVect2F p1v2, CVect2F p2v2, CVect2F p3v2) {
	volatile CVect2F result = { 0 };
	CFragmentInterpolateFloats(fragContext, 2, &p1v2, &p2v2, &p3v2, &result);
	return result;
}

CSMCALL CVect3F CFragmentInterpolateVect3F(CHandle fragContext,
	CVect3F p1v3, CVect3F p2v3, CVect3F p3v3) {
	volatile CVect3F result = { 0 };
	CFragmentInterpolateFloats(fragContext, 3, &p1v3, &p2v3, &p3v3, &result);
	return result;
}

CSMCALL CRgb	CFragmentInterpolateColor(CHandle fragContext,
	CRgb p1c, CRgb p2c, CRgb p3c) {
	// convert to floats for manipulation
	CVect3F fCol1 = _convertColorToVect3F(p1c);
	CVect3F fCol2 = _convertColorToVect3F(p2c);
	CVect3F fCol3 = _convertColorToVect3F(p3c);

	CVect3F result = CFragmentInterpolateVect3F(fragContext, fCol1, fCol2, fCol3);
	return _convertVect3FToColor(result);
}

CSMCALL CRgb	CFragmentConvertVect3ToColor(CVect3F vect3) {
	return _convertVect3FToColor(vect3);
}

CSMCALL CVect3F CFragmentConvertColorToVect3(CRgb color) {
	return _convertColorToVect3F(color);
}
