		TITLE	XREF - Copyright (c) SLR Systems 1994

		INCLUDE MACROS


XREF_STRUCT	STRUC

STORE_PTR	DD	?
BYTES_LEFT	DD	?
BLOCK_BASE	DD	?
BLOCK_CNT	DD	?

XREF_STRUCT	ENDS


		.DATA

		EXTERNDEF	XR_CURNMOD_GINDEX:DWORD,LAST_XR_CURNMOD_GINDEX:DWORD,FIRST_XREF_BLK:DWORD


		.CODE	ROOT_TEXT

		EXTERNDEF	GET_NEW_LOG_BLK:PROC


if	fg_xref

		PUBLIC	STORE_XREF_ENTRY,NEW_XREF_BLK_INIT


STORE_XREF_ENTRY	PROC
		;
		;EAX IS GINDEX TO STORE
		;
		MOV	ECX,XR_CURNMOD_GINDEX
		MOV	EDX,LAST_XR_CURNMOD_GINDEX

		CMP	ECX,EDX
		JNZ	L1$
L3$:

L2$		LABEL	PROC

		MOV	ECX,XREF_STRUCTURE.BYTES_LEFT	;COUNT BYTES LEFT
		MOV	EDX,XREF_STRUCTURE.STORE_PTR	;STORAGE POINTER

		SUB	ECX,4
		JZ	L4$

		MOV	[EDX],EAX
		ADD	EDX,4

		MOV	XREF_STRUCTURE.BYTES_LEFT,ECX
		MOV	XREF_STRUCTURE.STORE_PTR,EDX
L5$:
		RET

L1$:
		PUSH	EAX
		MOV	LAST_XR_CURNMOD_GINDEX,ECX

		MOV	EAX,-1
		CALL	L2$

		MOV	EAX,XR_CURNMOD_GINDEX
		CALL	L2$

		POP	EAX
		JMP	L3$

L4$:
		PUSH	EAX
		MOV	[EDX],EAX
		
		LEA	EAX,[EDX+4]

		MOV	ECX,OFF XREF_STRUCTURE
		ASSUME	ECX:PTR XREF_STRUCT

		CALL	NEW_XREF_BLK
		
		POP	EAX
		RET

STORE_XREF_ENTRY	ENDP


NEW_XREF_BLK	PROC
		;
		;ALLOCATE ANOTHER 16K BLOCK FOR STORING XREF INFORMATION
		;
		;ESI IS PLACE TO STORE NEW BLOCK ADDRESS
		;EBX IS XREF_STRUCTURE
		;
		MOV	EDX,EAX
		CALL	GET_NEW_LOG_BLK 	;GET LOGICAL BLOCK

		MOV	[EDX],EAX
		MOV	[ECX].STORE_PTR,EAX

		MOV	[ECX].BLOCK_BASE,EAX

		INC	[ECX].BLOCK_CNT
		MOV	[ECX].BYTES_LEFT,PAGE_SIZE-4

		RET

NEW_XREF_BLK	ENDP


NEW_XREF_BLK_INIT	PROC

		MOV	EAX,OFF FIRST_XREF_BLK
		MOV	ECX,OFF XREF_STRUCTURE

		CALL	NEW_XREF_BLK
		RET

NEW_XREF_BLK_INIT	ENDP


		.DATA?

		ALIGN	4

XREF_STRUCTURE	XREF_STRUCT<>

endif

		END

