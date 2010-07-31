		TITLE	RESOURCE - Copyright (c) SLR Systems 1994

		INCLUDE	MACROS
		INCLUDE	IO_STRUC
		INCLUDE	RESSTRUC
		INCLUDE	EXES

		PUBLIC	READ_SIZE_RESOURCES


		.DATA

		EXTERNDEF	SYMBOL_TEXT:BYTE,IMPEXP_MOD:BYTE,IMPEXP_NAM:BYTE

		EXTERNDEF	SYMBOL_LENGTH:DWORD,RESOURCE_FLAGS:DWORD,OBJ_DEVICE:DWORD,NEXT_RESOURCE_MASTER_PTR:DWORD
		EXTERNDEF	RESOURCE_TYPE_ID:DWORD,NEXT_RESOURCE_PTR:DWORD,RESOURCE_TYPE_PTR:DWORD,RESOURCE_SIZE:DWORD
		EXTERNDEF	RESOURCE_BLOCK_MASTER_PTRS:DWORD,RESOURCE_FILE_ADDR:DWORD,RESOURCE_TABLE_SIZE:DWORD
		EXTERNDEF	RESOURCE_NAME_OFFSET:DWORD,RESTYPE_HASH_TABLE_PTR:DWORD,RESNAME_HASH_TABLE_PTR:DWORD
		EXTERNDEF	RES_TYPENAME_HASH_TABLE_PTR:DWORD,RESTYPE_N_BYORD:DWORD,RESTYPE_N_BYNAME:DWORD
		EXTERNDEF	RESOURCE_HASHES:DWORD,CURN_FILE_LIST_GINDEX:DWORD

		EXTERNDEF	RC_LIST:FILE_LISTS,NEXEHEADER:NEXE,_FILE_LIST_GARRAY:STD_PTR_S,RESOURCE_STUFF:ALLOCS_STRUCT
		EXTERNDEF	RESTYPE_GARRAY:STD_PTR_S


		.CODE	PASS2_TEXT

		EXTERNDEF	_open_reading:proc,INSTALL_RESOURCE_TYPE:PROC,INSTALL_RESOURCE:PROC,INSTALL_RESOURCE_NAME:PROC
		EXTERNDEF	OPTI_HASH_SIGNIFICANT:PROC,WARN_RC_RET:PROC,_close_myi_handle:proc,ERR_RET:PROC
		EXTERNDEF	ERR_ABORT:PROC,GET_NEW_LOG_BLK:PROC,INSTALL_RES_TYPE_NAME:PROC,INSTALL_RTNL:PROC
		EXTERNDEF	RESOURCE_POOL_GET:PROC

		EXTERNDEF	DUP_RESOURCE_ERR:ABS,RESOURCE_CORRUPT_ERR:ABS,RES3216_ERR:ABS


RES_VARS	STRUC

RESOURCE_BYTES_LEFT_BP	DD	?
RESOURCE_BYTES_READ_BP	DD	?
DELTA_ICON_BP		DD	?
DELTA_CURSOR_BP		DD	?
MY_MAX_ICON_BP		DD	?
MY_MAX_CURSOR_BP	DD	?
RESOURCE_TYPE_BP	DD	?
RESOURCE_NAME_BP	DD	?
RESOURCE_COUNT_BP	DD	?
RESOURCE_LANG_BP	DD	?

RES_VARS	ENDS


FIX	MACRO	X

X	EQU	([EBP-SIZE RES_VARS].(X&_BP))

	ENDM


FIX	RESOURCE_BYTES_LEFT
FIX	RESOURCE_BYTES_READ
FIX	DELTA_ICON
FIX	DELTA_CURSOR
FIX	MY_MAX_ICON
FIX	MY_MAX_CURSOR
FIX	RESOURCE_TYPE
FIX	RESOURCE_NAME
FIX	RESOURCE_COUNT
FIX	RESOURCE_LANG


