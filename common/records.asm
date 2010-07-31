		TITLE	RECORDS - Copyright (c) SLR Systems 1994

		INCLUDE	MACROS
		INCLUDE	IO_STRUC

		PUBLIC	GET_RECORD,CHECK_CHECKSUM,GET_FIRST_RECORD

if	fg_rom
		PUBLIC	CHECKSUM
endif

		.DATA

		EXTERNDEF	TEMP_RECORD:BYTE

		EXTERNDEF	OBJ_DEVICE:DWORD,RECORD_LENGTH:DWORD,END_OF_RECORD:DWORD,BUFFER_OFFSET:DWORD,RECORD_TYPE:DWORD


		.CODE	PASS1_TEXT

		EXTERNDEF	_err_abort:proc,ERR_RET:PROC,WARN_RET:PROC

		EXTERNDEF	BAD_CHKSUM_ERR:ABS,UNEXP_EOF_ERR:ABS,REC_TOO_LONG_ERR:ABS,INVALID_OBJ_ERR:ABS


GET_FIRST_RECORD	PROC
		;
		;RETURN LENGTH IN BX, REST OF RECORD IN DS:SI
		;
		MOV	EBX,OBJ_DEVICE
		ASSUME	EBX:PTR MYI_STRUCT

		MOV	DL,-1
		XOR	EAX,EAX
		SETT	REC_TYPE_VALID,DL
		MOV	EDX,[EBX].MYI_COUNT
		MOV	ESI,[EBX].MYI_PTRA
		SUB	EDX,3
		JC	RPTR_HELP_AF
		MOV	[EBX].MYI_COUNT,EDX
		MOV	AL,[ESI]
		MOV	BUFFER_OFFSET,ESI
		CMP	AL,80H
		JZ	GR_CONTINUE1
		CMP	AL,6EH
		JZ	GR_CONTINUE1
		BITT	LIB_OR_NOT		;CANNOT BE F0 IF ALREADY IN A
		JNZ	L8$			;LIB
		CMP	AL,0F0H
		JNZ	L8$
GR_CONTINUE1:
		JMP	GR_CONTINUE
L8$:
		SETT	REC_TYPE_VALID
		MOV	AL,INVALID_OBJ_ERR
		push	EAX
		call	_err_abort

RPTR_HELP_AF:
		MOV	AL,3
		RESS	REC_TYPE_VALID,AH
		CALL	RPTR_HELP
		MOV	AL,[ESI]
		MOV	BUFFER_OFFSET,ESI
		MOV	BPTR RECORD_TYPE,AL		;STORE TYPE
		CMP	AL,80H
		JZ	GF_CONT
		CMP	AL,6EH
		JZ	GF_CONT
		BITT	LIB_OR_NOT
		JNZ	L8$
		CMP	AL,0F0H
		JNZ	L8$
GF_CONT:
		XOR	EAX,EAX
		MOV	DL,-1
		MOV	AX,1[ESI]
		ADD	ESI,3
		SETT	REC_TYPE_VALID,DL
		CMP	EAX,MAX_RECORD_LEN+1
		JNC	REC_TOO_LONG
		MOV	ESI,[EBX].MYI_PTRA
		JMP	RPTR_HELP_A_RET

GET_FIRST_RECORD	ENDP


GET_RECORD	PROC
		;
		;REST OF RECORD IN ESI
		;
		MOV	EBX,OBJ_DEVICE
		MOV	DL,-1

		XOR	EAX,EAX
		SETT	REC_TYPE_VALID,DL

		MOV	EDX,[EBX].MYI_COUNT
		MOV	ESI,[EBX].MYI_PTRA

		SUB	EDX,3
		JC	RPTR_HELP_A

		MOV	[EBX].MYI_COUNT,EDX
		MOV	AL,[ESI]

		MOV	BUFFER_OFFSET,ESI
GR_CONTINUE::
		MOV	AH,2[ESI]

		MOV	BPTR RECORD_TYPE,AL	;STORE TYPE
		MOV	AL,1[ESI]

		ADD	ESI,3
RPTR_HELP_A_RET::
		MOV	RECORD_LENGTH,EAX	;LENGTH

		CMP	EAX,MAX_RECORD_LEN+1
		JNC	REC_TOO_LONG
RPTR_A1:
		MOV	EDX,[EBX].MYI_COUNT
		LEA	EDI,[EAX+ESI]

		SUB	EDX,EAX
		MOV	[EBX].MYI_PTRA,EDI

		DEC	EDI
		JC	RPTR_HELP_B

		MOV	[EBX].MYI_COUNT,EDX
		MOV	END_OF_RECORD,EDI

		RET

