		TITLE	PEENDSEC - Copyright (c) SLR Systems 1994

		INCLUDE	MACROS
if	fg_pe
		INCLUDE	EXES
		INCLUDE	PE_STRUC
		INCLUDE	IO_STRUC
		INCLUDE	SYMBOLS

		PUBLIC	PE_OUT_END_OF_SECTION


		.DATA

		EXTERNDEF	CURN_SECTION_GINDEX:DWORD,CODEVIEW_SECTION_GINDEX:DWORD,FLAG_0C:DWORD,_ERR_COUNT:DWORD
		EXTERNDEF	CURN_OUTFILE_GINDEX:DWORD,FIRST_CODE_DATA_OBJECT_GINDEX:DWORD,N_CODE_DATA_OBJECTS:DWORD
		EXTERNDEF	CURN_C_TIME:DWORD,PE_BASE:DWORD,PE_OBJECT_ALIGN:DWORD,ALIGNMENT:DWORD,PE_NEXT_OBJECT_RVA:DWORD
		EXTERNDEF	STACK_SIZE:DWORD,HEAP_SIZE:DWORD,FIRST_SECTION_GINDEX:DWORD,FIRST_PEOBJECT_GINDEX:DWORD
		EXTERNDEF	_tls_used_GINDEX:DWORD

		EXTERNDEF	PEXEHEADER:PEXE,OUTFILE_GARRAY:STD_PTR_S,PE_OBJECT_GARRAY:STD_PTR_S

		EXTERNDEF	OUT_FLUSH_SEGMENT:DWORD


		.CODE	PASS2_TEXT

		EXTERNDEF	ERR_ABORT:PROC,EO_CV:PROC,MOVE_EAX_TO_EDX_NEXE:PROC,FLUSH_OUTFILE_CLOSE:PROC,PE_OUTPUT_RELOCS:PROC
		EXTERNDEF	CHANGE_PE_OBJECT:PROC,EO_CV_ROUTINE:PROC,PE_OUTPUT_RESOURCES:PROC,PE_FINISH_DEBUG:PROC


PE_OUT_END_OF_SECTION	PROC
		;
		;END OF A SECTION
		;
		MOV	EAX,CURN_SECTION_GINDEX
		MOV	ECX,FIRST_SECTION_GINDEX

		CMP	EAX,ECX
		JZ	L1$

		CMP	CODEVIEW_SECTION_GINDEX,EAX
		JZ	PE_EO_CV

		MOV	AL,0
		JMP	ERR_ABORT

L1$:
		;
		;OUTPUT OTHER SECTIONS, LIKE
		;	.tls		THREAD-LOCAL-STORAGE
		;	.rsrc		RESOURCES
		;	.reloc		RELOCS
		;	.debug		DEBUG SIMPLE HEADER
		;
		CALL	PE_OUTPUT_RESOURCES

		CALL	PE_OUTPUT_RELOCS	;

		MOV	EAX,CURN_OUTFILE_GINDEX
		CONVERT	EAX,EAX,OUTFILE_GARRAY
		ASSUME	EAX:PTR OUTFILE_STRUCT

		DEC	[EAX]._OF_SECTIONS	;# OF SECTIONS USING THIS FILE
		JNZ	L25$
		;
		;LAST SECTION TO USE THIS FILE, FLUSH AND CLOSE
		;
		CALL	FLUSH_OUTFILE_CLOSE

		XOR	EAX,EAX

		MOV	CURN_OUTFILE_GINDEX,EAX
L25$:
		RET

PE_OUT_END_OF_SECTION	ENDP


PE_EO_CV	PROC
		;
		;
		;
		;
		;DEAL WITH _tls_used
		;
		CALL	HANDLE_TLS
		;
		CALL	EO_CV_ROUTINE

		CALL	PE_FINISH_DEBUG		;OUTPUT DEBUG HEADER

		CALL	CHANGE_PE_OBJECT
		;
		;NOW DO PEXEHEADER
		;
		MOV	EAX,CURN_C_TIME
		GETT	CL,PE_SUBSYSTEM_DEFINED

		MOV	PEXEHEADER._PEXE_TIME_DATE,EAX
		OR	CL,CL

		JNZ	L1$

		MOV	PEXEHEADER._PEXE_SUBSYSTEM,PSUB_WIN_CHAR	;LINK386 COMPATIBILITY
