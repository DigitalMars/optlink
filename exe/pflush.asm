		TITLE	PFLUSH - Copyright (c) SLR Systems 1994

		INCLUDE	MACROS
		INCLUDE	SEGMENTS
		INCLUDE	SEGMSYMS
if	fg_prot

		PUBLIC	PROT_FLUSH_SEGMOD,PROT_FLUSH_SEGMENT,OS2_OUT_FLUSH_EXE,DO_OS2_PAGE_ALIGN,DO_OS2_ALIGN,DWORD_ALIGN_FINAL
		PUBLIC	DO_OS2_PAGE_ALIGN_NO_16


		.DATA

		EXTERNDEF	FIX2_SEG_COMBINE:BYTE,FIX2_SM_FLAGS:BYTE

		EXTERNDEF	SEG_PAGE_SIZE_M1:DWORD,SEG_PAGE_SHIFT:DWORD,LAST_SEG_OS2_NUMBER:DWORD,FLAG_0C:DWORD
		EXTERNDEF	ICODE_SEGMOD_GINDEX:DWORD,FIX2_NEW_SEGMOD_GINDEX:DWORD,IDATA_SEGMOD_GINDEX:DWORD,HIGH_WATER:DWORD
		EXTERNDEF	OLD_HIGH_WATER:DWORD,FINAL_HIGH_WATER:DWORD,EXEPACK_SEGMENT_START:DWORD,NEW_REPT_ADDR:DWORD
		EXTERNDEF	TARG_ADDR:DWORD,FIX2_SM_START:DWORD,FIX2_SM_LEN:DWORD,FIX2_SEG_LEN:DWORD,FIX2_SEG_OFFSET:DWORD
		EXTERNDEF	ZERO_BYTES:DWORD

		EXTERNDEF	EXEPACK_STRUCTURE:SEQ_STRUCT,SEGMENT_TABLE:SEGTBL_STRUCT

		EXTERNDEF	EXEPACK_ROUTINE:DWORD


		.CODE	PASS2_TEXT

		EXTERNDEF	ZERO_EAX_FINAL_HIGH_WATER:PROC,FLUSH_EAX_TO_FINAL:PROC,FLUSH_ERROR_LINENUMBERS:PROC
		EXTERNDEF	PACK_ZERO:PROC,OS2PACK_INIT:PROC,ERR_ABORT:PROC,FIXDS_ROUTINE:PROC
		EXTERNDEF	OS2_EXEPACK_FLUSH:PROC,OS2_EXEPACK:PROC,OS2_RELOC_FLUSH:PROC,PROT_DATA_TO_WINPACK:PROC
		EXTERNDEF	ZERO_EAX_EXEPACK_HIGH_WATER:PROC,DO_WINPACK:PROC,COMPRESS:PROC,WINPACK_RELOC_FLUSH:PROC
		EXTERNDEF	CHANGE_PE_OBJECT:PROC,CREATE_ICODE_DATA:PROC,CREATE_IDATA_DATA:PROC

		EXTERNDEF	ALIGN_SMALL_ERR:ABS


PROT_FLUSH_SEGMOD	PROC
		;
		;FLUSH INFO ABOUT THIS SEGMOD - THIS IS CALLED AT END OF EACH SEGMOD - OR AT END OF SEGMENT IF COMMON OR STACK
		;
		;FIRST SEE IF WE USE HIGH-WATER MARK OR SEGMENT SIZE
		;
		GETT	AL,DOING_DEBUG		;CODEVIEW SEGS ALWAYS WRITE FULL SIZE
		GETT	CL,RC_PRELOADS		;SO DO PRELOADS

		OR	AL,AL
		JNZ	L0$

		OR	CL,CL
		JZ	L1$
		;
		;DO THIS UNLESS THIS IS STACK SEGMENT...
		;
		MOV	AL,FIX2_SEG_COMBINE

		CMP	AL,SC_STACK
		JZ	L1$
L0$:
		;
		;USE MAX SIZE
		;IF COMMON, USE SEGMENT LENGTH...
		;
		MOV	EAX,FIX2_SM_LEN
		MOV	CL,FIX2_SEG_COMBINE

		CMP	CL,SC_COMMON
		JNZ	L05$

		MOV	EAX,FIX2_SEG_OFFSET
		MOV	ECX,FIX2_SEG_LEN

		ADD	EAX,ECX
L05$:
		MOV	HIGH_WATER,EAX		;SET NEW HIGH-WATER MARK