RPTR_HELP_A:
		MOV	AL,3
		RESS	REC_TYPE_VALID,AH
		CALL	RPTR_HELP
		MOV	AL,[ESI]
		MOV	BUFFER_OFFSET,ESI
		MOV	BPTR RECORD_TYPE,AL		;STORE TYPE
		XOR	EAX,EAX
		MOV	DL,-1
		MOV	AX,1[ESI]
		MOV	ESI,[EBX].MYI_PTRA
		SETT	REC_TYPE_VALID,DL
		JMP	RPTR_HELP_A_RET

REC_TOO_LONG::
		DEC	ESI
		MOV	AL,REC_TOO_LONG_ERR
		MOV	END_OF_RECORD,ESI
		push	EAX
		call	_err_abort

RPTR_HELP_B	LABEL	PROC

		MOV	[EBX].MYI_PTRA,ESI	;RESTORE PTR

RPTR_HELP	LABEL	PROC
		;
		;MOVE WHAT YOU CAN TO TEMP_RECORD
		;
		LEA	ECX,[EDX+EAX]
		MOV	EDI,OFF TEMP_RECORD
		PUSH	EAX		;BYTES TO READ
		SUB	EAX,ECX		;NUMBER TO MOVE AFTER THE FACT
		TEST	ECX,ECX
		PUSH	EDI
		JZ	RH_1
		MOV	ESI,[EBX].MYI_PTRA
		OPTI_MOVSB
RH_1:
		MOV	ESI,EAX
		MOV	EAX,EBX

		push	EAX
		CALL	[EBX].MYI_FILLBUF
		add	ESP,4

		POP	EDX
		MOV	ECX,ESI
		SUB	[EBX].MYI_COUNT,ESI
		MOV	ESI,[EBX].MYI_PTRA
		JC	UNEXP_EOF
		POP	EAX
		OPTI_MOVSB
		MOV	[EBX].MYI_PTRA,ESI
		LEA	ECX,[EDX+EAX-1]
		MOV	ESI,EDX
		MOV	END_OF_RECORD,ECX
		RET

UNEXP_EOF:
		MOV	AL,UNEXP_EOF_ERR
		push	EAX
		call	_err_abort

GET_RECORD	ENDP


CHECK_CHECKSUM	PROC
		;
		;RECALC. THE CHECKSUM FROM THE DATA AND COMPARE TO THE
		;CHECKSUM READ IN FROM BUFFER
		;
		MOV	ECX,RECORD_LENGTH
		MOV	EBX,ESI
		LEA	EAX,[ESI+ECX-1]
		DEC	ECX
		CMP	BPTR [EAX],0	;DON'T BOTHER IF NOT THERE...
		JZ	DO_RET1
		CALL	CHECKSUM
		SUB	DL,BPTR RECORD_TYPE	;ALSO PART OF RECORDS CHECK SUM	
		SUB	DL,BPTR RECORD_LENGTH
		SUB	DL,BPTR RECORD_LENGTH+1

		CMP	[EBX],DL
		JNZ	CC_1
		RET

CC_1:
		MOV	AL,BAD_CHKSUM_ERR
		CALL	WARN_RET
DO_RET1:
		RET

CHECK_CHECKSUM	ENDP


		ASSUME	EBX:NOTHING

CHECKSUM	PROC
		;
		;EBX IS START OF RECORD
		;ECX IS RECORD LENGTH
		;
		XOR	EDX,EDX
		;
		;CALCULATE CHECKSUM
		;
		PUSH	ECX
		SHR	ECX,2		;DIVIDE LENGTH BY 4
		JZ	SMALL		;JUMP IF LESS THAN 4 BYTES
L1:
		MOV	EAX,[EBX]
		ADD	EBX,4
		ADD	DL,AL
		ADD	DH,AH
		SHR	EAX,16
		ADD	DH,DL
		ADD	DL,AL
		ADD	DH,AH
		DEC	ECX
		JNZ	L1
		ADD	DL,DH
SMALL:
		POP	ECX		;NOW SLOW LOOP FOR MOD 8 BYTES
		AND	ECX,7
		JZ	AQW
LOOPT:
		ADD	DL,[EBX]
		INC	EBX
		DEC	ECX
		JNZ	LOOPT
AQW:
		NEG	DL		;DL IS DESIRED CHECKSUM
		RET

CHECKSUM	ENDP


		END