READ_SIZE_RESOURCES	PROC
		;
		;IF ADDING .RES TO EXE, NOW IS THE TIME TO DETERMINE THE SIZE
		;OF THE RESOURCE TABLE.  TO DO THIS WE MUST READ THE WHOLE
		;.RES FILE, BUILDING SEVERAL TABLES IN THE PROCESS
		;
		BITT	RC_SUPPLIED
		JZ	L09$			;NO
		MOV	EAX,RC_LIST.FILE_FIRST_GINDEX
		CONVERT	EAX,EAX,_FILE_LIST_GARRAY
		ASSUME	EAX:PTR FILE_LIST_STRUCT

		MOV	EAX,[EAX].FILE_LIST_NEXT_GINDEX

		OR	EAX,EAX
		JZ	L09$

		PUSHM	EBP,EDI,ESI,EBX

		MOV	EBP,ESP
		SUB	ESP,SIZE RES_VARS
		ASSUME	EBP:PTR RES_VARS

		PUSH	EAX
		XOR	EAX,EAX

		MOV	DELTA_ICON,EAX
		MOV	DELTA_CURSOR,EAX

		MOV	MY_MAX_ICON,EAX
		MOV	MY_MAX_CURSOR,EAX

		MOV	RESOURCE_BYTES_READ,EAX
		MOV	RESOURCE_COUNT,EAX

		CALL	RSRC_INST_INIT
		JMP	L0$

L09$:
		RET

L0$:
		POP	ESI

		TEST	ESI,ESI
		JZ	L19$

		MOV	EAX,ESI
		CONVERT	ESI,ESI,_FILE_LIST_GARRAY
		ASSUME	ESI:PTR FILE_LIST_STRUCT
		MOV	ECX,[ESI].FILE_LIST_NEXT_GINDEX

		PUSH	ECX
		push	EAX
		call	_open_reading		;GET OPEN OLD FILE...
		add	ESP,4
		ASSUME	EAX:PTR MYI_STRUCT
		ASSUME	ESI:NOTHING

		test	EAX,EAX
		stc
		jz	L0$

		MOV	EAX,[EAX].MYI_FILE_LIST_GINDEX
		CONVERT	EAX,EAX,_FILE_LIST_GARRAY
		ASSUME	EAX:PTR FILE_LIST_STRUCT

		MOV	EAX,[EAX].FILE_LIST_NFN.NFN_FILE_LENGTH
		MOV	ECX,MY_MAX_ICON

		MOV	RESOURCE_BYTES_LEFT,EAX
		OR	EAX,EAX			;ZERO-LENGTH .RES FILE...

		MOV	EAX,MY_MAX_CURSOR
		JZ	L41$

		ADD	DELTA_ICON,ECX
		ADD	DELTA_CURSOR,EAX

		XOR	EAX,EAX

		MOV	MY_MAX_ICON,EAX
		MOV	MY_MAX_CURSOR,EAX

		CALL	DETERMINE_FORMAT	;16 VS 32 BIT RESOURCE...

		BITT	RES_32_BIT
		JNZ	L2$

		CALL	READ_16_BIT_RESOURCES
		JMP	L41$

L1$:
		MOV	AL,RES3216_ERR
		CALL	ERR_RET
		JMP	L21$

L2$:
		BITT	OUTPUT_32BITS
		JZ	L1$
L21$:
		CALL	READ_32_BIT_RESOURCES
L41$:
		;
		;ROUND UP TO BLOCK BOUNDARY FOR MULTIPLE FILES...
		;
		MOV	EAX,RESOURCE_BYTES_READ

		ADD	EAX,PAGE_SIZE-1

		AND	EAX,NOT (PAGE_SIZE-1)

		MOV	RESOURCE_BYTES_READ,EAX
		;
		;CLOSE INPUT FILE
		;
		JMP	L0$

L19$:

if	fgh_inthreads
		BITT	_HOST_THREADED		;SKIP IF THREADED
		JNZ	L191$
endif
		MOV	EAX,OBJ_DEVICE
	push	EAX
	call	_close_myi_handle
	add	ESP,4
L191$:
		;
		;CALCULATE SIZE OF RESOURCE TABLE
		;
		MOV	EAX,RESTYPE_N_BYORD
		MOV	ECX,RESTYPE_N_BYNAME

		ADD	ECX,EAX
		MOV	EAX,12

		MUL	RESOURCE_COUNT

		SHL	ECX,3

		ADD	EAX,ECX
		JZ	L9$

		ADD	EAX,4			;SHIFT COUNT + NUL TYPE

		MOV	RESOURCE_TABLE_SIZE,EAX
		ADD	EAX,RESOURCE_NAME_OFFSET	;# OF BYTES IN NAME TABLE

		MOVZX	ECX,NEXEHEADER._NEXE_RESNAM_OFFSET

		ADD	EAX,ECX

		MOV	NEXEHEADER._NEXE_RESNAM_OFFSET,AX

		CMP	EAX,64K
		JNC	L99$