L1$:
		MOV	EDX,HIGH_WATER
		MOV	EAX,FIX2_SM_START

		SUB	EDX,EAX
		JC	L84$
		;
		;EDX IS # OF BYTES TO WRITE
		;
		GETT	AL,DOING_DEBUG		;FOR DEBUG, WRITE AS IS
		GETT	CL,RC_PRELOADS

		OR	AL,AL
		JNZ	L8$

		OR	CL,CL
		JNZ	L14$

		OR	EDX,EDX
		JZ	L84$	 		;SKIP, IT'S AN EMPTY SEGMENT

		CALL	PACK_ZERO		;TRY TRIMMING OFF TRAILING 0'S

		JZ	L84$			;SKIP, IT'S ALL ZEROS

		JMP	L15$

L14$:
		MOV	EDX,HIGH_WATER		;PRELOAD NON-DGROUP SEGMENTS USE ENTIRE SIZE
		MOV	EAX,FIX2_SM_START	;IF NOT ZERO LENGTH

		SUB	EDX,EAX
L15$:
		MOV	ECX,FIX2_SM_START
		GETT	AH,FIXDS_FLAG

		ADD	ECX,EDX
		MOV	AL,FIX2_SM_FLAGS

		MOV	HIGH_WATER,ECX
		TEST	AL,MASK SEG_CLASS_IS_CODE

		MOV	ECX,FLAG_0C
		JZ	L2$

		OR	AH,AH
		JZ	L2$

		AND	ECX,MASK APPTYPE	;LIBRARY?
		JNZ	L2$			;DON'T FIXDS A .DLL

		MOV	EAX,EDX
		CALL	FIXDS_ROUTINE
L2$:
		GETT	AL,OS2_PAGE_ALIGN_DONE

		OR	AL,AL
		JNZ	L3$

		CALL	DO_OS2_PAGE_ALIGN
		JMP	L3$

L84$:
		GETT	AL,RC_PRELOADS
		MOV	CL,FIX2_SEG_COMBINE

		OR	AL,AL
		JZ	L845$

		CMP	CL,SC_STACK
		JNZ	L845$

		MOV	EAX,FIX2_SM_START
		MOV	ECX,OLD_HIGH_WATER

		SUB	EAX,ECX

		PUSH	EAX
		CALL	ZERO_EAX_FINAL_HIGH_WATER

		MOV	EAX,LAST_SEG_OS2_NUMBER
		CONV_EAX_SEGTBL_ECX

		POP	EDX

		ADD	[ECX]._SEGTBL_PSIZE,EDX	;I DON'T REMEMBER
L845$:
		GETT	AL,EXEPACK_BODY

		OR	AL,AL
		JZ	L85$

		RET


L3$:
		;
		;ARE WE EXEPACKING OR JUST WRITING OUT?
		;
		GETT	AL,EXEPACK_BODY

		OR	AL,AL
		JNZ	L6$
		;
		;MAY NEED TO FILL IN ZEROS UP TO HERE...
		;
		MOV	EAX,FIX2_SM_START
		MOV	ECX,OLD_HIGH_WATER

		SUB	EAX,ECX
if	fg_winpack
		GETT	CL,SEGPACK_SEGMENT

		OR	CL,CL
		JNZ	L65$
endif
		CALL	ZERO_EAX_FINAL_HIGH_WATER
L8$:
		CALL	OS2_DATA_FLUSH
		;
		;EMPTY SEGMENT
		;
L85$:
		XOR	EAX,EAX

		MOV	HIGH_WATER,EAX
L9$:
		RET

L6$:
		JMP	OS2_EXEPACK

if	fg_winpack
L65$:
		CALL	ZERO_EAX_EXEPACK_HIGH_WATER		;FOR COMPRESSED SEGMENTS, BUFFER THE ZEROS
		JMP	PROT_DATA_TO_WINPACK			;BUFFER THE DATA

endif

PROT_FLUSH_SEGMOD	ENDP


DO_OS2_PAGE_ALIGN	PROC
		;
		;FIRST WRITE ZEROS UP TO NEXT AVAILABLE PAGE
		;
		;RETURNS CX = PAGE #
		;
		GETT	AL,RC_PRELOADS

		OR	AL,AL
		JZ	L1$

		MOV	EAX,16				;NEED AT LEAST 16 ZEROS
		CALL	ZERO_EAX_FINAL_HIGH_WATER

DO_OS2_PAGE_ALIGN_NO_16	LABEL	PROC

		MOV	EAX,SEG_PAGE_SIZE_M1

		MOV	SEG_PAGE_SIZE_M1,31

		PUSH	EAX
		CALL	DO_OS2_ALIGN			;THEN ALIGN TO MAX OF 32

		POP	EAX

		MOV	SEG_PAGE_SIZE_M1,EAX
