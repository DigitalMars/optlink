
#include "all.h"

void _init_install_module()
{
    // build a bit table

    void *p = _get_new_log_blk(); 	// leave in faster memory
    FSYMBEG = p;
    memset(p, 0, 8 * 1024);
    MAX_PAGEDWORD = 0;
}

/*

		PUBLIC	INSTALL_MODULE_PAGE,BINSER_MODULE


		.DATA

		EXTERNDEF	FSYMLIM:DWORD,MAX_PAGEDWORD:DWORD,LIB_MODULES:DWORD,EXETABLE:DWORD,NUMBLKS:DWORD
		EXTERNDEF	LIB_PAGE_SIZE:DWORD,LIB_MODULE_PTR_LAST_OFFSET:DWORD,LIB_MODULE_PTR_BLOCKS:DWORD,FSYMEND:DWORD
		EXTERNDEF	FSYMBEG:DWORD,LS_MODULE_PTR:DWORD


		.CODE	PASS1_TEXT

		externdef	LIBRARY_ERROR:PROC
		EXTERNDEF	GET_NEW_LOG_BLK:PROC

		EXTERNDEF	TOO_MANY_SYMBOLS_ERR:ABS


		public	_install_module_page
_install_module_page	proc
		mov	EAX,4[ESP]
_install_module_page	endp

INSTALL_MODULE_PAGE	PROC
		;
		;INSTALL MODULE[EAX] IN LIST OF MODULES...
		;
		PUSH	EBX
		MOV	ECX,EAX

		SHR	EAX,5
		MOV	EBX,FSYMBEG

		MOV	EDX,1
		AND	CL,1FH

		SHL	EDX,CL

		MOV	ECX,[EAX*4+EBX]

		OR	ECX,EDX
		MOV	EDX,MAX_PAGEDWORD

		MOV	DPTR [EAX*4+EBX],ECX
		CMP	EDX,EAX

		POP	EBX
		JB	L1$

		RET

L1$:
		MOV	MAX_PAGEDWORD,EAX
		RET

INSTALL_MODULE_PAGE	ENDP

		public	_binser_module
_binser_module	proc
		mov	EAX,4[ESP]
_binser_module	endp

BINSER_MODULE	PROC
		;
		;EAX IS PAGE #, RETURN MODULE #
		;
		PUSHM	EDI,ESI

		MUL	LIB_PAGE_SIZE		;ZEROS EDX

		MOV	ESI,LIB_MODULES
		MOV	ECX,EAX

		CMP	ESI,PAGE_SIZE/4
		JA	L3$

		PUSH	EBX
		MOV	EBX,LS_MODULE_PTR

		PUSH	EDX
		JMP	L2$

		;
		;ESI STORES OLD HIGH
		;EDI IS NEW HIGH
		;EDX IS NEW LOW
		;ECX IS VALUE SEARCHING FOR
		;EBX IS BASE
		;
L1$:
		LEA	EDX,[EDX+EDI+1]		;NEW LOW
L2$:
		MOV	EDI,ESI			;OLD HIGH

		SUB	EDI,EDX			;DISTANCE BETWEEN TWO POINTS

		SHR	EDI,1			;TAKE HALF OF THAT
		LEA	EAX,[EDX*4+EBX]

		CMP	ESI,EDX
		JZ	L8$			;SAME POINT, NOT FOUND...

		MOV	EAX,[EAX+EDI*4]

		AND	EAX,7FFFFFFFH

		CMP	EAX,ECX
		JB	L1$			;SMALLER, JUMP	=	6

		LEA	ESI,[EDX+EDI]
		JNZ	L2$			;LARGER, JUMP
		;
		;FOUND IT...
		;
		POPM	EAX,EBX

		ADD	EAX,ESI			;DELTA FOR BLOCK #

		POPM	ESI,EDI
		jc	Lerr
		RET

L3$:
		;
		;MORE THAN PAGE_SIZE/4 MODULES...  FIND CORRECT BLOCK FIRST
		;
		;EDX IS ZERO
		;
L31$:
		MOV	EAX,EXETABLE[EDX*4]		;POINTS TO MAXIMUMS
		INC	EDX

		CMP	EAX,ECX
		JAE	L5$

		CMP	EDX,16
		JB	L31$

		JMP	L81$

L5$:
		;
		;BX /4 IS CORRECT BLOCK # TO USE
		;NEED BLOCK# * PAGE_SIZE/4 ON STACK
		;NEED DS:BX AS START OF BLOCK
		;NEED DI AT END OF BLOCK (PAGE_SIZE UNLESS LAST BLOCK...)
		;
		PUSH	EBX
		MOV	EDI,LIB_MODULE_PTR_BLOCKS

		DEC	EDX
		DEC	EDI

		MOV	EBX,LS_MODULE_PTR
		CMP	EDX,EDI

		MOV	ESI,PAGE_SIZE
		JNZ	L51$

		MOV	ESI,LIB_MODULE_PTR_LAST_OFFSET
L51$:
		MOV	EBX,[EBX+EDX*4]

		SHL	EDX,PAGE_POWER-2

		SHR	ESI,2
		PUSH	EDX

		XOR	EDX,EDX
		JMP	L2$

L8$:
		;
		;WELL, MAKE DX POINT TO WHERE INSERT BELONGS
		;
		POPM	EAX,EBX
L81$:
		POPM	ESI,EDI
		STC
Lerr:
		mov	EAX,EBX
		jmp	LIBRARY_ERROR

		RET

BINSER_MODULE	ENDP
*/