L1$:
		BITT	PE_SUBSYS_VERSION_DEFINED
		JNZ	L15$

		MOV	AL,BPTR PEXEHEADER._PEXE_SUBSYSTEM
		MOV	EDX,3

		CMP	AL,PSUB_WIN_GUI
		MOV	ECX,10

		JZ	L14$

		CMP	AL,PSUB_WIN_CHAR
		JZ	L14$

		MOV	CL,0
		CMP	AL,PSUB_NATIVE

		MOV	DL,1
		JZ	L14$

		MOV	DL,0
		CMP	AL,PSUB_UNKNOWN

		JZ	L14$

		CMP	AL,PSUB_OS2
		JZ	L14$

		MOV	CX,90
		CMP	AL,PSUB_POSIX

		MOV	DX,19
		JNZ	L15$
L14$:
		MOV	PEXEHEADER._PEXE_SUBSYS_MAJOR,DX
		MOV	PEXEHEADER._PEXE_SUBSYS_MINOR,CX
L15$:
		MOV	EAX,818FH
		GETT	CL,PE_BASE_FIXED

		OR	CL,CL
		JNZ	L20$

		AND	AL,0FEH		;MARK NOT FIXED
L20$:
		MOV	ECX,FLAG_0C

		AND	ECX,MASK APPTYPE
		JZ	L22$

		OR	AH,20H		;MARK IT A DLL
L22$:
		GETT	CL,HANDLE_EXE_ERRORFLAG
		XOR	EBX,EBX

		OR	CL,CL
		JZ	L24$

		SUB	EBX,_ERR_COUNT
		JZ	L24$

		AND	EAX,NOT 2
L24$:
		MOV	PEXEHEADER._PEXE_FLAGS,AX
		;
		;SCAN OBJECTS LOOKING FOR FIRST CODE, FIRST DATA, AND FIRST UNINIT_DATA
		;
		MOV	EAX,MASK PEL_CODE_OBJECT
		CALL	SCAN_FOR_OBJECT		;RETURNS RVA IN DX:AX, LENGTH IN CX:BX

		MOV	PEXEHEADER._PEXE_CODE_SIZE,ECX
		MOV	PEXEHEADER._PEXE_CODE_BASE,EAX

		MOV	EAX,MASK PEL_INIT_DATA_OBJECT
		CALL	SCAN_FOR_OBJECT		;RETURNS RVA IN DX:AX, LENGTH IN CX:BX

		MOV	PEXEHEADER._PEXE_IDATA_SIZE,ECX
		MOV	PEXEHEADER._PEXE_DATA_BASE,EAX

		MOV	EAX,MASK PEL_UNINIT_DATA_OBJECT
		CALL	SCAN_FOR_OBJECT		;RETURNS RVA IN DX:AX, LENGTH IN CX:BX

		MOV	PEXEHEADER._PEXE_UDATA_SIZE,ECX
		MOV	EAX,PE_BASE

		MOV	PEXEHEADER._PEXE_IMAGE_BASE,EAX
		MOV	EAX,PE_OBJECT_ALIGN

		MOV	PEXEHEADER._PEXE_OBJECT_ALIGN,EAX
		MOV	EAX,ALIGNMENT

		MOV	PEXEHEADER._PEXE_FILE_ALIGN,EAX
		MOV	EAX,PE_NEXT_OBJECT_RVA

		MOV	PEXEHEADER._PEXE_IMAGE_SIZE,EAX
		MOV	EAX,FLAG_0C

		AND	EAX,MASK APPTYPE
		JNZ	L5$
		;
		;SET COMMIT MINIMUM OF 4K
		;
		MOV	EAX,PEXEHEADER._PEXE_STACK_COMMIT
		MOV	ECX,4K

		CMP	EAX,ECX
		JA	L41$

		MOV	PEXEHEADER._PEXE_STACK_COMMIT,ECX	;STACK COMMIT MINIMUM IS 8K
L41$:
		MOV	ECX,100000H		;DEFAULT IS 1 MEG
		GETT	AL,STACK_SIZE_FLAG

		OR	AL,AL
		JZ	L42$

		MOV	ECX,64K			;IF USER SET IT, MAKE IT AT LEAST 64K
L42$:
		CMP	STACK_SIZE,64K
		JA	L43$

		MOV	STACK_SIZE,ECX		;STACK SIZE MINIMUM IS 64K
L43$:
		JMP	L51$

L5$:
		XOR	EAX,EAX

		MOV	STACK_SIZE,EAX
		MOV	PEXEHEADER._PEXE_STACK_COMMIT,EAX
