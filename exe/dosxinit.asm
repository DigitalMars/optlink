		TITLE	SEGMINIT - Copyright (c) SLR Systems 1991

		INCLUDE	MACROS
		INCLUDE	IO_STRUC
		INCLUDE	SEGMSYMS
		INCLUDE	EXES


		PUBLIC	EXE_OUT_INIT


		.DATA

	SOFT	EXTB	ZEROS_16,OLD_TEMP_RECORD

	SOFT	EXTW	NONRES_BYTES_LEFT,OBJ_DEVICE,SYMBOL_LENGTH,OLD_EXE_HDR_SIZE,CURN_FILE_LIST_GINDEX

	SOFT	EXTD	OLD_LIST,ENTRYNAME_STUFF,ENTRYNAME_BITS,EXEHEADER,OLD_LIST,STUB_LIST,EXEHDR_ADDR,OS2_EXEHDR_START

	SOFT	EXTQ	_FILE_LIST_GARRAY

	SOFT	EXTCA	OPTI_MOVE

		.CODE	PASS2_TEXT

	SOFT	EXTP	OPEN_READING,N_NONRES_TABLE,SEARCH_ENTRYNAME,CLOSE_MYI_HANDLE,RELEASE_MYI_PTR,RELEASE_IO_SEGMENT,ERR_ABORT
	SOFT	EXTP	MOVE_EXEHDR_TO_FINAL,DOT,N_RES_TABLE,OPTI_MOVE_PRESERVE_SIGNIFICANT,REPORT_CLOSE_ASCIZ

	SOFT	EXTA	BAD_STUB_ERR

		ASSUME	DS:NOTHING

EXE_OUT_INIT	PROC
		;
		;OPEN EXE OR COM FILE FOR OUTPUT
		;
		FIXDS
if	0

		BITT	STUB_SUPPLIED		;IF NO 'STUB' AND NO 'OLD'
		JNZ	DO_INIT0
		BITT	OLD_SUPPLIED
		JZ	DO_INIT1		;DON'T USE SEPARATE THREAD

DO_INIT0	LABEL	PROC
		;
		;RUN STUB & OLD PROCESSING AS SEPARATE THREAD
		;
		;WAIT FOR 4TH OPEN_THREAD TO TERMINATE
		;
		LEA	AX,OPENS_READS_TERM_SEM
		CALL	DO_DOSSEMWAIT_AX

		LEA	BX,MYI_STUFF+1*SIZE MYI_STRUCT
		LEA	AX,MYI_OPEN_TERM_SEM[BX]
		CALL	DO_DOSSEMREQUEST_AX
		;OK, NOW START IT BACK UP...
		PUSHM	CS,OFF DO_INIT2
		LEA	AX,MYI_OPEN_THREAD_ID[BX]
		PUSHM	SS,AX
		LEA	AX,MYI_OPEN_THREAD_STACK[BX]
		AND	AL,0FCH
		PUSHM	SS,AX
		CALL	DO_DOSCREATETHREAD
		CALL	DO_DOSSLEEP_TWICE
		RET

DO_INIT2	LABEL	PROC

		FIXDS
		FIXES
		LEA	BP,STACK_VARIABLES+80H
		CALL	DO_INIT1
		ASSUME	DS:NOTHING
		;NOW STOP...
		CALL	DO_DOSENTERCRITSEC
		LEA	BX,MYI_STUFF+1*SIZE MYI_STRUCT
		LEA	AX,MYI_OPEN_TERM_SEM[BX]
		CALL	DO_DOSSEMCLEAR_AX
		XOR	AX,AX
		MOV	SS:MYI_OPEN_THREAD_ID[BX],AX
		PUSHM	AX,AX
		CALL	DOSEXIT

endif

DO_INIT1	LABEL	PROC
		;
		;NEED TO OUTPUT EITHER THE STANDARD STUB OR THE SUPPLIED ONE
		;
