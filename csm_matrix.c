// Bailey Jia-Tao Brown
// 2023
// <csm_matrix.c>

#include "csm_matrix.h"
#include "csmint.h"
#include <math.h>

static void __forceinline _cmset(PCMatrix mat, INT r, INT c, FLOAT val) {
	mat->mtr[r][c] = val;
}

static FLOAT __forceinline _cmget(PCMatrix mat, INT r, INT c) {
	return mat->mtr[r][c];
}

static const FLOAT _cmtorad = 0.0174533f;

static FLOAT __forceinline _cmsinf(FLOAT theta) {
	return sinf(theta * _cmtorad);
}

static FLOAT __forceinline _cmcosf(FLOAT theta) {
	return cosf(theta * _cmtorad);
}

CSMCALL CVect2F CMakeVect2F(FLOAT x, FLOAT y) {
	CVect2F rVect;
	rVect.x = x;
	rVect.y = y;
	return rVect;
}

CSMCALL CVect3F CMakeVect3F(FLOAT x, FLOAT y, FLOAT z) {
	CVect3F rVect;
	rVect.x = x;
	rVect.y = y;
	rVect.z = z;
	return rVect;
}

CSMCALL CVect4F CMakeVect4F(FLOAT x, FLOAT y, FLOAT z, FLOAT w) {
	CVect4F rVect;
	rVect.x = x;
	rVect.y = y;
	rVect.z = z;
	rVect.w = w;
	return rVect;
}

CSMCALL FLOAT	CVect3FDot(CVect3F left, CVect3F right) {
	return 
		left.x * right.x +
		left.y * right.y +
		left.z * right.z;
}

CSMCALL CVect3F CVect3FCross(CVect3F left, CVect3F right) {
	return CMakeVect3F(
		right.y * left.z - left.z * right.y,
		left.z * right.x - left.x * right.z,
		left.x * right.y - left.y * right.x
	);
}

CSMCALL CMatrix CMatrixIdentity(void) {
	CMatrix mat = { 0 };
	_cmset(&mat, 0, 0, 1.0f);
	_cmset(&mat, 1, 1, 1.0f);
	_cmset(&mat, 2, 2, 1.0f);
	_cmset(&mat, 3, 3, 1.0f);
	return mat;
}

CSMCALL CMatrix CMatrixTranslate(CMatrix orig, CVect3F trl) {
	// create translation matrix
	CMatrix trlm = CMatrixIdentity();
	_cmset(&trlm, 3, 0, trl.x);
	_cmset(&trlm, 3, 1, trl.y);
	_cmset(&trlm, 3, 2, trl.z);

	// multiply
	return CMatrixMultiply(orig, trlm);
}

CSMCALL CMatrix CMatrixScale(CMatrix orig, CVect3F scl) {
	// create scale matrix
	CMatrix trlm = { 0 };
	_cmset(&trlm, 0, 0, scl.x);
	_cmset(&trlm, 1, 1, scl.y);
	_cmset(&trlm, 2, 2, scl.z);
	_cmset(&trlm, 3, 3, 1.0f);

	// multiply
	return CMatrixMultiply(orig, trlm);
}

CSMCALL CMatrix CMatrixRotate(CMatrix orig, CVect3F rot) {
	// create (horrible) rotation matrix
	CMatrix sclm = CMatrixIdentity();

	FLOAT cosX = _cmcosf(rot.x);
	FLOAT cosY = _cmcosf(rot.y);
	FLOAT cosZ = _cmcosf(rot.z);

	FLOAT sinX = _cmsinf(rot.x);
	FLOAT sinY = _cmsinf(rot.y);
	FLOAT sinZ = _cmsinf(rot.z);

	// row 1
	_cmset(&sclm, 0, 0,  cosY * cosZ);
	_cmset(&sclm, 1, 0,  cosY * sinZ);
	_cmset(&sclm, 2, 0, -sinY);

	// row 2
	_cmset(&sclm, 0, 1, sinX * sinY * cosZ -
		cosX * sinZ);
	_cmset(&sclm, 1, 1, sinX * sinY * sinZ +
		cosX * cosZ);
	_cmset(&sclm, 2, 1, sinX * cosY);

	// row 3
	_cmset(&sclm, 0, 2, cosX * sinY * cosZ +
		sinX * sinZ);
	_cmset(&sclm, 1, 2, cosX * sinY * sinZ -
		sinX * cosZ);
	_cmset(&sclm, 2, 2, cosX * cosY);

	return CMatrixMultiply(orig, sclm);
}

CSMCALL CMatrix CMatrixTransform(CMatrix orig, CVect3F trl, CVect3F rot, CVect3F scl) {
	return CMatrixTranslate(CMatrixRotate(CMatrixScale(orig, scl), rot), trl);
}

CSMCALL CMatrix CMatrixMultiply(CMatrix m1, CMatrix m2) {
	CMatrix rmat = { 0 };

	// this implementation is from scaler.com

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				rmat.mtr[i][j] += m1.mtr[i][k] * m2.mtr[k][j];
			}
		}
	}

	return rmat;
}

CSMCALL CVect3F CMatrixApply(CMatrix mat, CVect3F vect) {
	
	// make 4 component vector of input vect with 4th comp as 1
	FLOAT vect4[4] = { vect.x, vect.y, vect.z, 1.0f };

	// make accumulator vector (size 4)
	FLOAT accumVect[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// implementation partly stolen from
	// github.com/SuJiaTao/HTGL/blob/main/src/TGL/Internal/Matrix.java
	
	// for each elem in vector 
	for (int ve = 0; ve < 4; ve++) {
		// walk matrix
		for (int mi = 0; mi < 4; mi++) {
			accumVect[ve] += vect4[mi] * _cmget(&mat, mi, ve);
		}
	}

	// copy only first 3 components of accumulator vector
	CVect3F rv3 = { accumVect[0], accumVect[1], accumVect[2] };

	return rv3;
}

CSMCALL void CMatrixSet(PCMatrix mat, INT x, INT y, FLOAT val) {
	_cmset(mat, x, y, val);
}