L9$:
		XOR	EAX,EAX
		MOV	ESP,EBP

		MOV	CURN_FILE_LIST_GINDEX,EAX

		POPM	EBX,ESI,EDI,EBP

		RET

L99$:
		SETT	EXEHDR_ERROR_FLAG

		JMP	L9$

READ_SIZE_RESOURCES	ENDP


READ_32_BIT_RESOURCES	PROC	NEAR
		;
		;
		;
L3$:
		;
		;ESI IS INPUT POINTER, EBX IS BYTES LEFT THIS BLOCK
		;
		CALL	READ_NEXT_RESOURCE_32

		TEST	EBX,EBX
		JNZ	L3$
L4$:
		MOV	EAX,RESOURCE_BYTES_LEFT

		OR	EAX,EAX
		JNZ	L3$

		RET


READ_32_BIT_RESOURCES	ENDP


READ_NEXT_RESOURCE_32	PROC	NEAR
		;
		;READ A 32-BIT RESOURCE FROM .RES FILE
		;
		CALL	GET_RESOURCE_DWORD		;FIRST IS RESOURCE SIZE

		MOV	RESOURCE_SIZE,EAX
		CALL	GET_RESOURCE_DWORD		;THEN HEADER SIZE
		;
		;RESOURCE TYPE
		;
		CALL	GET_RESOURCE_NAME_32		;RETURNS ID # OR NAME_GINDEX

		MOV	RESOURCE_TYPE,EAX		;<64K == ID, > == NAME
		;
		;THEN COMES RESOURCE NAME
		;
		CALL	GET_RESOURCE_NAME_32

		MOV	RESOURCE_NAME,EAX
		CALL	DWORD_ALIGN_INPUT

		CALL	GET_RESOURCE_DWORD		;SKIP DATAVERSION

		CALL	GET_RESOURCE_DWORD

		MOV	ECX,EAX
		OR	AL,1				;MARK 32-BIT RESOURCE

		SHR	ECX,16
		AND	EAX,0FFFFH

		MOV	RESOURCE_FLAGS,EAX
		MOV	RESOURCE_LANG,ECX

		CALL	GET_RESOURCE_DWORD		;SKIP VERSION

		CALL	GET_RESOURCE_DWORD		;SKIP CHARACTERISTICS

		CALL	INSTALL_AND_MOVE

		JMP	DWORD_ALIGN_INPUT

READ_NEXT_RESOURCE_32	ENDP


DWORD_ALIGN_INPUT	PROC	NEAR
		;
		;
		;
		XOR	EAX,EAX

		SUB	EAX,ESI

		AND	EAX,3

		ADD	ESI,EAX
		SUB	EBX,EAX

		RET

DWORD_ALIGN_INPUT	ENDP


INSTALL_AND_MOVE	PROC	NEAR
		;
		;
		;
		XOR	ECX,ECX
		MOV	EAX,RESOURCE_TYPE

		MOV	RESOURCE_TYPE_ID,ECX		;FOR CONVERSION OR COMPRESSION
		CMP	EAX,64K

		JAE	L3$
		;
		;SOME RESOURCES GET SPECIAL HANDLING...
		;
		CMP	EAX,000FH			;SKIP NAME STRINGS
		JZ	L8$

		MOV	RESOURCE_TYPE_ID,EAX
		CALL	SPECIAL_16			;SAME AS 16-BIT CHECKING...
