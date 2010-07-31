		TITLE	QSORTLIN - Copyright (c) SLR Systems 1994

		INCLUDE	MACROS

if	fg_cvpack
		INCLUDE	CVSTUFF

		PUBLIC	QSORT_LINNUMS


		.DATA

		EXTERNDEF	EXETABLE:DWORD,HIGH_WATER:DWORD

		EXTERNDEF	MODULE_GARRAY:STD_PTR_S,SYMBOL_GARRAY:STD_PTR_S


		.CODE	PASS2_TEXT

		EXTERNDEF	GET_NEW_LOG_BLK:PROC,STORE_XREF_ENTRY:PROC,_err_abort:proc,DOT:PROC

		EXTERNDEF	CV_TOO_MANY_ERR:ABS


QUICK_VARS	STRUC

QLEFT_BASE_BP		DD	?
QMIDDLE_BASE_BP		DD	?
QRIGHT_BASE_BP		DD	?
QLEFT_LOG_BP		DD	?
QRIGHT_LOG_BP		DD	?
QDELTA_BP		DD	?
QLEFT_NUMBER_BP		DD	?
QRIGHT_NUMBER_BP	DD	?
QLEFT_PTR_BP		DD	?
QRIGHT_PTR_BP		DD	?
QMIDDLE_PTR_BP		DD	?
Q_TEMP_WORD_BP		DD	?
QN_COUNT_BP		DD	?

QUICK_VARS	ENDS


FIX	MACRO	X

X	EQU	([EBP-SIZE QUICK_VARS].(X&_BP))

	ENDM


FIX	QLEFT_BASE
FIX	QMIDDLE_BASE
FIX	QRIGHT_BASE
FIX	QLEFT_LOG
FIX	QRIGHT_LOG
FIX	QDELTA
FIX	QLEFT_NUMBER
FIX	QRIGHT_NUMBER
FIX	Q_OS2_NUMBER
FIX	QLEFT_PTR
FIX	QRIGHT_PTR
FIX	QMIDDLE_PTR
FIX	Q_TEMP_WORD
FIX	QN_COUNT

QSORT_LINNUMS	PROC
		;
		;USE QUICKSORT ALGORITHM TO SORT SYMBOL TABLE IN NUMERIC
		;ORDER
		;
		;
		;FIRST COPY INDEXES TO ANOTHER AREA FOR EASY SORTING
		;
		PUSHM	EBP,EDI,ESI,EBX		;SAVE THAT STACK FRAME

		MOV	EBP,ESP
		ASSUME	EBP:PTR QUICK_VARS
		SUB	ESP,SIZE QUICK_VARS

		CALL	QSORT_INIT
		;
		;QUICK SCAN LOOKING FOR ANY OUT OF PLACE
		;
		MOV	ECX,EDI

		SUB	ECX,ESI
		JZ	L5$

		PUSHM	ESI,EDI

		XOR	EDI,EDI
		XOR	EDX,EDX

		MOV	Q_TEMP_WORD,ESI
		CALL	Q_SET_LEFT_BLOCK

		ASSUME	ESI:PTR CVL_STRUCT,EDI:PTR CVL_STRUCT

		INC	ECX
L0$:
		LEA	EBX,[ESI+PAGE_SIZE]

L1$:
		MOV	EAX,[ESI]._CVL_OFFSET
		ADD	ESI,SIZE CVL_STRUCT

		CMP	EDX,EAX
		JA	L4$
L2$:
		DEC	ECX
		JZ	L6$

		MOV	EDX,EAX
		CMP	ESI,EBX

		JNZ	L1$

		MOV	ESI,Q_TEMP_WORD

		ADD	ESI,PAGE_SIZE/SIZE CVL_STRUCT

		MOV	Q_TEMP_WORD,ESI
		CALL	Q_SET_LEFT_BLOCK

		JMP	L0$

L4$:
		POPM	EDI,ESI

if	debug
		CALL	DOT
endif		
		CALL	QUICK_LIN

L5$:
		MOV	EAX,QN_COUNT
		MOV	ESP,EBP

		POPM	EBX,ESI,EDI,EBP

		RET

L6$:
		POPM	EDI,ESI

		JMP	L5$

QSORT_LINNUMS	ENDP


QUICK_LINNUM_1	PROC	NEAR	PRIVATE

