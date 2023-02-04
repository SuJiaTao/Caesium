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

static CMatrix __forceinline _cmzero(void) {
	CMatrix mat;
	ZERO_BYTES(&mat, sizeof(CMatrix));
	return mat;
}

static const FLOAT _cmtorad = 0.0174533f;

static FLOAT __forceinline _cmsinf(FLOAT theta) {
	return sinf(theta * _cmtorad);
}

static FLOAT __forceinline _cmcosf(FLOAT theta) {
	return cosf(theta * _cmtorad);
}

CSMCALL CMatrix CMatrixIdentity(void) {
	CMatrix mat = _cmzero();
	_cmset(&mat, 0, 0, 1.0f);
	_cmset(&mat, 1, 1, 1.0f);
	_cmset(&mat, 2, 2, 1.0f);
	_cmset(&mat, 3, 3, 1.0f);
	return mat;
}

CSMCALL CMatrix CMatrixTranslate(CMatrix orig, CVect3F trl) {
	// create translation matrix
	CMatrix trlm = CMatrixIdentity();
	_cmset(&trlm, 0, 3, trl.x);
	_cmset(&trlm, 1, 3, trl.y);
	_cmset(&trlm, 2, 3, trl.z);

	// multiply
	return CMatrixMultiply(orig, trlm);
}

CSMCALL CMatrix CMatrixScale(CMatrix orig, CVect3F scl) {
	// create scale matrix
	CMatrix trlm = _cmzero();
	_cmset(&trlm, 0, 0, scl.x);
	_cmset(&trlm, 1, 1, scl.y);
	_cmset(&trlm, 2, 2, scl.z);

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
	_cmset(&sclm, 0, 0,  cosY * cosY);
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
	return CMatrixTranslate(CMatrixScale(CMatrixRotate(orig, rot), scl), trl);
}

CSMCALL CMatrix CMatrixMultiply(CMatrix m1, CMatrix m2) {
	CMatrix rmat;

	// this implementation is from wikipedia

	// for every row of m1
	for (int m1x = 0; m1x < 4; m1x++) {

		// for every column of m1
		for (int m1y = 0; m1y < 4; m1y++) {
			FLOAT accum = 0.0f;

			for (int k = 0; k < 4; k++) {
				accum += _cmget(&m1, m1x, k) * _cmget(&m2, k, m1x);
			}

			_cmset(&rmat, m1x, m1y, accum);
		}
	}

	return rmat;
}

CSMCALL CVect3F CMatrixApply(CMatrix mat, CVect3F vect) {
	CVect4F rv;
	ZERO_BYTES(&rv, sizeof(CVect3F));

	PFLOAT rva = &rv; // array hack

	// implementation partly stolen from
	// github.com/SuJiaTao/HTGL/blob/main/src/TGL/Internal/Matrix.java
	
	// for each elem in vector 
	for (int ve = 0; ve < 4; ve++) {
		// walk matrix
		for (int mi = 0; mi < 4; mi++) {
			rva[ve] += rva[mi] * _cmget(&mat, mi, ve);
		}
	}

	// copy only first 3 components
	CVect3F rv3;
	COPY_BYTES(&rva, &rv3, sizeof(FLOAT) * 3);

	return rv3;
}
