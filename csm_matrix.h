// Bailey Jia-Tao Brown
// 2023
// <csm_matrix.h>

#ifndef _CSM_MATRIX_INCLUDE_
#define _CSM_MATRIX_INCLUDE_ 

#include "csm.h"

typedef struct CMatrix {
	FLOAT mtr[4][4];
} CMatrix, *PCMatrix;

CSMCALL CVect2F CMakeVect2F(FLOAT x, FLOAT y);
CSMCALL CVect3F CMakeVect3F(FLOAT x, FLOAT y, FLOAT z);
CSMCALL CVect4F CMakeVect4F(FLOAT x, FLOAT y, FLOAT z, FLOAT w);
CSMCALL FLOAT	CVect3FDot(CVect3F left, CVect3F right);
CSMCALL CVect3F CVect3FCross(CVect3F left, CVect3F right);

CSMCALL CMatrix CMatrixIdentity(void);
CSMCALL CMatrix CMatrixTranslate(CMatrix orig, CVect3F trl);
CSMCALL CMatrix CMatrixScale(CMatrix orig, CVect3F scl);
CSMCALL CMatrix CMatrixRotate(CMatrix orig, CVect3F rot);
CSMCALL CMatrix CMatrixTransform(CMatrix orig, CVect3F trl, CVect3F rot, CVect3F scl);
CSMCALL CMatrix CMatrixMultiply(CMatrix m1, CMatrix m2);
CSMCALL CVect3F CMatrixApply(CMatrix mat, CVect3F vect);
CSMCALL void	CMatrixSet(PCMatrix mat, INT x, INT y, FLOAT val);

#endif