L1$:
		CALL	DO_OS2_ALIGN			;OR SELECTED ALIGNMENT

		MOV	EAX,FINAL_HIGH_WATER
		MOV	DL,-1

		MOV	EXEPACK_SEGMENT_START,EAX
		MOV	ECX,SEG_PAGE_SHIFT

		SHR	EAX,CL
		SETT	OS2_PAGE_ALIGN_DONE,DL

if	fg_segm
		CMP	EAX,64K
		JAE	L9$
endif
		;
		;STORE THIS IN SEGMENT TABLE...
		;
		MOV	EDX,EAX
		MOV	EAX,LAST_SEG_OS2_NUMBER

		CONV_EAX_SEGTBL_ECX

		MOV	[ECX]._SEGTBL_FADDR,EDX
		XOR	EAX,EAX

		MOV	NEW_REPT_ADDR,EAX
		ADD	EAX,4

		MOV	TARG_ADDR,EAX
		RESS	EXEPACK_FLUSHING,AH

		MOV	EXEPACK_ROUTINE,OFF OS2PACK_INIT

		RESS	EXEPACK_USING_NEW,AH
		MOV	EAX,EDX

		RET

L9$:
		MOV	AL,ALIGN_SMALL_ERR
		CALL	ERR_ABORT

DO_OS2_PAGE_ALIGN	ENDP


DO_OS2_ALIGN	PROC
		;
		;
		;
		XOR	EAX,EAX
		MOV	ECX,FINAL_HIGH_WATER

		SUB	EAX,ECX
		MOV	ECX,SEG_PAGE_SIZE_M1

		AND	EAX,ECX		;SEGMENT ALIGN SIZE -1
		JNZ	ZERO_EAX_FINAL_HIGH_WATER

		RET

DO_OS2_ALIGN	ENDP


DWORD_ALIGN_FINAL	PROC
		;
		;
		;
		MOV	EAX,FINAL_HIGH_WATER

		NEG	EAX

		AND	EAX,3
		JNZ	ZERO_EAX_FINAL_HIGH_WATER

		RET

DWORD_ALIGN_FINAL	ENDP


PROT_FLUSH_SEGMENT	PROC
		;
		;SEGMENT # HAS CHANGED.  FLUSH INFO ABOUT LAST SEGMENT.
		;
		;FIRST SEE IF ANY DATA APPEARED IN THE SEGMENT
		;
		;
		;ARE WE EXEPACKING OR JUST WRITING OUT?
		;
if	fg_winpack
		GETT	AL,SEGPACK_SEGMENT
endif
		MOV	ECX,LAST_SEG_OS2_NUMBER

if	fg_winpack
		OR	AL,AL
		JNZ	L5$
endif
		TEST	ECX,ECX
		JZ	L8$

		GETT	AL,EXEPACK_BODY
		MOV	ECX,HIGH_WATER

		OR	AL,AL
		JZ	L6$

		OR	ECX,ECX
		JZ	L6$

		CALL	OS2_EXEPACK_FLUSH
L6$:
if	fg_segm
		CALL	OS2_RELOC_FLUSH
if	fg_winpack
		GETT	AL,WINPACK_SELECTED

		OR	AL,AL
		JZ	L8$

		SETT	SEGPACK_SEGMENT,AL		;APPLOADER IS OUT, COMPRESS THE REST...
endif
L8$:
		RET
else
		RET
endif

if	fg_winpack
L5$:
		MOV	EAX,EXEPACK_STRUCTURE._SEQ_PTR

		MOV	NEW_REPT_ADDR,EAX
		OR	EAX,EAX

		JZ	L6$
		CALL	COMPRESS		;DEALS WITH SETTING ITERATED BIT IF SUCCESSFUL...
		JMP	WINPACK_RELOC_FLUSH

endif

PROT_FLUSH_SEGMENT	ENDP

endif


if	fg_prot

OS2_DATA_FLUSH	PROC
		;
		;
		;
		MOV	EDX,HIGH_WATER		;# OF BYTES TO WRITE
		MOV	EAX,LAST_SEG_OS2_NUMBER


		MOV	OLD_HIGH_WATER,EDX

		CONV_EAX_SEGTBL_ECX

		MOV	EAX,FIX2_SM_START
		MOV	[ECX]._SEGTBL_PSIZE,EDX

		SUB	EDX,EAX
		JZ	L5$

		MOV	EAX,EDX
		CALL	FLUSH_EAX_TO_FINAL
L5$:
		XOR	EAX,EAX

		MOV	HIGH_WATER,EAX

		RET

OS2_DATA_FLUSH	ENDP

endif


if	fg_segm

OS2_OUT_FLUSH_EXE	PROC
		;
		;IF NOT ALREADY DONE, FLUSH REMAINING DATA TO EXE AND DO
		;HEADER STUFF
		;
		GETT	AL,OUT_FLUSHED
		MOV	CL,-1

		OR	AL,AL
		JZ	L1$

		RET

