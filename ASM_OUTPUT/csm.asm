; Listing generated by Microsoft (R) Optimizing Compiler Version 19.29.30148.0 

include listing.inc

INCLUDELIB OLDNAMES

PUBLIC	??_C@_0L@LINMDAFK@CTerminate@			; `string'
PUBLIC	??_C@_0L@OFEJAFOB@CLogErrors@			; `string'
PUBLIC	??_C@_05PFCPLDJG@CLock@				; `string'
EXTRN	__imp_DeleteCriticalSection:PROC
EXTRN	__imp_HeapDestroy:PROC
EXTRN	__imp_InitializeCriticalSection:PROC
EXTRN	__imp_LeaveCriticalSection:PROC
EXTRN	__imp_EnterCriticalSection:PROC
EXTRN	__imp_HeapCreate:PROC
EXTRN	__security_check_cookie:PROC
COMM	_csmint:BYTE:04d0H
_DATA	ENDS
;	COMDAT ??_C@_05PFCPLDJG@CLock@
CONST	SEGMENT
??_C@_05PFCPLDJG@CLock@ DB 'CLock', 00H			; `string'
CONST	ENDS
;	COMDAT ??_C@_0L@OFEJAFOB@CLogErrors@
CONST	SEGMENT
??_C@_0L@OFEJAFOB@CLogErrors@ DB 'CLogErrors', 00H	; `string'
CONST	ENDS
;	COMDAT ??_C@_0L@LINMDAFK@CTerminate@
CONST	SEGMENT
??_C@_0L@LINMDAFK@CTerminate@ DB 'CTerminate', 00H	; `string'
CONST	ENDS
PUBLIC	CUnlock
PUBLIC	CLock
PUBLIC	CLogErrors
PUBLIC	CFreeError
PUBLIC	CGetLastError
PUBLIC	CMakeColor4
PUBLIC	CMakeColor3
PUBLIC	CFree
PUBLIC	CAlloc
PUBLIC	CTerminate
PUBLIC	CInitialize
PUBLIC	__real@3b808081
PUBLIC	__real@3c8efa39
PUBLIC	__real@3f000000
PUBLIC	__real@3f800000
PUBLIC	__real@437f0000
PUBLIC	__real@ba83126f
PUBLIC	__real@bf800000
PUBLIC	__xmm@80000000800000008000000080000000
EXTRN	__GSHandlerCheck:PROC
EXTRN	_fdclass:PROC
EXTRN	memset:PROC
EXTRN	__security_cookie:QWORD
EXTRN	_fltused:DWORD
;	COMDAT pdata
pdata	SEGMENT
$pdata$CUnlock DD imagerel $LN9@CUnlock
	DD	imagerel $LN9@CUnlock+87
	DD	imagerel $unwind$CUnlock
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$CLock DD	imagerel $LN9@CLock
	DD	imagerel $LN9@CLock+90
	DD	imagerel $unwind$CLock
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$CLogErrors DD imagerel $LN14@CLogErrors
	DD	imagerel $LN14@CLogErrors+189
	DD	imagerel $unwind$CLogErrors
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$CGetLastError DD imagerel $LN17@CGetLastEr
	DD	imagerel $LN17@CGetLastEr+226
	DD	imagerel $unwind$CGetLastError
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$CTerminate DD imagerel $LN15@CTerminate
	DD	imagerel $LN15@CTerminate+230
	DD	imagerel $unwind$CTerminate
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$CInitialize DD imagerel $LN5@CInitializ
	DD	imagerel $LN5@CInitializ+15
	DD	imagerel $unwind$CInitialize
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$0$CInitialize DD imagerel $LN5@CInitializ+15
	DD	imagerel $LN5@CInitializ+90
	DD	imagerel $chain$0$CInitialize
pdata	ENDS
;	COMDAT pdata
pdata	SEGMENT
$pdata$1$CInitialize DD imagerel $LN5@CInitializ+90
	DD	imagerel $LN5@CInitializ+95
	DD	imagerel $chain$1$CInitialize
;	COMDAT __xmm@80000000800000008000000080000000
CONST	SEGMENT
__xmm@80000000800000008000000080000000 DB 00H, 00H, 00H, 080H, 00H, 00H, 00H
	DB	080H, 00H, 00H, 00H, 080H, 00H, 00H, 00H, 080H
CONST	ENDS
;	COMDAT __real@bf800000
CONST	SEGMENT
__real@bf800000 DD 0bf800000r			; -1
CONST	ENDS
;	COMDAT __real@ba83126f
CONST	SEGMENT
__real@ba83126f DD 0ba83126fr			; -0.001
CONST	ENDS
;	COMDAT __real@437f0000
CONST	SEGMENT
__real@437f0000 DD 0437f0000r			; 255
CONST	ENDS
;	COMDAT __real@3f800000
CONST	SEGMENT
__real@3f800000 DD 03f800000r			; 1
CONST	ENDS
;	COMDAT __real@3f000000
CONST	SEGMENT
__real@3f000000 DD 03f000000r			; 0.5
CONST	ENDS
;	COMDAT __real@3c8efa39
CONST	SEGMENT
__real@3c8efa39 DD 03c8efa39r			; 0.0174533
CONST	ENDS
;	COMDAT __real@3b808081
CONST	SEGMENT
__real@3b808081 DD 03b808081r			; 0.00392157
CONST	ENDS
;	COMDAT xdata
xdata	SEGMENT
$chain$1$CInitialize DD 021H
	DD	imagerel $LN5@CInitializ
	DD	imagerel $LN5@CInitializ+15
	DD	imagerel $unwind$CInitialize
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$chain$0$CInitialize DQ 00004740500020521r	; 6.19294e-309
	DD	imagerel $LN5@CInitializ
	DD	imagerel $LN5@CInitializ+15
	DD	imagerel $unwind$CInitialize
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$CInitialize DQ 00000420400010401r	; 3.58617e-310
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$CTerminate DQ 03002320600020601r	; 1.96423e-77
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$CGetLastError DQ 00006340a00040a01r	; 8.62672e-309
	DD	07006320aH
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$CLogErrors DQ 00006340a00040a01r	; 8.62672e-309
	DD	07006320aH
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$CLock DQ 00000420400010401r		; 3.58617e-310
xdata	ENDS
;	COMDAT xdata
xdata	SEGMENT
$unwind$CUnlock DQ 00000420400010401r		; 3.58617e-310
xdata	ENDS
END