L3$:
		;
		;NOW, INSTALL THIS RESOURCE
		;
		MOV	EAX,RESOURCE_TYPE
		MOV	ECX,RESOURCE_BYTES_READ

		SUB	ECX,EBX

		MOV	RESOURCE_FILE_ADDR,ECX		;CALCULATE FILE ADDRESS
		CALL	INSTALL_RESOURCE_TYPE		;AX IS RESOURCE TYPE GINDEX

		MOV	ECX,EAX
		MOV	EAX,RESOURCE_NAME

		CALL	INSTALL_RES_TYPE_NAME		;OK, ADD THIS RESOURCE TO TYPE

		MOV	ECX,RESOURCE_LANG		;LANGUAGE IS NULL
		CALL	INSTALL_RTNL

		MOV	EAX,RESOURCE_COUNT
		JC	L4$				;OOPS, DUPLICATE RESOURCE?

		INC	EAX

		MOV	RESOURCE_COUNT,EAX
L4$:
		GETT	AL,POST_INSTALL_FIX

		OR	AL,AL
		JNZ	POST_INSTALL_FIXER
L8$:
		MOV	EAX,RESOURCE_SIZE
		JMP	SKIP_EAX_RESOURCE_BYTES	;POINT TO NEXT RESOURCE

INSTALL_AND_MOVE	ENDP


READ_16_BIT_RESOURCES	PROC	NEAR
		;
		;
		;
		XOR	EAX,EAX

		MOV	RESOURCE_LANG,EAX
L3$:
		;
		;DS:SI IS INPUT POINTER, CX IS BYTES LEFT THIS BLOCK
		;
		CALL	READ_NEXT_RESOURCE

		OR	EBX,EBX
		JNZ	L3$

		MOV	EAX,RESOURCE_BYTES_LEFT

		OR	EAX,EAX
		JNZ	L3$

		RET


READ_16_BIT_RESOURCES	ENDP


READ_NEXT_RESOURCE	PROC	NEAR
		;
		;READ A RESOURCE FROM .RES FILE
		;
		;
		;FIRST IS RESOURCE TYPE
		;
		CALL	GET_RESOURCE_NAME	;RETURNS ID # OR NAME_GINDEX

		MOV	RESOURCE_TYPE,EAX
		;
		;THEN COMES RESOURCE NAME
		;
		CALL	GET_RESOURCE_NAME

		MOV	RESOURCE_NAME,EAX
		CALL	GET_RESOURCE_WORD

		AND	EAX,0FFFEH			;MARK 16-BIT RESOURCE

		MOV	RESOURCE_FLAGS,EAX
		CALL	GET_RESOURCE_DWORD

		MOV	RESOURCE_SIZE,EAX
		JMP	INSTALL_AND_MOVE

READ_NEXT_RESOURCE	ENDP


SPECIAL_16	PROC	NEAR
		;
		;POSSIBLY SPECIAL HANDLING...
		;
		CMP	EAX,1		;CURSOR
		JZ	L1$

		CMP	EAX,3		;ICON
		JZ	L3$

		CMP	EAX,12		;GROUP_CURSOR
		JZ	L5$

		CMP	EAX,14		;GROUP_ICON
		JZ	L7$
L9$:
		RET

L1$:
		;
		;CURSOR RESOURCE
		;
		MOV	EAX,RESOURCE_NAME

		CMP	EAX,64K
		JAE	L9$

		MOV	ECX,MY_MAX_CURSOR
		MOV	EDX,DELTA_CURSOR

		CMP	ECX,EAX
		JA	L11$

		MOV	MY_MAX_CURSOR,EAX
L11$:
		ADD	EAX,EDX

		MOV	RESOURCE_NAME,EAX

		RET

L3$:
		;
		;ICON RESOURCE
		;
		MOV	EAX,RESOURCE_NAME

		CMP	EAX,64K
		JAE	L9$

		MOV	ECX,MY_MAX_ICON
		MOV	EDX,DELTA_ICON

		CMP	ECX,EAX
		JA	L31$

		MOV	MY_MAX_ICON,EAX
L31$:
		ADD	EAX,EDX

		MOV	RESOURCE_NAME,EAX

		RET

L5$:
		;
		;GROUP CURSOR
		;
		MOV	EAX,DELTA_CURSOR
		MOV	CL,-1

		OR	EAX,EAX
		JZ	L59$

		SETT	POST_INSTALL_FIX,CL
L59$:
		RET


L7$:
		;
		;GROUP ICON
		;
		MOV	EAX,DELTA_ICON
		MOV	CL,-1

		OR	EAX,EAX
		JZ	L79$

		SETT	POST_INSTALL_FIX,CL
