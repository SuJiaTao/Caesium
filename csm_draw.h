// <csm_draw.h>
// Bailey Jia-Tao Brown
// 2023

#ifndef _CSM_DRAW_INCLUDE_
#define _CSM_DRAW_INCLUDE_

#include "csm_renderclass.h"

CSMCALL BOOL CDraw(CHandle renderBuffer, CHandle rClass, PCMatrix pMatrix);
CSMCALL BOOL CDrawInstanced(CHandle renderBuffer, CHandle rClass,
	UINT32 instanceCount, PCMatrix matrixArray);

#endif