;		CALL	DOT
		XOR	DX,DX
		MOV	EXEHDR_ADDR.LW,DX	;NEXT EXEHDR WRITE ADDRESS
		MOV	EXEHDR_ADDR.HW,DX
		BITT	STUB_SUPPLIED
		JNZ	5$
		BITT	STUB_NONE		;NO STUB AT ALL...
		JNZ	8$
		;
		;THIS IS THE EASY METHOD...
		;
59$:
		MOV	AX,SEG DFLT_STUB
		MOV	DS,AX
		LEA	SI,DFLT_STUB		;MOVE DEFAULT STUB
		MOV	CX,DFLT_STUB_LEN
		CALL	MOVE_EXEHDR_TO_FINAL
8$:
		FIXDS
		FIXES
;		CALL	DOT
		LEA	DI,OS2_EXEHDR_START	;DEFINE OS2 HEADER START ADDRESS
		LEA	SI,EXEHDR_ADDR
		MOVSW
		MOVSW

if	0

		LEA	AX,STUB_OLD_DONE_SEM	;STUB AND OLD FINISHED
		CALL	DO_DOSSEMCLEAR_AX
endif
		MOV	CURN_FILE_LIST_GINDEX,0	;FOR ERRORS...
;		RESS	STUB_PROCESSING
		RET

4$:
		JMP	9$

58$:
		FIXDS
		JMP	59$

		ASSUME	DS:DGROUP
5$:
		;
		;OK, NEED TO COPY A SPECIFIC STUB TO OUTPUT STREAM
		;
		MOV	AX,STUB_LIST.FILE_FIRST_GINDEX
		CONVERT_GINDEX_AX_DSSI	_FILE_LIST_GARRAY
		MOV	AX,[SI].FILE_LIST_NEXT_GINDEX
		MOV	CX,AX
		CONVERT_GINDEX_AX_DSSI	_FILE_LIST_GARRAY

		CALL	OPEN_READING		;GET OPEN STUB FILE...
		JC	58$			;NOT FOUND...
		;
		;NOW GET FIRST BLOCK OF DATA...
		;
		CALL	[BX].MYI_FILLBUF

;		CALL	DOT

		PUSH	[BX].MYI_COUNT
		LDS	SI,[BX].MYI_PTR
		;
		;DS:SI IS EXEHEADER...
		;
		LEA	DI,EXEHEADER		;VERIFY SIGNATURE
		CMP	_EXE_SIGN[SI],'ZM'
		JNZ	4$		;NOT VALID EXE...
		;
		;SAVE A COPY OF OLD HEADER
		;
		MOV	AX,[SI]._EXE_LEN_PAGE_512	;CONVERT SIZE INTO FLAT 32-BIT #
		MOV	BX,512
		MUL	BX
		CMP	[SI]._EXE_LEN_MOD_512,0
		JZ	51$
		SUB	AX,BX
		SBB	DX,0
		ADD	AX,[SI]._EXE_LEN_MOD_512