L79$:
		RET

SPECIAL_16	ENDP


POST_INSTALL_FIXER	PROC	NEAR
		;
		;
		;
		MOV	EAX,RESOURCE_TYPE

		RESS	POST_INSTALL_FIX

		CMP	EAX,12
		JZ	ADJUST_GROUP_CURSOR

POST_INSTALL_FIXER	ENDP


ADJUST_GROUP_ICON	PROC	NEAR
		;
		;THESE POINTERS NEED ADJUSTED
		;
		MOV	EAX,4
		CALL	SKIP_EAX_RESOURCE_BYTES

		XOR	EAX,EAX
		CALL	GET_RESOURCE_WORD

		INC	EAX
		PUSH	EDI

		MOV	EDI,EAX
		CALL	RES_32_SKIP_WORD

		JMP	L7$

L1$:
		MOV	EAX,12
		CALL	SKIP_EAX_RESOURCE_BYTES

		MOV	EAX,DELTA_ICON
		CALL	ADD_TO_RESOURCE_WORD

		CALL	RES_32_SKIP_WORD
L7$:
		DEC	EDI
		JNZ	L1$

		POP	EDI

		RET

ADJUST_GROUP_ICON	ENDP


ADJUST_GROUP_CURSOR	PROC	NEAR
		;
		;
		;
		MOV	EAX,4
		CALL	SKIP_EAX_RESOURCE_BYTES

		XOR	EAX,EAX
		CALL	GET_RESOURCE_WORD

		INC	EAX
		PUSH	EDI

		MOV	EDI,EAX
		CALL	RES_32_SKIP_WORD

		JMP	L7$

L1$:
		MOV	EAX,12
		CALL	SKIP_EAX_RESOURCE_BYTES

		MOV	EAX,DELTA_CURSOR
		CALL	ADD_TO_RESOURCE_WORD

		CALL	RES_32_SKIP_WORD
L7$:
		DEC	EDI
		JNZ	L1$

		POP	EDI

		RET

ADJUST_GROUP_CURSOR	ENDP


RES_32_SKIP_WORD	PROC	NEAR
		;
		;
		;
		GETT	AL,RES_32_BIT

		OR	AL,AL
		JNZ	GET_RESOURCE_WORD

		RET

RES_32_SKIP_WORD	ENDP


ADD_TO_RESOURCE_WORD	PROC	NEAR
		;
		;AX MUST BE ADDED TO CURRENT WORD AS YOU SKIP IT...
		;
		SUB	EBX,2
		JB	L5$

		ADD	WPTR [ESI],AX
		ADD	ESI,2

		RET

L5$:
		ADD	EBX,2
		PUSH	EDX

		MOV	EDX,EAX
		CALL	GET_RESOURCE_BYTE

		ADD	AL,DL

		PUSHFD

		MOV	[ESI-1],AL
		CALL	GET_RESOURCE_BYTE

		POPFD

		ADC	AL,DH
		POP	EDX

		MOV	[ESI-1],AL

		RET

ADD_TO_RESOURCE_WORD	ENDP


GET_RESOURCE_NAME	PROC	NEAR
		;
		;GET NEXT RESOURCE NAME, RETURN ID# OR TEXT OFFSET
		;
		MOV	EDI,OFF SYMBOL_TEXT
		CALL	GET_RESOURCE_BYTE

		CMP	AL,-1
		JNZ	L5$
		;
		;ID #'S PRECEDED BY A 0FFH
		;
		XOR	EAX,EAX
		JMP	GET_RESOURCE_WORD

;		AND	EAX,7FFFH			;MAKE SURE IT WASN'T SET

;		RET

L2$:
		CALL	GET_RESOURCE_BYTE
L5$:
		XOR	CL,CL
		MOV	[EDI],AL

		MOV	[EDI+1],CL				;STORE UNICODE
		ADD	EDI,2

		CMP	EDI,OFF SYMBOL_TEXT+SYMBOL_TEXT_SIZE
		JA	L99$

		OR	AL,AL
		JNZ	L2$