L1$:
		SETT	OUT_FLUSHED,CL

		RET

OS2_OUT_FLUSH_EXE	ENDP

endif

if	fg_pe

		PUBLIC	PE_FLUSH_SEGMENT,PE_FLUSH_SEGMOD

PE_FLUSH_SEGMENT	PROC
		;
		;OBJECT # HAS CHANGED.  FLUSH INFO ABOUT LAST OBJECT.
		;
		JMP	CHANGE_PE_OBJECT

PE_FLUSH_SEGMENT	ENDP


PE_FLUSH_SEGMOD	PROC
		;
		;FLUSH INFO ABOUT THIS SEGMOD - THIS IS CALLED AT END OF EACH SEGMOD - OR AT END OF SEGMENT IF COMMON OR STACK
		;
		;FIRST SEE IF WE USE HIGH-WATER MARK OR SEGMENT SIZE
		;
		GETT	AL,DOING_DEBUG		;CODEVIEW SEGS ALWAYS WRITE FULL SIZE
		MOV	CL,FIX2_SEG_COMBINE

		OR	AL,AL
		JNZ	L0$

		CMP	CL,SC_COMMON
		JZ	L0$

		CMP	CL,SC_STACK
		JZ	L0$

		MOV	ECX,FIX2_NEW_SEGMOD_GINDEX

		MOV	EAX,ICODE_SEGMOD_GINDEX
		MOV	EDX,IDATA_SEGMOD_GINDEX

		CMP	ECX,EAX
		JZ	L01$	;1$

		CMP	ECX,EDX
		JNZ	L0$

		CALL	CREATE_IDATA_DATA
		JMP	L0$

L01$:
		CALL	CREATE_ICODE_DATA
L0$:
		;
		;USE MAX SIZE
		;IF COMMON, USE SEGMENT LENGTH...
		;
		MOV	EAX,FIX2_SM_LEN
		MOV	CL,FIX2_SEG_COMBINE

		CMP	CL,SC_COMMON
		JNZ	L05$

		MOV	EAX,FIX2_SEG_OFFSET
		MOV	ECX,FIX2_SEG_LEN

		ADD	EAX,ECX
L05$:
		MOV	HIGH_WATER,EAX		;SET NEW HIGH-WATER MARK
L1$:
		MOV	EDX,HIGH_WATER
		MOV	EAX,FIX2_SM_START

		SUB	EDX,EAX
		JC	L85$
		;
		;EDX IS # OF BYTES TO WRITE
		;
		GETT	AL,DOING_DEBUG		;FOR DEBUG, WRITE AS IS
		MOV	CL,FIX2_SM_FLAGS

		OR	AL,AL
		JNZ	L8$

		OR	EDX,EDX
		JZ	L85$	 		;SKIP, IT'S AN EMPTY SEGMENT

		AND	CL,MASK SEG_CLASS_IS_CODE
		JNZ	L2$

		CALL	PACK_ZERO		;TRY TRIMMING OFF TRAILING 0'S
L2$:
		MOV	EAX,FIX2_SM_START
		JZ	L85$			;SKIP, IT'S ALL ZEROS

		ADD	EAX,EDX

		MOV	HIGH_WATER,EAX
L3$:
		;
		;MAY NEED TO FILL IN ZEROS UP TO HERE...
		;
		MOV	EAX,FIX2_SM_START
		MOV	ECX,OLD_HIGH_WATER

		SUB	EAX,ECX
		JZ	L8$

		MOV	CL,FIX2_SM_FLAGS
		MOV	EDX,0CCCCCCCCH

		AND	CL,MASK SEG_CLASS_IS_CODE
		JZ	L7$

		MOV	ZERO_BYTES,EDX
L7$:
		CALL	ZERO_EAX_FINAL_HIGH_WATER

		XOR	EAX,EAX

		MOV	ZERO_BYTES,EAX
L8$:
		CALL	PE_DATA_FLUSH
		;
		;EMPTY SEGMENT
		;
L85$:
		XOR	EAX,EAX

		MOV	HIGH_WATER,EAX
L9$:
		RET

PE_FLUSH_SEGMOD	ENDP


PE_DATA_FLUSH	PROC
		;
		;
		;
		MOV	EAX,HIGH_WATER		;# OF BYTES TO WRITE
		MOV	ECX,FIX2_SM_START

		MOV	OLD_HIGH_WATER,EAX
		SUB	EAX,ECX

		JZ	L5$

		CALL	FLUSH_EAX_TO_FINAL
L5$:
		XOR	EAX,EAX

		MOV	HIGH_WATER,EAX

		RET

PE_DATA_FLUSH	ENDP

endif

		END

