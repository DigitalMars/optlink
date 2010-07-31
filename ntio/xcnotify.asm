		TITLE	XCNOTIFY

		INCLUDE	MACROS
		INCLUDE	WIN32DEF

		PUBLIC	XCNOTIFY

SIZE_OF_80387_REGISTERS	EQU	80

;
; The following flags control the contents of the CONTEXT structure.
;

CONTEXT_i386	EQU	10000H	;this assumes that i386 and
CONTEXT_i486	EQU	10000H	;i486 have identical context records

CONTEXT_CONTROL		EQU	CONTEXT_i386 OR 1	;SS:SP, CS:IP, FLAGS, BP
CONTEXT_INTEGER		EQU	CONTEXT_i386 OR 2	;AX, BX, CX, DX, SI, DI
CONTEXT_SEGMENTS	EQU	CONTEXT_i386 OR 4	;DS, ES, FS, GS
CONTEXT_FLOATING_POINT	EQU	CONTEXT_i386 OR 8	;387 state
CONTEXT_DEBUG_REGISTERS	EQU	CONTEXT_i386 OR 10H	;DB 0-3,6,7

CONTEXT_FULL		EQU	CONTEXT_CONTROL OR CONTEXT_INTEGER OR CONTEXT_SEGMENTS


FLOATING_SAVE_AREA	STRUC

ControlWord		DD	?
StatusWord		DD	?
TagWord			DD	?
ErrorOffset		DD	?
ErrorSelector		DD	?
DataOffset		DD	?
DataSelector		DD	?
RegisterArea		DB	SIZE_OF_80387_REGISTERS DUP(?)
Cr0NpxState		DD	?

FLOATING_SAVE_AREA	ENDS

;
; Context Frame
;
;  This frame has a several purposes: 1) it is used as an argument to
;  NtContinue, 2) is is used to constuct a call frame for APC delivery,
;  and 3) it is used in the user level thread creation routines.
;
;  The layout of the record conforms to a standard call frame.
;

CONTEXT		STRUC

;
;The flags values within this flag control the contents of
;a CONTEXT record.
;
;If the context record is used as an input parameter, then
;for each portion of the context record controlled by a flag
;whose value is set, it is assumed that that portion of the
;context record contains valid context. If the context record
;is being used to modify a threads context, then only that
;portion of the threads context will be modified.
;
;If the context record is used as an IN OUT parameter to capture
;the context of a thread, then only those portions of the thread's
;context corresponding to set flags will be returned.
;
;The context record is never used as an OUT only parameter.
;

ContextFlags	DD	?

;
;This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
;set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
;included in CONTEXT_FULL.
;

XDr0		DD	?
XDr1		DD	?
XDr2		DD	?
XDr3		DD	?
XDr6		DD	?
XDr7		DD	?

;
;This section is specified/returned if the
;ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
;

FloatSave	FLOATING_SAVE_AREA	<>

;
;This section is specified/returned if the
;ContextFlags word contians the flag CONTEXT_SEGMENTS.
;

SegGs		DD	?
SegFs		DD	?
SegEs		DD	?
SegDs		DD	?

;
;This section is specified/returned if the
;ContextFlags word contians the flag CONTEXT_INTEGER.
;

XEdi		DD	?
XEsi		DD	?
XEbx		DD	?
XEdx		DD	?
XEcx		DD	?
XEax		DD	?

;
;This section is specified/returned if the
;ContextFlags word contians the flag CONTEXT_CONTROL.
;

XEbp		DD	?
XEip		DD	?
SegCs		DD	?	;MUST BE SANITIZED
EFlags		DD	?	;MUST BE SANITIZED
XEsp		DD	?
SegSs		DD	?

CONTEXT		ENDS


		.DATA

		EXTERNDEF	_ABORTER_SEM:GLOBALSEM_STRUCT


		.CODE	ROOT_TEXT

		EXTERNDEF	_capture_eax:proc
		EXTERNDEF	HEXDWOUT:PROC,CAPTURE_EAX:PROC,FIRST_LABEL:PROC

;EXCEPTION_DISPOSITION XCNOTIFY(
;            EXCEPTION_RECORD *exception_record,
;            void *EstablisherFrame,
;            CONTEXT *context,
;            void *dispatcher_context)


XCNOTIFY	PROC
		;
		;
		;
		PUSH	EBP
		MOV	EBP,ESP

		PUSHM	EDI,ESI

		MOV	ESI,010H[EBP]		;CONTEXT RECORD
		ASSUME	ESI:PTR CONTEXT
		PUSH	EBX

		CAPTURE	_ABORTER_SEM		;ONLY ONE THREAD CAN GP-FAULT

		MOV	EBX,[ESI].ContextFlags

		TEST	EBX,CONTEXT_i386
		JZ	L9$			;I DON'T THINK THIS CAN HAPPEN

		AND	EBX,NOT CONTEXT_i386

		TEST	EBX,1			;SS:ESP, CS:EIP, EBP
		JZ	L3$

		MOV	EAX,[ESI].XEip
		MOV	EDI,OFF EIP_DHEX

		CALL	HEXDWOUT

		MOV	EAX,[ESI].XEbp
		MOV	EDI,OFF EBP_DHEX

		CALL	HEXDWOUT

		MOV	EAX,[ESI].XEsp
		MOV	EDI,OFF ESP_DHEX

		CALL	HEXDWOUT
L3$:
		TEST	EBX,2
		JZ	L5$				;AX, BX, CX, DX, SI, DI

		MOV	EAX,[ESI].XEax
		MOV	EDI,OFF EAX_DHEX

		CALL	HEXDWOUT

		MOV	EAX,[ESI].XEbx
		MOV	EDI,OFF EBX_DHEX

		CALL	HEXDWOUT

		MOV	EAX,[ESI].XEcx
		MOV	EDI,OFF ECX_DHEX

		CALL	HEXDWOUT

		MOV	EAX,[ESI].XEdx
		MOV	EDI,OFF EDX_DHEX

		CALL	HEXDWOUT

		MOV	EAX,[ESI].XEsi
		MOV	EDI,OFF ESI_DHEX

		CALL	HEXDWOUT

		MOV	EAX,[ESI].XEdi
		MOV	EDI,OFF EDI_DHEX

		CALL	HEXDWOUT
L5$:
		MOV	EAX,OFF FIRST_LABEL
		MOV	EDI,OFF BASE_HEX

		CALL	HEXDWOUT
L9$:
		PUSH	10010H		;fuStyle
		PUSH	OFF CONTEXT_MSG
		PUSH	OFF REGISTERS_MSG
		PUSH	0

		CALL	MessageBox

		PUSH	1
		CALL	ExitProcess

;		POPM	EBX,ESI

;		POP	EDI
;		MOV	ESP,EBP

;		POP	EBP

;		RET

XCNOTIFY	ENDP


		.DATA

CONTEXT_MSG	DB	'Unexpected OPTLINK Termination at EIP='
EIP_DHEX	DB	'????????',0

REGISTERS_MSG	DB	'EAX='
EAX_DHEX	DB	'????????',' EBX='
EBX_DHEX	DB	'????????',' ECX='
ECX_DHEX	DB	'????????',' EDX='
EDX_DHEX	DB	'????????',0DH,0AH,'ESI='
ESI_DHEX	DB	'????????',' EDI='
EDI_DHEX	DB	'????????',' EBP='
EBP_DHEX	DB	'????????',' ESP='
ESP_DHEX	DB	'????????',0DH,0AH,'First='
BASE_HEX	DB	'????????',0


		END

