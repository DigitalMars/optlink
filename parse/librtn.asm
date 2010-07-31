		TITLE	LIBRTN - Copyright (c) SLR Systems 1994

		INCLUDE MACROS
		INCLUDE	IO_STRUC
		INCLUDE	EXES

		PUBLIC	LIBSTUFF,RET_TRUE,RET_FALSE,LIB_ROUTINE_FINAL,LIB_ROUTINE_FINAL_COMENT

		.DATA

		EXTERNDEF	EXETYPE_FLAG:BYTE


		.CODE	FILEPARSE_TEXT

		EXTERNDEF	_get_filename:proc,HANDLE_LIBS:PROC,HANDLE_LOD:PROC,GTNXTU:PROC,ERR_RET:PROC,WARN_RET:PROC
		EXTERNDEF	_err_abort:proc
		externdef	_ret_false:proc

		EXTERNDEF	DUP_LOD_ERR:ABS,DOSX_NONDOSX_ERR:ABS,DOSX_EXEPACK_ERR:ABS


LRF_VARS	STRUCT

LRF_NFN_STRUCT_EBP	DB	SIZEOF NFN_STRUCT DUP(?)

LRF_VARS	ENDS


FIX	MACRO	XX

XX	EQU	([EBP-SIZEOF LRF_VARS].(XX&_EBP))

ENDM

FIX	LRF_NFN_STRUCT


LIB_ROUTINE_FINAL	PROC
		;
		;EAX IS INPTR1 TO USE
		;
		PUSH	EBP
		MOV	EBP,ESP
		SUB	ESP,SIZEOF LRF_VARS
		MOV	EDX,EAX
		ASSUME	EBP:PTR LRF_VARS
		JMP	L2$

L1$:
		LEA	ECX,LRF_NFN_STRUCT
		MOV	EAX,OFF LIBSTUFF

		push	EDX
		push	ECX
		push	EAX
		call	_get_filename		;EAX IS FILESTUFF, EDX IS INPTR, ECX IS NFN_STRUCT, RETURNS EAX IS INPTR
		add	ESP,12

		PUSH	EAX
		LEA	EAX,LRF_NFN_STRUCT
		CALL	HANDLE_LIBS
		POP	EDX

L2$:
		MOV	AL,[EDX]
		INC	EDX
		CMP	AL,';'
		JZ	L2$			;SKIP ';'
		CMP	AL,','
		JZ	L2$
		CMP	AL,' '
		JZ	L2$
		CMP	AL,0DH
		JZ	L9$
		DEC	EDX
		CMP	AL,1AH
		JNZ	L1$
L9$:
		MOV	ESP,EBP
		MOV	EAX,EDX
		POP	EBP
		RET

LIB_ROUTINE_FINAL	ENDP


LIB_ROUTINE_FINAL_COMENT	PROC
		;
		;EAX IS INPTR1 TO USE
		;
		PUSH	EBP
		MOV	EBP,ESP

		SUB	ESP,SIZEOF LRF_VARS
		MOV	EDX,EAX
		ASSUME	EBP:PTR LRF_VARS

		JMP	L2$

L1$:
		LEA	ECX,LRF_NFN_STRUCT
		MOV	EAX,OFF LIBSTUFF

		push	EDX
		push	ECX
		push	EAX
		call	_get_filename		;EAX IS FILESTUFF, EDX IS INPTR, ECX IS NFN_STRUCT, RETURNS EAX IS INPTR
		add	ESP,12

		PUSH	EAX
		LEA	EAX,LRF_NFN_STRUCT
		ASSUME	EAX:PTR NFN_STRUCT

		MOV	ECX,[EAX].NFN_PATHLEN
		MOV	EDX,[EAX].NFN_EXTLEN

		ADD	ECX,[EAX].NFN_PRIMLEN
		CMP	EDX,4

		JNZ	L15$

		MOV	DL,[EAX+ECX+1].NFN_TEXT
		LEA	ECX,[EAX+ECX+2].NFN_TEXT

		MOV	AL,DL
		TO_UPPER

		CMP	AL,'L'
		JNZ	L15$

		MOV	AL,[ECX]
		INC	ECX

		TO_UPPER

		CMP	AL,'O'
		JNZ	L15$

		MOV	AL,[ECX]
		INC	ECX

		TO_UPPER

		CMP	AL,'D'
		JZ	L5$
L15$:
		BITT	DEFAULTLIBRARYSEARCH_FLAG		;IGNORE LIBSEARCH REQUESTS?
		JZ	L19$
		LEA	EAX,LRF_NFN_STRUCT
		CALL	HANDLE_LIBS
L19$:
		POP	EDX

L2$:
		MOV	AL,[EDX]
		INC	EDX

		CMP	AL,';'
		JZ	L2$			;SKIP ';'

		CMP	AL,','
		JZ	L2$

		CMP	AL,' '
		JZ	L2$

		CMP	AL,0DH
		JZ	L9$

		DEC	EDX
		CMP	AL,1AH

		JNZ	L1$
L9$:
		MOV	ESP,EBP
		MOV	EAX,EDX

		POP	EBP

		RET

L5$:
		;
		;HANDLE LIKE A STUB FILE
		;
		BITT	LOD_SUPPLIED
		JNZ	L8$
		MOV	AL,DOSX_EXE_TYPE
		XCHG	AL,EXETYPE_FLAG
		OR	AL,AL
		JZ	L51$
		CMP	AL,DOSX_EXE_TYPE
		JZ	L51$
		MOV	AL,DOSX_NONDOSX_ERR
		CALL	ERR_RET
L51$:
		BITT	EXEPACK_SELECTED
		JZ	L52$
		MOV	AL,DOSX_EXEPACK_ERR
		push	EAX
		call	_err_abort
L52$:
		LEA	EAX,LRF_NFN_STRUCT
		SETT	LOD_SUPPLIED
		CALL	HANDLE_LOD
		POP	EDX
		JMP	L9$

L8$:
		MOV	AL,DUP_LOD_ERR
		CALL	WARN_RET
		POP	EDX
		JMP	L9$

LIB_ROUTINE_FINAL_COMENT	ENDP


		ALIGN	4

LIBSTUFF	LABEL	DWORD

		DCA	_ret_false	;DEFAULT TO NUL
		DD	LIB_EXT		;DEFAULT LIB EXTENSION
if	fg_mscmd
		DCA	RET_TRUE	;YES, THIS IS WANTED
		DD	LIB_MSG		;IF PROMPTING...


LIB_MSG		DB	21,'Libraries and Paths: '

endif

LIB_EXT		DB	4,'.lib'

RET_FALSE	PROC

		CMP	AL,AL
		RET

RET_FALSE	ENDP

RET_TRUE	PROC

		OR	AL,1
		RET

RET_TRUE	ENDP

		END