GRN_1::
		PUSH	ESI
		SUB	EDI,OFF SYMBOL_TEXT+2

		MOV	ESI,OFF SYMBOL_TEXT
		MOV	SYMBOL_LENGTH,EDI
		;
		;NOW, INSTALL THIS IN RESOURCE_NAMES_TABLE
		;
		MOV	EAX,EDI
		CALL	OPTI_HASH_SIGNIFICANT		;JUST DO HASH...

		POP	ESI
		JMP	INSTALL_RESOURCE_NAME		;EAX IS GINDEX

L99$:
		JMP	RES_ERROR

GET_RESOURCE_NAME	ENDP


GET_RESOURCE_NAME_32	PROC	NEAR
		;
		;GET NEXT RESOURCE NAME, RETURN ID# OR TEXT OFFSET
		;
		XOR	EAX,EAX

		MOV	EDI,OFF SYMBOL_TEXT
		CALL	GET_RESOURCE_WORD

		CMP	AX,-1
		JNZ	L5$
		;
		;ID #'S PRECEDED BY A 0FFH
		;
		XOR	EAX,EAX				;MARK IT AN ID #
		JMP	GET_RESOURCE_WORD

L2$:
		CALL	GET_RESOURCE_WORD
L5$:
		MOV	[EDI],EAX			;dword zero terminate
		ADD	EDI,2

		CMP	EDI,OFF SYMBOL_TEXT+SYMBOL_TEXT_SIZE
		JA	L99$

		AND	EAX,0FFFFH
		JNZ	L2$

		JMP	GRN_1

L99$:
		JMP	RES_ERROR

GET_RESOURCE_NAME_32	ENDP


GET_RESOURCE_BYTE	PROC	NEAR
		;
		;GET NEXT BYTE FROM RESOURCE FILE
		;
		DEC	EBX
		JS	L5$

		MOV	AL,[ESI]
		INC	ESI

		RET

L5$:
		CALL	FILL_RESOURCE_BUFFER

		TEST	EBX,EBX
		JNZ	GET_RESOURCE_BYTE

		JMP	RES_ERROR

GET_RESOURCE_BYTE	ENDP


GET_RESOURCE_WORD	PROC	NEAR
		;
		;GET NEXT WORD FROM RESOURCE FILE
		;
		SUB	EBX,2
		JB	L5$

		MOV	AL,[ESI]

		MOV	AH,[ESI+1]
		ADD	ESI,2

		RET

L5$:
		;
		;RARE, SO GO AHEAD AND DO IT THE SLOW WAY
		;
		PUSH	ECX
		ADD	EBX,2

		CALL	GET_RESOURCE_BYTE

		MOV	ECX,EAX
		CALL	GET_RESOURCE_BYTE

		MOV	AH,AL

		MOV	AL,CL
		POP	ECX

		RET

GET_RESOURCE_WORD	ENDP


GET_RESOURCE_DWORD	PROC	NEAR
		;
		;GET NEXT WORD FROM RESOURCE FILE
		;
		SUB	EBX,4
		JB	L5$

		MOV	EAX,[ESI]
		ADD	ESI,4

		RET

L5$:
		;
		;RARE, SO GO AHEAD AND DO IT THE SLOW WAY
		;
		PUSH	ECX
		ADD	EBX,4

		CALL	GET_RESOURCE_WORD

		MOV	ECX,EAX
		CALL	GET_RESOURCE_WORD

		SHL	EAX,16
		AND	ECX,0FFFFH

		OR	EAX,ECX
		POP	ECX

		RET

GET_RESOURCE_DWORD	ENDP


SKIP_EAX_RESOURCE_BYTES	PROC	NEAR
		;
		;ESI IS CURRENT POINTER, EBX IS BYTES LEFT THIS BLOCK
		;
		CMP	EAX,EBX			;SKIPPING MORE THAN IS LEFT
		JA	L1$			;DO LARGE

		ADD	ESI,EAX
		SUB	EBX,EAX

		RET

L1$:
		;
		;SKIP CX BYTES
		;
		SUB	EAX,EBX
		CALL	FILL_RESOURCE_BUFFER		;GET ANOTHER BUFFER FULL

		TEST	EBX,EBX
		JNZ	SKIP_EAX_RESOURCE_BYTES
RES_ERROR::
		MOV	AL,RESOURCE_CORRUPT_ERR
		CALL	ERR_ABORT