QUICK_2:
		;
		;JUST SORT THE TWO AND RETURN...
		;
		CALL	Q_SET_LEFT_BLOCK	;DS:SI
		CALL	Q_SET_RIGHT_BLOCK	;ES:DI
		JMP	QL_SORT2

QUICK3:
		POP	EAX
QUICK2_FINISH:
QUICK_DONE:

		RET

QUICK_LIN::
		;
		;OK BOYS, HERE GOES A QUICK-SORT IMPLEMENTATION...
		;
		;SI IS LEFT LINNUM #, DI IS RIGHT LINNUM #
		;
		MOV	ECX,EDI
QUICK_LIN_1:
		SUB	ECX,ESI
		JNA	QUICK_DONE	;RIGHT <= LEFT, QUIT
		;
		;WHAT ABOUT REAL SMALL CX ?
		;
		INC	ECX
		JZ	QUICK_DONE

		CMP	ECX,2
		JZ	QUICK_2

		MOV	QDELTA,ECX

		SHR	ECX,1
		PUSH	ESI		;SAVE ORIGINAL LEFTY

		ADD	ECX,ESI		;HALF WAY IN BETWEEN...
		MOV	QLEFT_NUMBER,ESI

		MOV	QRIGHT_NUMBER,EDI
		CALL	Q_SET_LEFT_BLOCK	;DS:SI

		MOV	QLEFT_PTR,ESI
		CALL	Q_SET_RIGHT_BLOCK	;ES:DI

		MOV	QRIGHT_PTR,EDI
		CALL	Q_SET_MIDDLE_BLOCK	;ES:DI

		MOV	QMIDDLE_PTR,EDI
		;
		;DO THREE-SOME SORT
		;
		;IF LEFT>MIDDLE, XCHG LEFT&MIDDLE
		;
		CALL	QL_SORT2
		;
		;IF LEFT > RIGHT, XCHG LEFT&RIGHT
		;
		MOV	EDI,QRIGHT_PTR
		CALL	QL_SORT2
		;
		;LASTLY, IF MIDDLE > RIGHT, XCHG MIDDLE&RIGHT
		;
		MOV	ESI,QMIDDLE_PTR
		CALL	QL_SORT2

		CMP	QDELTA,3
		JZ	QUICK3
		;
		;NOW XCHG MIDDLE WITH RIGHT-1
		;
		CALL	DEC_RIGHT_ESDI

		CALL	QEXCHANGE2


		MOV	EAX,QRIGHT_NUMBER		;SAVE RIGHTY
		;
		;DEFINE TEST SYMBOL FROM RIGHT END (ORIGINALLY FROM MIDDLE)
		;
		MOV	EBX,[EDI]._CVL_OFFSET		;GET RIGHT SYMBOL VALUE

		PUSH	EAX
		CALL	DEC_RIGHT_ESDI

		MOV	ESI,QLEFT_PTR
		CALL	INC_LEFT_DSSI
		;
		;SCAN FROM LEFT UNTIL SOMETHING FOUND >= CX:BX
		;
		MOV	EDX,QLEFT_NUMBER
QNL_AGAIN:
QNL_LOOP:
		;
		;COMPARE LEFT SYMBOL
		;
		MOV	EAX,[ESI]._CVL_OFFSET
		ADD	ESI,SIZE CVL_STRUCT

		CMP	EAX,EBX
		JNC	QNL_TRY_RIGHT
QNL_NEXT:
		LEA	EAX,[EDX+1]
		INC	EDX

		TEST	EAX,PAGE_SIZE/SIZE CVL_STRUCT - 1
		JNZ	QNL_LOOP

		MOV	ESI,EDX
		CALL	Q_SET_LEFT_BLOCK

		JMP	QNL_LOOP

QNL_TRY_RIGHT:
		;
		;OOPS, CHANGE AND SCAN FROM OTHER DIRECTION
		;
		SUB	ESI,SIZE CVL_STRUCT
		MOV	QLEFT_NUMBER,EDX

		MOV	QLEFT_PTR,ESI
		MOV	EDX,QRIGHT_NUMBER
		;
		;SCAN FROM RIGHT UNTIL SOMETHING FOUND <= CX:BX
		;
QNR_LOOP:
		MOV	EAX,[EDI]._CVL_OFFSET
		SUB	EDI,SIZE CVL_STRUCT

		CMP	EAX,EBX
		JBE	QNR_SWAP
