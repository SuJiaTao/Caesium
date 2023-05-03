// <csm_shader.h>
// Bailey JT Brown
// 2023

#ifndef _CSM_SHADER_INCLUDE_
#define _CSM_SHADER_INCLUDE_ 

#define CSM_MAX_VERTEX_OUTPUTS 0x10

typedef enum CSampleType {
	CSampleType_Clamp,
	CSampleType_ClampToEdge,
	CSampleType_Repeat
} CSampleType;

typedef struct CVertexInputOutput {
	FLOAT	components[CSM_VERTEX_DATA_BUFFER_MAX_COMPONENTS];
	UINT32	componentCount;
} CVertexOutput, *PCVertexOutput, CVertexInput, *PCVertexInput;

CSMCALL CColor	CFragmentConvertFloat3ToColor(FLOAT r, FLOAT g, FLOAT b);
CSMCALL CColor	CFragmentConvertFloat4ToColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a);
CSMCALL CColor	CFragmentConvertVect3FToColor(CVect3F vect3);
CSMCALL CColor	CFragmentConvertVect4FToColor(CVect4F vect4);
CSMCALL CVect3F	CFragmentConvertColorToVect3F(CColor color);
CSMCALL CVect4F	CFragmentConvertColorToVect4F(CColor color);

CSMCALL CColor	CFragmnetBlendColor(CColor bottom, CColor top);
CSMCALL CColor	CFramgnetWeightColor(CColor c1, CColor c2, FLOAT factor);

CSMCALL BOOL	CFragmentSampleRenderBuffer(PCColor inOutColor, CHandle renderBuffer,
	CVect2F uv, CSampleType sampleType);

CSMCALL PCDrawInput	CFragmentGetDrawInput(CHandle fragContext, UINT32 drawInputID, PVOID outBuffer);
CSMCALL PCDrawInput	CVertexGetDrawInput(CHandle vertContext, UINT32 drawInputID, PVOID outBuffer);
CSMCALL	BOOL		CShaderFreeDrawInput(PCDrawInput pDrawInput);

CSMCALL	CVertexInput	CVertexGetClassVertexInput(CHandle vertContext, UINT32 bufferID);
CSMCALL BOOL			CVertexSetVertexOutput(CHandle vertContext, UINT32 outputID,
	PFLOAT inBuffer, UINT32 componentCount);

CSMCALL CVertexOutput	CFragmentGetVertexOutput(CHandle fragContext, UINT32 bufferID);

CSMCALL

#endif