SKIP_EAX_RESOURCE_BYTES	ENDP


DETERMINE_FORMAT	PROC	NEAR
		;
		;IS THIS A 16-BIT OR 32-BIT .RES FILE?
		;
		CALL	FILL_RESOURCE_BUFFER

		MOV	ECX,00FFH
		MOV	EAX,[ESI]

		RESS	RES_32_BIT,CH
		TEST	EAX,0FFFFH

		JNZ	L0$

		TEST	EAX,0FFFF0000H
		JNZ	L5$

		SETT	RES_32_BIT,CL
		MOV	EAX,[ESI+4]

		CMP	EAX,20H
		JNZ	L5$

		SUB	EBX,20H
		JC	L5$

		ADD	ESI,20H
L0$:
		RET

L5$:
		JMP	RES_ERROR

DETERMINE_FORMAT	ENDP


FILL_RESOURCE_BUFFER	PROC	NEAR
		;
		;READ NEXT BLOCK FROM .RES FILE, ADJUST POINTERS AS NEEDED
		;
		PUSHM	EDX,ECX,EAX
		MOV	EAX,OBJ_DEVICE
		ASSUME	EAX:PTR MYI_STRUCT

		PUSH	EDI
		MOV	EBX,EAX
		ASSUME	EBX:PTR MYI_STRUCT

		push	EAX
		CALL	[EAX].MYI_FILLBUF		;READ NEXT BLOCK
		add	ESP,4
		;
		;NOW, MOVE STUFF OUT OF MYI STRUCTURE...
		;
		ASSUME	EAX:NOTHING

L1$:
		MOV	ESI,RESOURCE_BLOCK_MASTER_PTRS
		MOV	ECX,NEXT_RESOURCE_MASTER_PTR

		TEST	ESI,ESI
		JZ	L8$
L2$:
		MOV	EDI,[ESI+ECX]
		MOV	EAX,NEXT_RESOURCE_PTR

		TEST	EDI,EDI
		JZ	L7$

		MOV	ECX,[EBX].MYI_PTRA

		MOV	[EDI+EAX],ECX
		ADD	EAX,4

		CMP	EAX,256
		JZ	L4$

		MOV	NEXT_RESOURCE_PTR,EAX
L5$:
		XOR	EAX,EAX
		MOV	ESI,[EBX].MYI_PTRA

		MOV	[EBX].MYI_PTRA,EAX
		MOV	[EBX].MYI_BLOCK,EAX

		MOV	EBX,[EBX].MYI_COUNT

		SUB	RESOURCE_BYTES_LEFT,EBX
		POP	EDI

		ADD	RESOURCE_BYTES_READ,EBX
		POPM	EAX,ECX,EDX

		RET

L4$:
		ADD	NEXT_RESOURCE_MASTER_PTR,4
		JMP	L5$

L7$:
		MOV	EAX,256
		CALL	RESOURCE_POOL_GET	;SSYM_POOL_ALLOC

		MOV	[ESI+ECX],EAX
		MOV	EDI,EAX

		MOV	ECX,64
		XOR	EAX,EAX

		REP	STOSD

		MOV	NEXT_RESOURCE_PTR,EAX
		JMP	L1$

L8$:
		MOV	EAX,256
		CALL	RESOURCE_POOL_GET	;SSYM_POOL_ALLOC

		MOV	RESOURCE_BLOCK_MASTER_PTRS,EAX
		MOV	EDI,EAX

		MOV	ECX,64
		XOR	EAX,EAX

		REP	STOSD

		MOV	NEXT_RESOURCE_MASTER_PTR,EAX
		JMP	L1$

FILL_RESOURCE_BUFFER	ENDP


RSRC_INST_INIT	PROC
		;
		;
		;
		CALL	GET_NEW_LOG_BLK

		MOV	RESOURCE_HASHES,EAX

		ADD	RESTYPE_HASH_TABLE_PTR,EAX
		MOV	EDI,EAX

		ADD	RESNAME_HASH_TABLE_PTR,EAX
		MOV	ECX,PAGE_SIZE/4

		ADD	RES_TYPENAME_HASH_TABLE_PTR,EAX
		XOR	EAX,EAX

		REP	STOSD

		RET

RSRC_INST_INIT	ENDP


		END