L51$:

		MOV	ECX,STACK_SIZE
		MOV	EAX,HEAP_SIZE

		MOV	PEXEHEADER._PEXE_STACK_RESERVE,ECX

		TEST	EAX,EAX
		JZ	L52$

		CMP	EAX,64K
		JAE	L53$

		MOV	EAX,64K
		JMP	L53$

L52$:
		MOV	EAX,100000H		;DEFAULT IS 1 MEG IF NOT SET
L53$:
		MOV	PEXEHEADER._PEXE_HEAP_RESERVE,EAX
		MOV	EAX,PEXEHEADER._PEXE_HEAP_COMMIT

		TEST	EAX,EAX
		JNZ	L54$

		MOV	PEXEHEADER._PEXE_HEAP_COMMIT,4K
L54$:
		MOV	EAX,OFF PEXEHEADER
		MOV	ECX,SIZE PEXE

		XOR	EDX,EDX
		CALL	MOVE_EAX_TO_EDX_NEXE
		;
		;NOW SEND OBJECT TABLE TOO...
		;
		CALL	FLUSH_OBJECTS

		MOV	EAX,CURN_OUTFILE_GINDEX
		CONVERT	EAX,EAX,OUTFILE_GARRAY
		ASSUME	EAX:PTR OUTFILE_STRUCT

		DEC	[EAX]._OF_SECTIONS	;# OF SECTIONS USING THIS FILE
		JNZ	L25$
		;
		;LAST SECTION TO USE THIS FILE, FLUSH AND CLOSE
		;
		CALL	FLUSH_OUTFILE_CLOSE

		XOR	EAX,EAX
		MOV	CURN_OUTFILE_GINDEX,EAX
L25$:
		RET

PE_EO_CV	ENDP


FLUSH_OBJECTS	PROC	NEAR
		;
		;
		;
		PUSHM	ESI,EBX

		MOV	EBX,SIZE PEXE
		MOV	ESI,FIRST_PEOBJECT_GINDEX

		TEST	ESI,ESI
		JZ	L5$
L1$:
		CONVERT	ESI,ESI,PE_OBJECT_GARRAY
		ASSUME	ESI:PTR PE_IOBJECT_STRUCT

		MOV	ECX,SIZE PE_OBJECT_STRUCT
		MOV	EAX,ESI

		MOV	EDX,EBX
		CALL	MOVE_EAX_TO_EDX_NEXE

		MOV	ESI,[ESI]._PEOBJECT_NEXT_GINDEX
		ADD	EBX,SIZE PE_OBJECT_STRUCT

		TEST	ESI,ESI
		JNZ	L1$
L5$:
		;
		;ALIGN STUFF UP TO
		;
		POPM	EBX,ESI

		RET

FLUSH_OBJECTS	ENDP


SCAN_FOR_OBJECT	PROC	NEAR
		;
		;AX IS MASK TO SEARCH FOR
		;
		;IF FOUND, CX:BX == SIZE
		;	   DX:AX == RVA
		;
		MOV	ECX,N_CODE_DATA_OBJECTS
		MOV	EDX,FIRST_CODE_DATA_OBJECT_GINDEX

		TEST	ECX,ECX
		JZ	L5$
L1$:
		CONVERT	EDX,EDX,PE_OBJECT_GARRAY
		ASSUME	EDX:PTR PE_IOBJECT_STRUCT

		TEST	[EDX]._PEOBJECT_FLAGS,EAX
		JNZ	L8$

		MOV	EDX,[EDX]._PEOBJECT_NEXT_GINDEX

		DEC	ECX
		JNZ	L1$
L5$:
		XOR	EAX,EAX
		XOR	ECX,ECX

		RET

L8$:
		MOV	ECX,[EDX]._PEOBJECT_VSIZE
		MOV	EAX,[EDX]._PEOBJECT_RVA

		RET

SCAN_FOR_OBJECT	ENDP


HANDLE_TLS	PROC
		;
		;
		;
		MOV	ECX,_tls_used_GINDEX

		TEST	ECX,ECX
		JZ	L9$

		CONVERT	ECX,ECX,SYMBOL_GARRAY
		ASSUME	ECX:PTR SYMBOL_STRUCT

		MOV	AL,[ECX]._S_NSYM_TYPE
		MOV	EDX,[ECX]._S_OFFSET

		CMP	AL,NSYM_RELOC
		JNZ	L9$

		SUB	EDX,PE_BASE

		MOV	PEXEHEADER._PEXE_THREAD_LOCAL_SIZE,18H

		MOV	PEXEHEADER._PEXE_THREAD_LOCAL_RVA,EDX
L9$:
		RET

HANDLE_TLS	ENDP

endif

		END