QNR_NEXT:
		MOV	EAX,EDX
		DEC	EDX

		TEST	EAX,PAGE_SIZE/SIZE CVL_STRUCT-1
		JNZ	QNR_LOOP

		MOV	EDI,EDX
		CALL	Q_SET_RIGHT_BLOCK

		JMP	QNR_LOOP

QNR_SWAP:
		;
		;SWAP INDEXES IN TABLE PLEASE
		;
		ADD	EDI,SIZE CVL_STRUCT
		MOV	EAX,QLEFT_NUMBER

		MOV	QRIGHT_NUMBER,EDX
		CMP	EAX,EDX

		JNC	QN_DONE1		;SAME, CANNOT SWAP

		CALL	QEXCHANGE2
		;
		;MOVE BOTH POINTERS
		;
		CALL	DEC_RIGHT_ESDI

		CALL	INC_LEFT_DSSI

		MOV	EDX,QLEFT_NUMBER
		MOV	EAX,QRIGHT_NUMBER

		CMP	EAX,EDX
		JAE	QNL_AGAIN		;JUMP IF ANY LEFT
QN_DONE1:
		;
		;SWAP R+1 WITH ORIGINAL PTR...
		;
		POP	EDI		; THIS BECOMES i

		PUSH	EDI
		CALL	Q_SET_RIGHT_BLOCK

		CALL	QEXCHANGE2		;SWAP THEM...
		;
		;DETERMINE WHICH HALF WILL BE PROCESSED...(WE WANT TO DO SMALLER HALF)
		;
		POP	ECX		;ORIGINAL RIGHTY - WHERE MIDDLE WAS STORED
		POP	EDX		;ORIGINAL LEFT

		INC	ECX
		MOV	EAX,QRIGHT_NUMBER

		MOV	EBX,ECX
		MOV	EDI,QLEFT_NUMBER

		SUB	EAX,EDX
		SUB	EBX,EDI

		CMP	EAX,EBX
		JC	QN_DONE2
		;
		;LETS SAY DO SECOND HALF FIRST
		;
		MOV	EAX,QRIGHT_NUMBER
		PUSH	EDX		;SAVE ORIGINAL LEFT NUMBER

		PUSH	EAX		;RIGHT TO USE THERE
		LEA	ESI,[EDI+1]

		MOV	EDI,ECX
		CALL	QUICK_LIN

		POPM	EDI,ESI

		MOV	ECX,EDI
		JMP	QUICK_LIN_1

QN_DONE2:
		;
		;LETS SAY DO FIRST HALF FIRST
		;
		INC	EDI
		MOV	ESI,EDX

		PUSH	EDI
		PUSH	ECX

		MOV	EDI,QRIGHT_NUMBER
		CALL	QUICK_LIN

		POPM	EDI,ESI

		MOV	ECX,EDI
		JMP	QUICK_LIN_1

QUICK_LINNUM_1	ENDP


QSORT_INIT	PROC	NEAR	PRIVATE
		;
		;DO SETUP FOR A QUICKSORT
		;
		;HOW MANY LINNUMS ARE THERE?
		;
		MOV	EAX,HIGH_WATER

		SHR	EAX,CVL_BITS

		MOV	QN_COUNT,EAX
		MOV	EDI,EAX

		CMP	EAX,64K
		JAE	L8$
		;
		;EAX IS # OF SYMBOLS
		;
		XOR	EAX,EAX
		XOR	ESI,ESI			;SI (LEFT) LINNUM IS 0

		MOV	QLEFT_BASE,EAX
		MOV	QMIDDLE_BASE,EAX

		MOV	QRIGHT_BASE,EAX
		DEC	EAX

		DEC	EDI			;DI (RIGHT) SYMBOL IS LAST_SYMBOL
		MOV	QLEFT_LOG,EAX

		MOV	QRIGHT_LOG,EAX

		RET

L8$:
		MOV	AL,CV_TOO_MANY_ERR
		push	EAX
		call	_err_abort

QSORT_INIT	ENDP


Q_SET_LEFT_BLOCK	PROC	NEAR	PRIVATE
		;
		;ESI IS LINNUM #
		;SET ESI TO BE LEFT POINTER...
		;
		MOV	EAX,ESI

		SHR	EAX,PAGE_BITS-CVL_BITS
		AND	ESI,PAGE_SIZE/SIZE CVL_STRUCT-1

		CMP	QLEFT_LOG,EAX
		JNZ	L5$

		SHL	ESI,CVL_BITS
		MOV	EAX,QLEFT_BASE

		ADD	ESI,EAX

		RET