51$:
		MOV	ES:[DI]._EXE_LEN_MOD_512,AX	;STORE AS 32-BIT #
		MOV	ES:[DI]._EXE_LEN_PAGE_512,DX
		;
		;SUBTRACT OUT HEADER SIZE...
		;
		MOV	AX,[SI]._EXE_HDR_SIZE		;IN PARAGRAPHS
		MOV	OLD_EXE_HDR_SIZE,AX
		MOV	BX,16
		MUL	BX
		SUB	ES:[DI]._EXE_LEN_MOD_512,AX	;RESULTS IN LOAD IMAGE SIZE?
		SBB	ES:[DI]._EXE_LEN_PAGE_512,DX
		;
		;CALC NEW HEADER SIZE...
		;
		MOV	AX,[SI]._EXE_N_RELOC
		MOV	ES:[DI]._EXE_N_RELOC,AX
		ADD	AX,3
		SHRI	AX,2				;# OF PARAGRAPHS USED BY RELOCS
		ADD	AX,4				;OTHER HEADER PARAGRAPHS
		MOV	ES:[DI]._EXE_HDR_SIZE,AX	;HEADER SIZE IN PARAS
		MUL	BX				;HEADER SIZE IN BYTES
		ADD	AX,ES:[DI]._EXE_LEN_MOD_512
		ADC	DX,ES:[DI]._EXE_LEN_PAGE_512	;OVERALL SIZE IN BYTES
		MOV	CX,AX
		AND	AX,511
		MOV	ES:[DI]._EXE_LEN_MOD_512,AX	;SIZE MOD 512
		ADD	CX,511
		ADC	DX,0
		SHR	DX,1
		RCR	CX,1
		MOV	AL,CH
		MOV	AH,DL
		MOV	ES:[DI]._EXE_LEN_PAGE_512,AX	;# OF 512-BYTE PAGES
		LEA	DI,_EXE_MIN_ABOVE[DI]
		LEA	SI,_EXE_MIN_ABOVE[SI]		;PRESERVE HEADER UP TO RELOC_OFF
		MOV	CX,(_EXE_RELOC_OFF-_EXE_MIN_ABOVE)/2
		REP	MOVSW
		MOV	AX,40H				;NEW RELOC_OFF IS 40H
		STOSW
		LODSW
		;
		;MOVE UP TO RELOC_OFF OR 003C, WHICHEVER IS SMALLER
		;
		MOVSW			;OVERLAY #
		SUB	AX,SI		;BYTES TILL RELOC_OFF
		MOV	CX,3CH-_EXE_OVLY_NUM-2
		CMP	CX,AX
		JB	52$
		XCHG	AX,CX
52$:
		REP	MOVSB
		;
		;FILL TO 003C WITH ZEROS
		;
		MOV	CX,OFF EXEHEADER+3CH
		SUB	CX,DI
		XOR	AX,AX
		REP	STOSB
		;
		;STORE A 16-BYTE BOUNDARY AS OS2EXE OFFSET...
		;
		PUSH	DS				;WE WILL BE COPYING WHOLE FILE, OVERLAYS, ETC.
		MOV	BX,OBJ_DEVICE
		MOV	AX,SS:[BX].MYI_FILE_LIST_GINDEX
		CONVERT_GINDEX_AX_DSSI	_FILE_LIST_GARRAY
		MOV	CX,[SI].FILE_LIST_NFN.NFN_FILE_LENGTH.LW
		MOV	BX,[SI].FILE_LIST_NFN.NFN_FILE_LENGTH.HW
		;
		;ADD IN ADJUSTMENT FOR CHANGE IN HEADER SIZE...
		;
		MOV	AX,OLD_EXE_HDR_SIZE
		SUB	AX,EXEHEADER._EXE_HDR_SIZE
		MOV	DX,16
		IMUL	DX
		SUB	CX,AX
		SBB	BX,DX
		XCHG	AX,CX
		;
		ADD	AX,15
		ADC	BX,0
		AND	AL,0F0H
		STOSW
		XCHG	AX,BX
		STOSW
		;
		;NOW WRITE 40H BYTES FROM EXEHEADER
		;
		FIXDS
		LEA	SI,EXEHEADER
		MOV	CX,40H
		CALL	MOVE_EXEHDR_TO_FINAL
		;
		;NOW MOVE ANY AND ALL RELOCATIONS
		;
		POPM	DS,CX			;CX IS BYTES IN BUFFER

		ASSUME	DS:NOTHING

		MOV	SI,DS:_EXE_RELOC_OFF
		SUB	CX,SI
		JC	9$
		MOV	AX,4
		MUL	DS:_EXE_N_RELOC 	;DX:AX IS # OF BYTES TO MOVE...
		CALL	MOVE_BUNCH

		CALL	PARA_OUTPUT		;ROUND OUTPUT TO PARAGRAPH
		;
		;NOW MOVE REST OF EXE (INCLUDING OVERLAYS, CV INFO, ETC.)
		;
		MOV	AX,16
		MUL	OLD_EXE_HDR_SIZE	;BYTES IN HEADER
		MOV	BX,AX
		AND	AX,PAGE_SIZE-1		;DESIRED OFFSET IN BLOCK
		SUB	AX,SI			;CURRENT OFFSET IN BLOCK
		SUB	CX,AX
		JC	9$
		ADD	SI,AX
		;
		PUSHM	DS,SI,CX
		MOV	BX,OBJ_DEVICE
		MOV	AX,SS:[BX].MYI_FILE_LIST_GINDEX
		CONVERT_GINDEX_AX_DSSI	_FILE_LIST_GARRAY
		MOV	CX,[SI].FILE_LIST_NFN.NFN_FILE_LENGTH.LW
		MOV	BX,[SI].FILE_LIST_NFN.NFN_FILE_LENGTH.HW
		MOV	AX,16
		MUL	OLD_EXE_HDR_SIZE
		SUB	CX,AX
		SBB	BX,DX
		MOV	DX,BX
		XCHG	AX,CX

		POPM	CX,SI,DS

		CALL	MOVE_BUNCH
		XOR	BX,BX
		MOV	AX,DS
		MOV	DS,BX
		CALL	RELEASE_IO_SEGMENT

		CALL	PARA_OUTPUT
		MOV	BX,OBJ_DEVICE
		MOV	SS:[BX].MYI_PTR.SEGM,0
