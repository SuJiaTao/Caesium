; Listing generated by Microsoft (R) Optimizing Compiler Version 19.29.30148.0 

include listing.inc

INCLUDELIB OLDNAMES

PUBLIC	??_C@_0BD@EAKJIILB@Bad?5clipping?5state@	; `string'
PUBLIC	??_C@_0BD@MPMALDBJ@Bad?5Clipping?5State@	; `string'
EXTRN	__imp_sqrtf:PROC
;	COMDAT ??_C@_0BD@MPMALDBJ@Bad?5Clipping?5State@
CONST	SEGMENT
??_C@_0BD@MPMALDBJ@Bad?5Clipping?5State@ DB 'Bad Clipping State', 00H ; `string'
CONST	ENDS
;	COMDAT ??_C@_0BD@EAKJIILB@Bad?5clipping?5state@
CONST	SEGMENT
??_C@_0BD@EAKJIILB@Bad?5clipping?5state@ DB 'Bad clipping state', 00H ; `string'
PUBLIC	CInternalPipelineClipTri
;	COMDAT pdata
pdata	SEGMENT
$pdata$CInternalPipelineClipTri DD imagerel $LN225@CInternalP
	DD	imagerel $LN225@CInternalP+377
	DD	imagerel $unwind$CInternalPipelineClipTri
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$14$CInternalPipelineClipTri DD imagerel $LN225@CInternalP+377
	DD	imagerel $LN225@CInternalP+4732
	DD	imagerel $chain$14$CInternalPipelineClipTri
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$15$CInternalPipelineClipTri DD imagerel $LN225@CInternalP+4732
	DD	imagerel $LN225@CInternalP+4770
	DD	imagerel $chain$15$CInternalPipelineClipTri
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$_clipTriCase2 DD imagerel _clipTriCase2
	DD	imagerel _clipTriCase2+1878
	DD	imagerel $unwind$_clipTriCase2
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$_clipTriCase1 DD imagerel _clipTriCase1
	DD	imagerel _clipTriCase1+2564
	DD	imagerel $unwind$_clipTriCase1
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$_genInterpolatedVertInputs DD imagerel _genInterpolatedVertInputs
	DD	imagerel _genInterpolatedVertInputs+468
	DD	imagerel $unwind$_genInterpolatedVertInputs
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$_vectDist DD imagerel _vectDist
	DD	imagerel _vectDist+63
	DD	imagerel $unwind$_vectDist
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$_genPlaneIntersectPoint DD imagerel _genPlaneIntersectPoint
	DD	imagerel _genPlaneIntersectPoint+117
	DD	imagerel $unwind$_genPlaneIntersectPoint
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$_genTriClipInfo DD imagerel _genTriClipInfo
	DD	imagerel _genTriClipInfo+182
	DD	imagerel $unwind$_genTriClipInfo
;	COMDAT xdata
xdata	SEGMENT
$unwind$_genTriClipInfo DQ 00000220400010401r	; 1.84783e-310
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$_genPlaneIntersectPoint DQ 00000682b00032b01r ; 5.65873e-310
	DD	02204H
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$_vectDist DQ 00000420400010401r		; 3.58617e-310
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$_genInterpolatedVertInputs DQ 00007885200115201r ; 1.04752e-308
	DQ	0000968420008784ar		; 1.30824e-308
	DQ	000140125001d3425r		; 2.78196e-308
	DQ	0c018d01ae01cf01er		; -6.20323
	DQ	00000501460157016r		; 4.35017e-310
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$_clipTriCase1 DQ 00055f864001f7519r	; 4.88855e-307
	DQ	00057d8540056e85cr		; 5.3057e-307
	DQ	00059b8440058c84cr		; 5.72284e-307
	DQ	0005b9834005aa83cr		; 6.13999e-307
	DQ	0005d782a005c882fr		; 6.55716e-307
	DQ	000c83420005e6825r		; 6.89342e-305
	DQ	0e010f01200be0120r		; -5.67746e+154
	DQ	06009700ac00cd00er		; 4.26329e+154
	DD	05008H
	DD	imagerel __GSHandlerCheck
	DD	0540H
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$_clipTriCase2 DQ 00054f864001f7519r	; 4.66604e-307
	DQ	00056d8540055e85cr		; 5.08319e-307
	DQ	00058b8440057c84cr		; 5.50034e-307
	DQ	0005a98340059a83cr		; 5.91748e-307
	DQ	0005c782a005b882fr		; 6.33465e-307
	DQ	000c63420005d6825r		; 6.3238e-305
	DQ	0e010f01200bc0120r		; -5.67746e+154
	DQ	06009700ac00cd00er		; 4.26329e+154
	DD	05008H
	DD	imagerel __GSHandlerCheck
	DD	0530H
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$chain$15$CInternalPipelineClipTri DD 021H
	DD	imagerel $LN225@CInternalP
	DD	imagerel $LN225@CInternalP+377
	DD	imagerel $unwind$CInternalPipelineClipTri
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$chain$14$CInternalPipelineClipTri DQ 00057f881001e8121r ; 5.33366e-307
	DQ	00059d86f0058e878r		; 5.7508e-307
	DQ	0005bb85d005ac866r		; 6.16794e-307
	DQ	0005e884b005d9854r		; 6.79368e-307
	DQ	000606839005f7842r		; 7.30141e-307
	DQ	000c3e42800c2f430r		; 5.66521e-305
	DQ	000c5741800c4c420r		; 6.11016e-305
	DQ	000cc340800c66410r		; 8.03255e-305
	DD	imagerel $LN225@CInternalP
	DD	imagerel $LN225@CInternalP+377
	DD	imagerel $unwind$CInternalPipelineClipTri
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$CInternalPipelineClipTri DQ 0005ca81c00062d19r ; 6.37632e-307
	DQ	05002d00400c70113r		; 2.72293e+77
	DD	imagerel __GSHandlerCheck
	DD	0560H
END