L5$:
		MOV	QLEFT_LOG,EAX
		MOV	EAX,EXETABLE[EAX*4]		;LOGICAL BLOCK ADDRESS

		SHL	ESI,CVL_BITS
		MOV	QLEFT_BASE,EAX

		ADD	ESI,EAX

		RET


Q_SET_LEFT_BLOCK	ENDP


Q_SET_RIGHT_BLOCK	PROC	NEAR	PRIVATE
		;
		;SET EDI TO BE RIGHT POINTER...
		;

		MOV	EAX,EDI

		SHR	EAX,PAGE_BITS-CVL_BITS
		AND	EDI,PAGE_SIZE/SIZE CVL_STRUCT-1

		CMP	QRIGHT_LOG,EAX	;BLOCK CHANGING?
		JNZ	L5$

		SHL	EDI,CVL_BITS
		MOV	EAX,QRIGHT_BASE

		ADD	EDI,EAX

		RET

L5$:

		MOV	QRIGHT_LOG,EAX
		MOV	EAX,EXETABLE[EAX*4];LOGICAL BLOCK ADDRESS

		SHL	EDI,CVL_BITS
		MOV	QRIGHT_BASE,EAX

		ADD	EDI,EAX

		RET

Q_SET_RIGHT_BLOCK	ENDP


Q_SET_MIDDLE_BLOCK	PROC	NEAR	PRIVATE
		;
		;SET EDI TO BE RIGHT POINTER...
		;
		MOV	EAX,ECX
		MOV	EDI,ECX

		SHR	EAX,PAGE_BITS - CVL_BITS
		AND	EDI,PAGE_SIZE/SIZE CVL_STRUCT - 1

		SHL	EDI,CVL_BITS
		MOV	EAX,EXETABLE[EAX*4]	;LOGICAL BLOCK ADDRESS

		MOV	QMIDDLE_BASE,EAX
		ADD	EDI,EAX

		RET

Q_SET_MIDDLE_BLOCK	ENDP


QL_SORT2	PROC	NEAR	PRIVATE
		;
		;SORT [DS:SI] VS [ES:DI]
		;
		MOV	EAX,[ESI]._CVL_OFFSET
		MOV	ECX,[EDI]._CVL_OFFSET

		CMP	EAX,ECX
		JA	L2$

		RET

QEXCHANGE2::
		MOV	EAX,[ESI]._CVL_OFFSET
		MOV	ECX,[EDI]._CVL_OFFSET
L2$:
		MOV	[EDI]._CVL_OFFSET,EAX
		MOV	[ESI]._CVL_OFFSET,ECX

		MOV	EAX,[ESI]._CVL_LINNUM
		MOV	ECX,[EDI]._CVL_LINNUM

		MOV	[EDI]._CVL_LINNUM,EAX
		MOV	[ESI]._CVL_LINNUM,ECX

		MOV	EAX,[ESI]._CVL_SRC_GINDEX
		MOV	ECX,[EDI]._CVL_SRC_GINDEX

		MOV	[EDI]._CVL_SRC_GINDEX,EAX
		MOV	[ESI]._CVL_SRC_GINDEX,ECX

		RET

QL_SORT2	ENDP


DEC_RIGHT_ESDI	PROC	NEAR	PRIVATE
		;
		;
		;
		MOV	EAX,QRIGHT_NUMBER
		SUB	EDI,SIZE CVL_STRUCT

		TEST	EAX,PAGE_SIZE/SIZE CVL_STRUCT -1
		JZ	L9$

		DEC	EAX

		MOV	QRIGHT_NUMBER,EAX

		RET

L9$:
		DEC	EAX

		MOV	QRIGHT_NUMBER,EAX
		MOV	EDI,EAX

		JMP	Q_SET_RIGHT_BLOCK

DEC_RIGHT_ESDI	ENDP


INC_LEFT_DSSI	PROC	NEAR	PRIVATE
		;
		;
		;
		MOV	EAX,QLEFT_NUMBER
		ADD	ESI,SIZE CVL_STRUCT

		INC	EAX

		MOV	QLEFT_NUMBER,EAX
		TEST	EAX,PAGE_SIZE/SIZE CVL_STRUCT-1

		JZ	L9$

		RET


L9$:
		MOV	ESI,EAX
		JMP	Q_SET_LEFT_BLOCK


INC_LEFT_DSSI	ENDP

endif

		END