iff	fgh_os2
		CALL	CLOSE_MYI_HANDLE
endif
if	fgh_windll
		CALL	REPORT_CLOSE_ASCIZ
endif
		;
		;OK, THAT SHOULD DO IT...
		;
		JMP	8$

9$:
		MOV	CL,BAD_STUB_ERR
		CALL	ERR_ABORT


SEGM_OUT_INIT	ENDP

		ASSUME	DS:NOTHING

MOVE_BUNCH	PROC	NEAR
		;
		;DS:SI IS DATA SOURCE, CX IS BYTES LEFT THIS BLOCK
		;DX:AX IS # OF BYTES TO MOVE
		;
		OR	DX,DX
		JZ	MB_3
		JMP	MB_2

MB_1:
		CALL	MB_GNB		;DS:SI IS BUFFER, CX IS BYTE COUNT
MB_2:
		CALL	MB_MOVE_CX_FLUSH

		SUB	AX,CX
		MOV	CX,0
		SBB	DX,CX
		JNZ	MB_1
		JMP	MB_3

MB_LT64:
		CALL	MB_GNB		;DS:SI IS POINTER, CX IS COUNT
MB_3:
		;
		;CX IS BYTES LEFT THIS BLOCK
		;AX IS TOTAL # OF BYTES TO MOVE
		;
		;MOVE SMALLER OF AX AND CX
		;
		MOV	DX,CX		;# IN BLOCK
		CMP	CX,AX
		JB	MB_4
		MOV	CX,AX
MB_4:
		CALL	MB_MOVE_CX_FLUSH
		SUB	AX,CX
		JNZ	MB_LT64
		SUB	DX,CX
		MOV	CX,DX
		;
		RET

MOVE_BUNCH	ENDP

MB_GNB		PROC	NEAR
		;
		;GET NEXT BUFFER
		;
		PUSHM	DX,AX
		FIXDS
;		MOV	AX,DS
;		CALL	RELEASE_SEGMENT
		MOV	BX,OBJ_DEVICE
;		MOV	SS:[BX].MYI_PTR.SEGM,0
		CALL	DGROUP:[BX].MYI_FILLBUF
		POPM	AX,DX
		MOV	CX,DGROUP:[BX].MYI_COUNT
		LDS	SI,DGROUP:[BX].MYI_PTR
		ASSUME	DS:NOTHING
		RET

MB_GNB		ENDP

MB_MOVE_CX_FLUSH	PROC	NEAR
		;
		;
		;
		PUSHM	DX,AX,CX,DS
		CALL	MOVE_EXEHDR_TO_FINAL		;STORE THAT MUCH
		POPM	DS,CX,AX,DX
		RET

