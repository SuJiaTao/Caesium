// Bailey Jia-Tao Brown
// 2023
// <csmint_draw.h>

#ifndef _CSMINT_DRAW_INCLUDE_
#define _CSMINT_DRAW_INCLUDE_ 

#include "csmint.h"

CMesh CInternalDrawProcessMesh(PCRenderInstance instance, PCMesh mesh);
CMesh CInternalDrawProjectMesh(PCRenderInstance instance, PCMesh mesh);
void  CInternalDrawMeshVerts(PCMesh projectedMesh);
void  CInternalDrawLine(CVect4F p1, CVect4F p2);
void  CInternalDrawMeshLines(PCMesh projectedMesh);

#endif