MB_MOVE_CX_FLUSH	ENDP

		ASSUME	DS:NOTHING

PARA_OUTPUT	PROC	NEAR
		;
		;ROUND HEADER OUTPUT TO PARAGRAPH BOUNDARY
		;
		XOR	AX,AX
		SUB	AX,EXEHDR_ADDR.LW
		AND	AX,0FH
		JZ	9$
		XCHG	AX,CX
		PUSHM	DS,SI,ES,DI,AX
		FIXDS
		LEA	SI,ZEROS_16
		CALL	MOVE_EXEHDR_TO_FINAL
		POPM	CX,DI,ES,SI,DS
9$:
		RET

PARA_OUTPUT	ENDP

		ASSUME	DS:NOTHING

PROCESS_OLD	PROC
		;
		;AN 'OLD' FILE WAS GIVEN..., NOW WHAT?
		;
		;FIRST, WERE ANY EXPORTS DECLARED?
		;
		CMP	ENTRYNAME_STUFF.ALLO_HASH_TABLE_PTR.SEGM,0
		JZ	9$		;NOPE, SKIP IT...
		;
		;OK, SET UP TO OPEN AND READ FROM OLD FILE
		;
		MOV	AX,OLD_LIST.FILE_FIRST_GINDEX
		CONVERT_GINDEX_AX_DSSI	_FILE_LIST_GARRAY
		MOV	AX,[SI].FILE_LIST_NEXT_GINDEX
		MOV	CX,AX
		CONVERT_GINDEX_AX_DSSI	_FILE_LIST_GARRAY

		CALL	OPEN_READING		;GET OPEN OLD FILE...
		JC	9$
		CALL	N_RES_TABLE
		OR	AX,AX
		JZ	5$
		CALL	PROCESS_THIS_SET
5$:
		;
		;NOW, GET # OF BYTES IN NONRESIDENT NAME TABLE
		;
		CALL	N_NONRES_TABLE		;0 IF ERROR OR NONE...
		OR	AX,AX
		JZ	8$
		CALL	PROCESS_THIS_SET
8$:
		;
		;RELEASE BLOCK AT MYI_PTR IF THERE
		;
		CALL	RELEASE_MYI_PTR
iff	fgh_os2
		CALL	CLOSE_MYI_HANDLE
endif

9$:
		RET

PROCESS_OLD	ENDP


		ASSUME	DS:NOTHING

PROCESS_THIS_SET	PROC	NEAR
		;
		;
		;
		FIXDS
		MOV	NONRES_BYTES_LEFT,AX
		MOV	BX,OBJ_DEVICE
		CALL	DGROUP:[BX].MYI_FILLBUF	;GET DATA
if	fgh_os2
		MOV	DGROUP:[BX].MYI_BUFCNT,0
endif
		ASSUME	DS:NOTHING
3$:
		CALL	GET_NEXT_NAME		;DS:SI POINTS TO NEXT
		JC	9$			;DX IS HASH, SYMBOL_DATA
		MOV	CX,[SI] 		;ENTRY #
		;
		;FIRST SEE IF IT IS USED UP...
		;
		JCXZ	3$			;ORD 0 IS DESCRIPTION...
		LDS	SI,ENTRYNAME_BITS
		SYM_CONV_DS
		PUSH	CX
		MOV	BX,CX
		SHRI	BX,3		;THAT BYTE
		AND	CX,7
		MOV	AX,1
		SHL	AL,CL		;THAT BIT...
		TEST	[SI+BX],AL
		POP	CX
		JNZ	3$		;ORD ALREADY DEFINED
		PUSH	CX
		;
		;SEE IF THAT EXPORT EXISTS
		;
		CALL	SEARCH_ENTRYNAME	;
		POP	CX
		JC	3$		;NOT AN EXPORT ANY MORE
		CMP	_ENT_ORD[BX],CX
		JZ	3$
		CMP	_ENT_ORD[BX],0
		JNZ	3$
		MOV	_ENT_ORD[BX],CX
		LDS	SI,ENTRYNAME_BITS
		SYM_CONV_DS
		MOV	BX,CX
		SHRI	BX,3		;THAT BYTE
		AND	CX,7
		MOV	AX,1
		SHL	AL,CL		;THAT BIT...
		OR	[SI+BX],AL
		JMP	3$

9$:
		RET

PROCESS_THIS_SET	ENDP


		ASSUME	DS:NOTHING

GET_NEXT_NAME	PROC	NEAR
		;
		;CARRY MEANS FINISHED
		;RETURN DS:SI AT END OF TEXT (PTING TO ORD #)
		;DX IS HASH, SYMBOL_LENGTH IS SYMBOL
		;
		MOV	BX,OBJ_DEVICE
		MOV	AX,1
		CALL	GNN		;RETURN PTR TO ONE BYTE
		JC	9$
		FIXES
		LEA	DI,SYMBOL_LENGTH
		LODSB
		XOR	AH,AH
		STOSW
		OR	AX,AX
		JZ	9$		;ZERO LENGTH IS END OF TABLE
		PUSH	AX
		CALL	GNN		;RETURN PTR TO AX BYTES
		POP	AX
		JC	9$
		CALL	OPTI_MOVE_PRESERVE_SIGNIFICANT	;HASHED
		MOV	AX,2		;PTR TO ORD #
		CALL	GNN
		JC	9$
		RET

9$:
		;
		;TABLE IS DONE...
		;
		FIXES
		STC
		RET

GET_NEXT_NAME	ENDP


		ASSUME	DS:NOTHING

GNN		PROC	NEAR
		;
		;RETURN PTR TO AX BYTES
		;
		SUB	NONRES_BYTES_LEFT,AX
		JC	9$
2$:
		SUB	DGROUP:[BX].MYI_COUNT,AX
		JC	1$
		LDS	SI,DGROUP:[BX].MYI_PTR
		ADD	AX,SI
		MOV	DGROUP:[BX].MYI_PTR.OFFS,AX
9$:
		RET

1$:
		PUSHM	CX,AX,DI
		MOV	CX,DGROUP:[BX].MYI_COUNT
		ADD	CX,AX
		LEA	DI,OLD_TEMP_RECORD
		FIXES
		SUB	AX,CX		;NUMBER TO MOVE AFTER THE FACT
		JCXZ	RH_1
		LDS	SI,DGROUP:[BX].MYI_PTR
		OPTI_MOVSB
RH_1:
;		CALL	RELEASE_MYI_PTR 	;RELEASE OLD DATA
		FIXDS
		CALL	DGROUP:[BX].MYI_FILLBUF
if	fgh_os2
		MOV	DGROUP:[BX].MYI_BUFCNT,0
endif
		XCHG	AX,CX
		SUB	DGROUP:[BX].MYI_COUNT,CX
		JC	8$
		LDS	SI,DGROUP:[BX].MYI_PTR
		ASSUME	DS:NOTHING
		OPTI_MOVSB
		MOV	DGROUP:[BX].MYI_PTR.OFFS,SI
		POPM	DI,AX
		FIXDS
		POP	CX
		LEA	SI,OLD_TEMP_RECORD
		CMP	AL,AL
		RET

8$:
		POPM	DI,AX,CX
		STC
		RET

GNN		ENDP

endif

;iff	data_in_codeseg

;		.DATA

;endif

if	fg_segm

		EVEN	?

DFLT_STUB	DW	'ZM',DFLT_STUB_LEN,1,0,4,10H,-1,0
		DW	00FEH,0,12H,0,40H,0,0,0
		DW	0,0,0,0,0,0,0,0
		DW	0,0,0,0,0,0,DFLT_STUB_LEN,0
		DB	'Not a DOS Progra'
		DB	'm$',16H,1FH,33H,0D2H,0B4H,9
		DB	0CDH,21H,0B8H,01,4CH,0CDH,21H,0
;		db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;		db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
DFLT_STUB_LEN	EQU	$-DFLT_STUB

endif

		END

