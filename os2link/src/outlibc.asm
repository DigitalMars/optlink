		TITLE	OUTLIBC - Copyright (C) SLR Systems 1995

		INCLUDE	MACROS
if	V5
		INCLUDE	SYMBOLS
		INCLUDE	SEGMSYMS
		INCLUDE	IO_STRUC
		INCLUDE	PE_STRUC
                INCLUDE COFF_LIB
		INCLUDE	SEGMENTS
		INCLUDE	CLASSES
		INCLUDE	EXES
		INCLUDE	RELEASE

		PUBLIC	MAKE_COFF_IMPLIB


		.DATA

		EXTERNDEF	TEMP_RECORD:BYTE,MODULE_NAME:BYTE
		EXTERNDEF	EXP_BUFFER:DWORD

		EXTERNDEF	IMP_DEVICE:DWORD
		EXTERNDEF	ENTRYNAMES_TOTAL_LENGTH:DWORD,FIRST_ENTRYNAME_GINDEX:DWORD,IMPLIB_PAGEBITS:DWORD

		EXTERNDEF	ENTRYNAME_GARRAY:STD_PTR_S,SYMBOL_GARRAY:STD_PTR_S

		EXTERNDEF	IMP_OUTALL:DWORD,IMPLIB_COFF_FILE_GINDEX:DWORD
		EXTERNDEF	GOBJ1_SECT_RAW2_LEN:DWORD,GOBJ1_SECTION_STUFF:BYTE
		EXTERNDEF	FBN_SECT_RAW2_LEN:DWORD,FBN_SECTION_STUFF:BYTE
		EXTERNDEF	DBN_SECT_RAW2_LEN:DWORD,DBN_SECTION_STUFF:BYTE
		EXTERNDEF	CURN_C_TIME:DWORD
		EXTERNDEF	COFF_HELPERS_STRUCTURE:SEQ_STRUCT

		.CODE	MIDDLE_TEXT

		EXTERNDEF	UNUSE_SORTED_EXPORTS:PROC

		.CODE	ROOT_TEXT


		EXTERNDEF	MOVE_ASCIZ_ECX_EAX:PROC,MOVE_ASCIZ_ESI_EDI:PROC,CBTA32:PROC
		EXTERNDEF	READ_EAXECX_EDXEBX_RANDOM:PROC,STORE_EAXECX_EDXEBX_RANDOM:PROC,STORE_EAXECX_EDX_SEQ:PROC
		EXTERNDEF	RELEASE_BLOCK:PROC

		.CODE	PASS2_TEXT

		EXTERNDEF	IMP_INIT:PROC,_flush_trunc_close:proc

OUTLIBC_VARS	STRUC

WM_PTR_BP		DD	?
WM_CNT_BP		DD	?
WM_BLK_PTR_BP		DD	?

__IMP_SYM_PTR_BP	DD	?
__IMP_INSERT_BP         DD      ?
GENOBJ1_INSERT_BP       DD	?
GENOBJ2_INSERT_BP       DD	?
CHECK_INSERT_PROC_BP	DD	?

HELPER_STRINGS_BP	DD	?
HELPER_STRINGS_LENGTH_BP DD	?
HELPER_STRINGS_N_BP	DD	?
ARCHIVES_N_BP	        DD      ?
HELPER_TWO_BP		DD	?

MODNAME_LENGTH_BP	DD	?
ALIAS_LENGTH_BP		DD	?
STRINGTABLE_LENGTH_BP	DD	?

NEXT_HELPER_STRING_BP   DD      ?
NEXT_CH1_OFFSET_BP      DD      ?
NEXT_CH2_OFFSET_BP      DD      ?
NEXT_CH2_INDEX_BP       DD      ?

GENOBJ1_FOFFSET_BP	DD	?
GENOBJ2_FOFFSET_BP	DD	?
GENOBJ3_FOFFSET_BP	DD	?
INITIAL_FOFFSET_BP	DD	?

CURRENT_FOFFSET_BP	DD	?
INNER_FOFFSET_BP	DD	?

INSERT_SAVE_CNT_BP      DD	?
INSERT_SAVE_PTR_BP      DD	?
INSERT_SAVE_BLK_PTR_BP  DD	?
DWORDOUT_BP		DD	?

OUTLIBC_VARS	ENDS


FIX	MACRO	X

X	EQU	([EBP-SIZE OUTLIBC_VARS].(X&_BP))

	ENDM

FIX     WM_PTR
FIX     WM_CNT
FIX     WM_BLK_PTR
FIX     __IMP_SYM_PTR
FIX     __IMP_INSERT
FIX     GENOBJ1_INSERT
FIX     GENOBJ2_INSERT
FIX	CHECK_INSERT_PROC
FIX     HELPER_STRINGS
FIX     HELPER_STRINGS_LENGTH
FIX     HELPER_STRINGS_N
FIX     ARCHIVES_N
FIX     HELPER_TWO
FIX	MODNAME_LENGTH
FIX     ALIAS_LENGTH
FIX	STRINGTABLE_LENGTH
FIX     NEXT_HELPER_STRING
FIX     NEXT_CH1_OFFSET
FIX     NEXT_CH2_OFFSET
FIX     NEXT_CH2_INDEX
FIX     GENOBJ1_FOFFSET
FIX     GENOBJ2_FOFFSET
FIX     GENOBJ3_FOFFSET
FIX     INITIAL_FOFFSET
FIX     CURRENT_FOFFSET
FIX     INNER_FOFFSET
FIX     INSERT_SAVE_CNT
FIX     INSERT_SAVE_PTR
FIX     INSERT_SAVE_BLK_PTR
FIX     DWORDOUT


                ;MAKE_COFF_IMPLIB
                ;
                ;  EAX   IMPLIB_COFF_FILE_GINDEX
                ;  CL    IMPLIB_COFF_FLAG
                ;
                ;Ok, we make the 2 helper guys , drop them,
		; then drop 3 static (generated) archives,
		; then drop an archive for each exported symbol.
                ;
MAKE_COFF_IMPLIB PROC
                PUSHM	EBP,EDI,ESI,EBX

		MOV	EBP,ESP
		SUB	ESP,SIZE OUTLIBC_VARS
		ASSUME	EBP:PTR OUTLIBC_VARS
		;
		;OPEN COFF .LIB FILE
		;
		CALL	IMP_INIT
		MOV	IMP_DEVICE,EAX
                ;
                ;Walk sorted export list calculating import library file offsets
                ;
		CALL	TBLINIT_1

		CALL	FORMAT_INIT
		JMP	L2$

L1$:
		CALL	COFF_IMPLIB_FIRST_PASS
L2$:
		CALL	TBLNEXT_1
		JNZ	L1$

		CALL	FORMAT_ONE
                ;
                ;Walk sorted export list building Helper file offset and index lists, stringtable
                ;
		CALL	TBLINIT_1
		JMP	L4$

L3$:
		CALL	COFF_IMPLIB_SECOND_PASS
L4$:
		CALL	TBLNEXT_1
		JNZ	L3$

		CALL	FORMAT_TWO
		;
		;OUTPUT EACH ENTRYNAME THAT IS DEFINED
		;
		CALL	TBLINIT_1
		JMP	L6$

L5$:
		CALL	COFF_IMPLIB_THIRD_PASS
L6$:
		CALL	TBLNEXT_1
		JNZ	L5$

		CALL	UNUSE_SORTED_EXPORTS    ;Indicate done with the list
		;
		;  CLOSE FILE
		;
		MOV	EAX,IMP_DEVICE
		push	EAX
		call	_flush_trunc_close	;NOW CLOSE
		add	ESP,4

		MOV	ESP,EBP
		POPM	EBX,ESI,EDI,EBP

L9$:
		RET

MAKE_COFF_IMPLIB	ENDP


APPEND_CH1_OFFSET_EDX PROC NEAR

		PUSHM	EBX
                XCHG    DH,DL
                ROR     EDX,16
                XCHG    DH,DL
                MOV     DWORDOUT,EDX
		MOV	EBX,NEXT_CH1_OFFSET
		ADD	NEXT_CH1_OFFSET,4
                LEA	EAX,DWORDOUT
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
		MOV	ECX,4
		CALL	STORE_EAXECX_EDXEBX_RANDOM
		POPM	EBX
                RET

APPEND_CH1_OFFSET_EDX ENDP


APPEND_CH2_INDEX_DX PROC NEAR

		PUSHM	EBX
                MOV     WPTR DWORDOUT,DX
                LEA	EAX,DWORDOUT
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
		MOV	ECX,2
		MOV	EBX,NEXT_CH2_INDEX
		ADD	NEXT_CH2_INDEX,2
		CALL	STORE_EAXECX_EDXEBX_RANDOM
		POPM	EBX
                RET

APPEND_CH2_INDEX_DX ENDP


APPEND_CH2_OFFSET_EDX PROC NEAR

		PUSHM	EBX
                MOV     DWORDOUT,EDX
                LEA	EAX,DWORDOUT
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
		MOV	ECX,4
		MOV	EBX,NEXT_CH2_OFFSET
		ADD	NEXT_CH2_OFFSET,4
		CALL	STORE_EAXECX_EDXEBX_RANDOM
                MOV     EDX,DWORDOUT
		POPM	EBX
                RET

APPEND_CH2_OFFSET_EDX ENDP


APPEND_HELPER_STRING_ECX PROC NEAR

		PUSHM	EBX,EDX
                MOV	EDI,ECX
                MOV	EDX,ECX
                MOV	ECX,-1
                XOR	EAX,EAX
                REPNE	SCASB
                MOV	ECX,EDI
                SUB	ECX,EDX
                MOV	EAX,EDX
		MOV	EBX,NEXT_HELPER_STRING
		ADD	NEXT_HELPER_STRING,ECX
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
		CALL	STORE_EAXECX_EDXEBX_RANDOM
		POPM	EDX,EBX
                RET

APPEND_HELPER_STRING_ECX ENDP


CHECK_DLL_FIRST PROC	NEAR

                CMP	GENOBJ1_INSERT,EDX
                JE      L3$
L1$:
                CMP	GENOBJ2_INSERT,EDX
                JE      L4$
L2$:
                CMP     __IMP_INSERT,EDX
                JE      L5$

                RET
L3$:
		CALL	INSERT_FIRST_OBJ
                JMP	L1$
L4$:
		CALL	INSERT_SECOND_OBJ
                JMP	L2$
L5$:
		JMP	INSERT__IMP_

CHECK_DLL_FIRST ENDP


CHECK_DLL_LAST PROC	NEAR

                CMP	GENOBJ2_INSERT,EDX
                JE      L3$
L1$:
                CMP     __IMP_INSERT,EDX
                JE      L4$
L2$:
                CMP	GENOBJ1_INSERT,EDX
                JE      L5$

                RET
L3$:
		CALL	INSERT_SECOND_OBJ
                JMP	L1$
L4$:
		CALL	INSERT__IMP_
                JMP	L2$
L5$:
		JMP	INSERT_FIRST_OBJ

CHECK_DLL_LAST ENDP


CHECK_DLL_MIDDLE PROC	NEAR

                CMP	GENOBJ2_INSERT,EDX
                JE      L3$
L1$:
                CMP	GENOBJ1_INSERT,EDX
                JE      L4$
L2$:
                CMP     __IMP_INSERT,EDX
                JE      L5$

                RET
L3$:
		CALL	INSERT_SECOND_OBJ
                JMP	L1$
L4$:
		CALL	INSERT_FIRST_OBJ
                JMP	L2$
L5$:
		JMP	INSERT__IMP_

CHECK_DLL_MIDDLE ENDP


COFF_IMPLIB_FIRST_PASS	PROC	NEAR

		;
		;EBX IS ENTRYNAME GINDEX
		;
		CONVERT	EBX,EBX,ENTRYNAME_GARRAY
		ASSUME	EBX:PTR ENT_STRUCT

		MOV	ESI,[EBX]._ENT_INTERNAL_NAME_GINDEX
		CONVERT	ESI,ESI,SYMBOL_GARRAY
		ASSUME	ESI:PTR SYMBOL_STRUCT

                MOV     AX,WPTR [EBX]._ENT_FLAGS ;AL FLAGS, AH FLAGS_EXT
		TEST	AL,MASK ENT_UNDEFINED+MASK ENT_PRIVATE
		JNZ	L6$
                ;
                ; determine whether code or data
                ;
                TEST	AH,MASK ENT_EXT_DEFREF	;on DEF file ?
                JNZ	L1$
                ;
                ; If not from a .DEF file - examine section class type
                ;
		MOV	EDX,[ESI]._S_SEG_GINDEX
		CONVERT	EDX,EDX,SEGMENT_GARRAY
		ASSUME	EDX:PTR SEGMENT_STRUCT

		MOV	EDX,[EDX]._SEG_CLASS_GINDEX
		CONVERT	EDX,EDX,CLASS_GARRAY
		ASSUME	EDX:PTR CLASS_STRUCT

                AND     AH,NOT MASK ENT_EXT_DATA
		TEST	[EDX]._C_TYPE_FLAG,MASK SEG_CLASS_IS_CODE

		ASSUME	EDX:NOTHING
                JNZ	L1$

                OR      AH,MASK ENT_EXT_DATA
L1$:
                LEA     EDI,[ESI]._S_NAME_TEXT
                ASSUME  ESI:NOTHING

                MOV     ESI,EDI
L11$:
                INC     EDI
                TEST    BPTR [EDI-1],0FFh
                JNZ     L11$

                SUB     EDI,ESI			;Alias ASCIIZ length
                MOV     ALIAS_LENGTH,EDI
                ;
		;Calculate variable archive entry length in EDX
		;
		;                               Function		    Data
		;			    By Name	 By Ordinal  By Name	 By Ordinal
		; COFF File header 		     20	         20	      20	 20
		; Section headers           4*40=   160  3*40=  120  3*40=   120 2*40=   80
		; Raw Data and Relocations  16+3*10= 46  14+1*10=24  8+2*10=  28 8+0*10=  8
	  	;			   +RawNameEven		    +RawNameEven
		; Symbol Table              11*18=  198  9*18=  162  8*18=   144 6*18=  108
		; String Table                       29          29           29         29
		;			     +ModNamLen  +ModNamLen   +ModNamLen +ModNamLen
                ;                            +  2*Alias  +  2*Alias   +    Alias +    Alias
		;
		; Total Fixed                       453         355          343        245
		; Total Var		     +ModNamLen  +ModNamLen   +ModNamLen +ModNamLen
                ;                            +  2*Alias  +  2*Alias   +    Alias +    Alias
		;			   +RawNameEven	 	    +RawNameEven
		;
		MOV	EDX,MODNAME_LENGTH
                ADD     EDX,EDI
                ;
                ;DATA symbol will insert '__imp_'+alias in Helper StringTable
                ;CODE symbol will insert alias and '__imp_'+alias in Helper StringTable
                ;
                ADD     HELPER_STRINGS_LENGTH,EDI
                ADD     HELPER_STRINGS_LENGTH,6
                INC     HELPER_STRINGS_N

                TEST    AH,MASK ENT_EXT_DATA
                JNZ     L12$

                ADD     HELPER_STRINGS_LENGTH,EDI
                INC     HELPER_STRINGS_N
                ADD     EDX,EDI                         ;Extra StringTable Alias if code
L12$:
		;
		TEST	AL,MASK ENT_BYNAME		;IF EXPORTING THIS BY NAME
		JNZ	L2$

                TEST    AL,MASK ENT_ORD_SPECIFIED
                JNZ     L3$

		GETT	CH,ALL_EXPORTS_BY_ORDINAL
		OR	CH,CH
		JNZ	L3$
                ;
                ;By Name
                ;
L2$:
		AND	AH, NOT MASK ENT_EXT_BYORD

                LEA	EDI,[EBX]._ENT_TEXT
                MOV     ECX,EDI
L21$:
                INC     EDI
                TEST    BYTE PTR [EDI-1],0FFh
                JNZ     L21$

                SUB     EDI,ECX
                INC     EDI
                AND     DI,0FFFEh

                ADD     EDI,343
                TEST    AH,MASK ENT_EXT_DATA
                JNZ     L4$

                ADD     EDI,110
	        JMP     L4$
                ;
                ;By Ordinal
                ;
L3$:
		OR	AH,MASK ENT_EXT_BYORD
                MOV     EDI,245

                TEST    AH,MASK ENT_EXT_DATA
                JNZ     L4$

                ADD     EDI,110
L4$:
		MOV	[EBX]._ENT_FLAGS_EXT,AH
                ADD     EDI,EDX
		MOV     [EBX]._ENT_COFF_ARCHIVE_SIZE,DI

                INC	ARCHIVES_N		;Zero based HINT + 1
                MOV     EDX,ARCHIVES_N
                MOV     [EBX]._ENT_COFF_ARCHIVE_HINT,DX
                ;
                ;(EBX)	offset of Entry structure
                ;(EDX)  hint
                ;(ESI)  offset of alias
		;
                MOV     EAX,ESI
                ;
                ;Check for sort insertions
                ;
                TEST    __IMP_INSERT,-1
                JZ      L7$
L5$:
                TEST    GENOBJ1_INSERT,-1
                JZ      L9$
L51$:
                TEST    GENOBJ2_INSERT,-1
                JZ      L8$
L6$:
                RET

L7$:
		MOV     EDI,OFF __IMP_CONST
                MOV     ECX,SIZEOF __IMP_CONST + 1
                CMP     ECX,ALIAS_LENGTH
                JB      L72$

                DEC	ECX
                CALL    COMPARE_ALIAS
                JBE     L5$
L71$:
                MOV	__IMP_INSERT,EDX
                JMP     L5$

L72$:
                DEC	ECX
                CALL    COMPARE_ALIAS
                JB      L5$
		JA	L71$

                LEA     ESI,[EAX+SIZEOF __IMP_CONST]
                MOV	EDI,__IMP_SYM_PTR
                MOV     ECX,ALIAS_LENGTH
                SUB     ECX,SIZEOF __IMP_CONST
                REPE	CMPSB
                JBE     L5$
                JMP	L71$

L8$:
                MOV     EDI,OFF GENOBJ2_CONST
                MOV     ECX,SIZEOF GENOBJ2_CONST + SIZEOF GENOBJ1_CONST
                CALL    COMPARE_ALIAS
                JBE     L6$

                MOV	GENOBJ2_INSERT,EDX
                JMP     L6$

L9$:
		MOV	EDI,OFF MODULE_NAME+4
		MOV	ECX,MODNAME_LENGTH
                CALL    COMPARE_ALIAS
                JB      L51$
                JA      L92$

                MOV     ECX,ALIAS_LENGTH
		SUB	ECX,MODNAME_LENGTH
                JBE     L51$

                CMP     ECX,SIZEOF GENOBJ1_CONST
                JNA     L91$

                MOV     ECX,SIZEOF GENOBJ1_CONST
L91$:
                MOV     EDI,OFF GENOBJ1_CONST
                REPE	CMPSB
                JBE     L51$
L92$:
                MOV	GENOBJ1_INSERT,EDX
                JMP     L51$

COFF_IMPLIB_FIRST_PASS	ENDP


COFF_IMPLIB_SECOND_PASS	PROC	NEAR
		;
		;EBX IS ENTRYNAME GINDEX
		;
		CONVERT	EBX,EBX,ENTRYNAME_GARRAY
		ASSUME	EBX:PTR ENT_STRUCT

		MOV	ESI,[EBX]._ENT_INTERNAL_NAME_GINDEX
		CONVERT	ESI,ESI,SYMBOL_GARRAY
		ASSUME	ESI:PTR SYMBOL_STRUCT

		TEST	[EBX]._ENT_FLAGS,MASK ENT_UNDEFINED+MASK ENT_PRIVATE
		JNZ	L5$
                ;
                ;Check for sort insertions
                ;

                XOR	EDX,EDX
                MOV     DX,[EBX]._ENT_COFF_ARCHIVE_HINT

		CALL	CHECK_INSERT_PROC

                TEST    [EBX]._ENT_FLAGS_EXT,MASK ENT_EXT_DATA
		JNZ	L4$

                ADD     DX,3
                CALL    APPEND_CH2_INDEX_DX

                LEA     ECX,[ESI]._S_NAME_TEXT
                CALL    APPEND_HELPER_STRING_ECX

                MOV     EDX,CURRENT_FOFFSET
                CALL    APPEND_CH1_OFFSET_EDX
L4$:
                MOV     EDX,CURRENT_FOFFSET
                CALL    APPEND_CH2_OFFSET_EDX

                XOR	ECX,ECX
		MOV	CX,[EBX]._ENT_COFF_ARCHIVE_SIZE
                INC     ECX
                AND     CL,0FEh
                ADD     EDX,ECX
                ADD     EDX,SIZE COFF_ARCHIVE_HDR
                MOV     CURRENT_FOFFSET,EDX
L5$:
                RET
COFF_IMPLIB_SECOND_PASS	ENDP


COFF_IMPLIB_SECOND_PASS_INNER	PROC	NEAR
		;
		;EBX IS ENTRYNAME GINDEX
		;
		CONVERT	EBX,EBX,ENTRYNAME_GARRAY
		ASSUME	EBX:PTR ENT_STRUCT

		TEST	[EBX]._ENT_FLAGS,MASK ENT_UNDEFINED+MASK ENT_PRIVATE
		JNZ	L9$

		MOV	ESI,[EBX]._ENT_INTERNAL_NAME_GINDEX
		CONVERT	ESI,ESI,SYMBOL_GARRAY
		ASSUME	ESI:PTR SYMBOL_STRUCT

                XOR	EDX,EDX
                MOV     DX,[EBX]._ENT_COFF_ARCHIVE_HINT

                ADD     DX,3
                CALL    APPEND_CH2_INDEX_DX

                MOV     EAX,OFF __IMP_CONST
                MOV     ECX,SIZEOF __IMP_CONST
                PUSHM	EBX
		MOV	EBX,NEXT_HELPER_STRING
		ADD	NEXT_HELPER_STRING,ECX
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
		CALL	STORE_EAXECX_EDXEBX_RANDOM
                POPM	EBX

                LEA     ECX,[ESI]._S_NAME_TEXT
                CALL    APPEND_HELPER_STRING_ECX

                MOV     EDX,INNER_FOFFSET
                CALL    APPEND_CH1_OFFSET_EDX

                XOR	ECX,ECX
		MOV	CX,[EBX]._ENT_COFF_ARCHIVE_SIZE
                INC     ECX
                AND     CL,0FEh
                MOV     EDX,INNER_FOFFSET
                ADD     EDX,ECX
                ADD     EDX,SIZE COFF_ARCHIVE_HDR
                MOV     INNER_FOFFSET,EDX
L9$:
                RET
COFF_IMPLIB_SECOND_PASS_INNER	ENDP


COFF_IMPLIB_THIRD_PASS	PROC	NEAR
		;
		;EBX IS ENTRYNAME GINDEX
		;
		CONVERT	EBX,EBX,ENTRYNAME_GARRAY
		ASSUME	EBX:PTR ENT_STRUCT

                MOV     AX,WPTR [EBX]._ENT_FLAGS ;AL FLAGS, AH FLAGS_EXT
		TEST	AL,MASK ENT_UNDEFINED+MASK ENT_PRIVATE
		JNZ	L7$

		MOV	ESI,[EBX]._ENT_INTERNAL_NAME_GINDEX
		CONVERT	ESI,ESI,SYMBOL_GARRAY
		ASSUME	ESI:PTR SYMBOL_STRUCT

                LEA     EDX,[ESI]._S_NAME_TEXT
                ASSUME  ESI:NOTHING

                MOV     ESI,EDX
L1$:
                INC     EDX
                TEST    BPTR [EDX-1],0FFh
                JNZ     L1$

                SUB     EDX,ESI			;Alias ASCIIZ length
                MOV     ALIAS_LENGTH,EDX

                TEST    AH,MASK ENT_EXT_DATA
                JNZ      L2$

                ADD	EDX,EDX
L2$:
                ADD	EDX,MODNAME_LENGTH
                MOV	STRINGTABLE_LENGTH,EDX
                ADD	STRINGTABLE_LENGTH,29

                XOR	EAX,EAX
		MOV	AX,[EBX]._ENT_COFF_ARCHIVE_SIZE
                CALL    STORE_ARCHIVE_SIZE

                MOV	EDI,OFF TEMP_SECT_HDR
                XOR	ECX,ECX
		MOV	CX,[EBX]._ENT_COFF_ARCHIVE_SIZE
                SUB	ECX,20
		SHR	ECX,2
                INC	ECX
                XOR	EAX,EAX
                REP     STOSD

                MOV     AX,WPTR [EBX]._ENT_FLAGS ;AL FLAGS, AH FLAGS_EXT
                TEST    AH,MASK ENT_EXT_BYORD
                JNZ     L5$
                ;
                ;By Name
                ;
                LEA	EDI,[EBX]._ENT_TEXT
                MOV     ECX,EDI
L3$:
		INC	EDI
                TEST    BYTE PTR [EDI-1],0FFh
                JNZ     L3$

                SUB     EDI,ECX
                ADD     EDI,3
                AND     DI,0FFFEh

                TEST    AH,MASK ENT_EXT_DATA
                JNZ     L4$

		JMP	GENERATE_FUNCTION_BY_NAME
                ;
                ;Data By Name
                ;
L4$:
		JMP	GENERATE_DATA_BY_NAME
                ;
                ;By Ordinal
                ;
L5$:
                TEST    AH,MASK ENT_EXT_DATA
		JNZ	L6$
                ;
                ;Function by Ordinal
                ;
		JMP	GENERATE_FUNCTION_BY_ORDINAL

L6$:
                ;
                ;Data by Ordinal
                ;
		JMP	GENERATE_DATA_BY_ORDINAL
L7$:
		RET



GENERATE_DATA_BY_NAME LABEL NEAR
                ;
		;Generate Data By Name Archive
		;
		MOV	DBN_SECT_RAW2_LEN,EDI
		MOV	FBN_SECT_RAW2_LEN,EDI
                MOV	ECX,3
                MOV	TEMP_FILE_HDR._CFH_N_OBJECTS,CX
                ADD	EDI,168
                MOV	TEMP_FILE_HDR._CFH_SYMTBL_PTR,EDI
                MOV	TEMP_FILE_HDR._CFH_N_SYMBOLS,8
                ;
                ;Format Data By Name Section Table
                ;
		MOV	EDI,OFF TEMP_SECT_HDR
                MOV	EDX,OFF DBN_SECTION_STUFF
                CALL    GENERATE_SECTION_HEADERS
                ;
                ;Format Data By Name Raw Data and Relos
                ;
		MOV	BPTR [EDI+8],4
		MOV	BPTR [EDI+12],7
		MOV	BPTR [EDI+22],4
		MOV	BPTR [EDI+26],7
                ADD	EDI,30
                MOV	AX,[EBX]._ENT_COFF_ARCHIVE_HINT
                DEC	AX
                MOV	[EDI-2],AX
		PUSHM	ESI
                LEA	ESI,[EBX]._ENT_TEXT
                CALL	MOVE_ASCIZ_ESI_EDI
                INC	EDI
		POPM	ESI
		SUB	EDX,EDX
		MOV	EAX,1
                TEST	DI,1
                JZ      FINISH_DATA

                MOV	BPTR[EDI],0
                INC	EDI
		JMP	FINISH_DATA

GENERATE_DATA_BY_ORDINAL LABEL NEAR
                ;
		;Generate Data By Ordinal Archive
		;
                MOV	ECX,2
                MOV	TEMP_FILE_HDR._CFH_N_OBJECTS,CX
                MOV	TEMP_FILE_HDR._CFH_SYMTBL_PTR,108
                MOV	TEMP_FILE_HDR._CFH_N_SYMBOLS,6
                ;
                ;Format Data By Ordinal Section Table
                ;
		MOV	EDI,OFF TEMP_SECT_HDR
                MOV	EDX,OFF DBO_SECTION_STUFF
                CALL    GENERATE_SECTION_HEADERS
                ;
                ;Format Data By Ordinal Raw Data and Relos
                ;
                MOV	EAX,[EBX]._ENT_ORD
		OR	EAX,80000000h
                MOV	DPTR [EDI],EAX
                MOV	DPTR [EDI+4],EAX
                ADD	EDI,8
		SUB	EDX,EDX
		MOV	EAX,1
		JMP	FINISH_DATA

GENERATE_FUNCTION_BY_NAME LABEL NEAR
                ;
		;Generate Function By Name Archive
		;
		MOV	FBN_SECT_RAW2_LEN,EDI

                MOV	ECX,4
                MOV	TEMP_FILE_HDR._CFH_N_OBJECTS,CX
                ADD	EDI,224
                MOV	TEMP_FILE_HDR._CFH_SYMTBL_PTR,EDI
                MOV	TEMP_FILE_HDR._CFH_N_SYMBOLS,11
                ;
                ;Format Function By Name Section Table
                ;
		MOV	EDI,OFF TEMP_SECT_HDR
                MOV	EDX,OFF FBN_SECTION_STUFF
                CALL    GENERATE_SECTION_HEADERS
                ;
                ;Format Function By Name Raw Data and Relos
                ;
                MOV	WPTR [EDI],25FFh
                MOV	BPTR [EDI+6],2
		MOV	BPTR [EDI+10],10
		MOV	BPTR [EDI+14],6
		MOV	BPTR [EDI+24],7
		MOV	BPTR [EDI+28],7
		MOV	BPTR [EDI+38],7
		MOV	BPTR [EDI+42],7
                ADD	EDI,46
                MOV	AX,[EBX]._ENT_COFF_ARCHIVE_HINT
                DEC	AX
                MOV	[EDI-2],AX
		PUSHM	ESI
                LEA	ESI,[EBX]._ENT_TEXT
                CALL	MOVE_ASCIZ_ESI_EDI
                INC	EDI
		POPM	ESI
                TEST	DI,1
                JZ      FINISH_FUNCTION

                MOV	BPTR[EDI],0
                INC	EDI

FINISH_FUNCTION LABEL	NEAR
                ;
                ;Format Function Symbol Table
                ;
		ASSUME	EDI:PTR COFF_SYMTBL_ENTRY

		MOV	[EDI]._CSE_NAME_DD1,'xet.'
		MOV	[EDI]._CSE_NAME_DD2,'t'
		MOV	[EDI]._CSE_SECTION,1
		MOV	[EDI]._CSE_STORAGE_CLASS,3
		MOV	[EDI]._CSE_AUX_CNT,1
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		ASSUME	EDI:PTR COFF_SYMTBL_AUX

		MOV	[EDI]._CSA_SECTION_LENGTH,6
		MOV	[EDI]._CSA_RELOCS,1
		MOV	[EDI]._CSA_SELECT,1
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		ASSUME	EDI:PTR COFF_SYMTBL_ENTRY
                MOV	EDX,ALIAS_LENGTH
                MOV	EAX,2

FINISH_DATA	LABEL	NEAR
		ADD	EDX,4
                ;
                ;EDX  4+Alias Length (Function) or 4 (Data) for Stringtable offset calc
                ;
		MOV	[EDI]._CSE_NAME_DD1,'adi.'
		MOV	[EDI]._CSE_NAME_DD2,'5$at'
		MOV	[EDI]._CSE_SECTION,AX
		MOV	ECX,EAX
		INC	EAX
		MOV	[EDI]._CSE_STORAGE_CLASS,3
		MOV	[EDI]._CSE_AUX_CNT,1
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		ASSUME	EDI:PTR COFF_SYMTBL_AUX

		MOV	[EDI]._CSA_SECTION_LENGTH,4

                TEST	[EBX]._ENT_FLAGS_EXT,MASK ENT_EXT_BYORD
                JNZ	L81$

		MOV	[EDI]._CSA_RELOCS,1
L81$:
		MOV	[EDI]._CSA_SELECT,1
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		ASSUME	EDI:PTR COFF_SYMTBL_ENTRY

		MOV	[EDI]._CSE_NAME_DD1,'adi.'
		MOV	[EDI]._CSE_NAME_DD2,'4$at'
		MOV	[EDI]._CSE_SECTION,AX
                INC	EAX
		MOV	[EDI]._CSE_STORAGE_CLASS,3
		MOV	[EDI]._CSE_AUX_CNT,1
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		ASSUME	EDI:PTR COFF_SYMTBL_AUX

		MOV	[EDI]._CSA_SECTION_LENGTH,4

                TEST	[EBX]._ENT_FLAGS_EXT,MASK ENT_EXT_BYORD
                JNZ	L82$

		MOV	[EDI]._CSA_RELOCS,1
L82$:
		MOV	[EDI]._CSA_CD_SECT_IDX,CX
		MOV	[EDI]._CSA_SELECT,5
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

                TEST	[EBX]._ENT_FLAGS_EXT,MASK ENT_EXT_DATA
                JNZ	L83$

		ASSUME	EDI:PTR COFF_SYMTBL_ENTRY

		MOV	[EDI]._CSE_NAME_DD2,4
		MOV	[EDI]._CSE_SECTION,1
		MOV	[EDI]._CSE_STORAGE_CLASS,2
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY
L83$:
                TEST	[EBX]._ENT_FLAGS_EXT,MASK ENT_EXT_BYORD
                JNZ	L8$

		MOV	[EDI]._CSE_NAME_DD1,'adi.'
		MOV	[EDI]._CSE_NAME_DD2,'6$at'
		MOV	[EDI]._CSE_SECTION,AX
		MOV	[EDI]._CSE_STORAGE_CLASS,3
		MOV	[EDI]._CSE_AUX_CNT,1
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		ASSUME	EDI:PTR COFF_SYMTBL_AUX

		MOV	[EDI]._CSA_CD_SECT_IDX,CX
                MOV	EAX,FBN_SECT_RAW2_LEN
		MOV	[EDI]._CSA_SECTION_LENGTH,EAX
		MOV	[EDI]._CSA_SELECT,5
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		ASSUME	EDI:PTR COFF_SYMTBL_ENTRY
L8$:
		MOV	[EDI]._CSE_NAME_DD2,EDX
		MOV	[EDI]._CSE_STORAGE_CLASS,2
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		ADD	EDX,MODNAME_LENGTH
                ADD	EDX,SIZEOF GENOBJ1_CONST

		MOV	[EDI]._CSE_NAME_DD2,EDX
		MOV	[EDI]._CSE_SECTION,CX
		MOV	[EDI]._CSE_STORAGE_CLASS,2
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY
		ASSUME	EDI:NOTHING
                ;
                ;Format String Table
                ;
                MOV	EDX,STRINGTABLE_LENGTH
                MOV	[EDI],EDX
                ADD	EDI,4

                MOV	EDX,ESI

                TEST	[EBX]._ENT_FLAGS_EXT,MASK ENT_EXT_DATA
                JNZ	L9$
                ;
                MOV	ECX,ALIAS_LENGTH
                REP	MOVSB
L9$:
                CALL	MOVE_MODNAME

                MOV     ESI,OFF GENOBJ1_CONST
		CALL	MOVE_ASCIZ_ESI_EDI
		MOV	DPTR [EDI+1],'mi__'
		MOV	WPTR [EDI+5],'_p'
                ADD	EDI,7

                MOV	ESI,EDX
                MOV	ECX,ALIAS_LENGTH
                REP	MOVSB

		JMP	PUT_ARCHIVE


GENERATE_FUNCTION_BY_ORDINAL LABEL NEAR

                ;
		;Generate Function By Ordinal Archive
		;

                MOV	ECX,3
                MOV	TEMP_FILE_HDR._CFH_N_OBJECTS,CX
                MOV	TEMP_FILE_HDR._CFH_SYMTBL_PTR,164
                MOV	TEMP_FILE_HDR._CFH_N_SYMBOLS,9
                ;
                ;Format Function By Ordinal Section Table
                ;
		MOV	EDI,OFF TEMP_SECT_HDR
                MOV	EDX,OFF FBO_SECTION_STUFF
                CALL    GENERATE_SECTION_HEADERS
                ;
                ;Format Function By Ordinal Raw Data
                ;
                MOV	WPTR [EDI],25FFh
                MOV	BPTR [EDI+6],2
		MOV	BPTR [EDI+10],8
		MOV	BPTR [EDI+14],6

                MOV	EAX,[EBX]._ENT_ORD
		OR	EAX,80000000h
                MOV	DPTR [EDI+16],EAX
                MOV	DPTR [EDI+20],EAX
                ADD	EDI,24

		JMP	FINISH_FUNCTION

COFF_IMPLIB_THIRD_PASS	ENDP


COMPARE_ALIAS   PROC
                MOV     ESI,EAX
                CMP     ECX,ALIAS_LENGTH
                JBE	L1$

                MOV     ECX,ALIAS_LENGTH
L1$:
                REPE	CMPSB
                RET
COMPARE_ALIAS   ENDP


FORMAT_INIT	PROC 	NEAR
		PUSHM	EBX
                ;
                ;Initialize accumulators/pointers
                ;
                XOR     EAX,EAX
                MOV     __IMP_INSERT,EAX
                MOV     GENOBJ1_INSERT,EAX
                MOV     GENOBJ2_INSERT,EAX
                MOV     ARCHIVES_N,EAX
                MOV     HELPER_STRINGS_N,3
                MOV	EAX,DPTR MODULE_NAME
                SUB	EAX,4
                MOV	EDX,EAX
                ADD	EDX,EAX
                MOV	MODNAME_LENGTH,EAX
                ADD	EDX,1 + (2 * SIZEOF GENOBJ1_CONST) + SIZEOF GENOBJ2_CONST + SIZEOF GENOBJ3_CONST
                MOV     HELPER_STRINGS_LENGTH,EDX

		CONVERT	EBX,EBX,ENTRYNAME_GARRAY
		ASSUME	EBX:PTR ENT_STRUCT

		MOV	ESI,[EBX]._ENT_INTERNAL_NAME_GINDEX
		CONVERT	ESI,ESI,SYMBOL_GARRAY
		ASSUME	ESI:PTR SYMBOL_STRUCT

                LEA     EAX,[ESI]._S_NAME_TEXT
		MOV	__IMP_SYM_PTR,EAX

                MOV	CHECK_INSERT_PROC,OFF CHECK_DLL_FIRST
                MOV	AL,MODULE_NAME+4
                CMP	AL,'N'
                JB      L5$
                JA	L2$

                MOV	EDI,OFF MODULE_NAME+4
                MOV	ECX,MODNAME_LENGTH
                MOV	ESI,OFF GENOBJ2_CONST
                CMP	ECX,SIZEOF GENOBJ1_CONST + SIZEOF GENOBJ2_CONST
                JB      L1$

                MOV	ECX,SIZEOF GENOBJ1_CONST + SIZEOF GENOBJ2_CONST
L1$:
		REPE	CMPSB
		JB	L5$

L2$:
                MOV	CHECK_INSERT_PROC,OFF CHECK_DLL_MIDDLE
                CMP	AL,'_'

                JB      L5$
                JA	L4$


                MOV	ECX,MODNAME_LENGTH
                MOV	EDI,OFF MODULE_NAME+4
                MOV	ESI,OFF __IMP_CONST
                CMP	ECX,4
                JB      L3$

                MOV	ECX,4
L3$:
		REPE	CMPSB
		JB	L5$

L4$:
                MOV	CHECK_INSERT_PROC,OFF CHECK_DLL_LAST
                ;
		;SEND Archive Signature RECORD !<arch>\n
                ;
L5$:
                MOV     DWORD PTR TEMP_RECORD,'ra<!'
                MOV    	DWORD PTR TEMP_RECORD+4,0A3E6863h	;\n,'>hc'
                MOV	EAX,OFF TEMP_RECORD
                MOV     ECX,8
                ;
                ;  [EAX](ECX)  [Source offset](Length)
                ;
		CALL	IMP_OUTALL

		XOR	EAX,EAX
		MOV	EDI,OFF COFF_HELPERS_STRUCTURE
		MOV	ECX,SIZE SEQ_STRUCT/4+ 1K*1K/PAGE_SIZE/4

		REP	STOSD
                ;
                ;   Format Archive Header in TEMP_RECORD
                ;
                MOV	EDI,OFF TEMP_RECORD
                MOV     ECX,(SIZE COFF_ARCHIVE_HDR)/ 4
                MOV     EAX,'    '

                REP     STOSD

                MOV     WPTR TEMP_CAH_EOH,0A60h                    ;'`\n'
                MOV     TEMP_CAH_NAME,'/'
                MOV     TEMP_CAH_MODE,'0'
                MOV     EAX,CURN_C_TIME
                MOV     ECX,OFF TEMP_CAH_DATE
                POPM	EBX
                JMP     CBTA32

FORMAT_INIT	ENDP


FORMAT_ONE	PROC	NEAR

                MOV     EAX,HELPER_STRINGS_N
                MOV	EDX,EAX
                XCHG 	DH,DL
                ROR	EDX,16
                XCHG 	DH,DL
                MOV	DWORDOUT,EDX
                LEA	EAX,DWORDOUT
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
		MOV	ECX,4
		CALL	STORE_EAXECX_EDX_SEQ           ;First field is helper 1 count
                ;
                ;Calculate Initial COFF File offsets
                ;
                ;Calculate size of helper one
                ;
                MOV	NEXT_CH1_OFFSET,4                 ;Past entry count dword

                MOV     EAX,HELPER_STRINGS_N
                ADD     EAX,EAX
                MOV	EDX,EAX			;EDX = size of CH2_Indices
                ADD     EAX,EAX
                ADD     EAX,4
;
                MOV     HELPER_STRINGS,EAX
                MOV	NEXT_HELPER_STRING,EAX

                ADD	EAX,HELPER_STRINGS_LENGTH
                CALL    STORE_ARCHIVE_SIZE
                INC	EAX
                AND	AL,0FEh
		MOV     ECX,EAX                 ;ECX = archive 1 size

                MOV     HELPER_TWO,ECX                     ;Start of second helper archive
		MOV	NEXT_CH2_OFFSET,ECX
		MOV	NEXT_CH2_INDEX,ECX
                ;
                ;Calculate size of helper two
                ;
		MOV	EAX,ARCHIVES_N
                ADD     EAX,3
                ADD     EAX,EAX
                ADD     EAX,EAX
                ADD     EAX,EDX

                ADD     EAX,8
                ADD     EAX,HELPER_STRINGS_LENGTH
                INC	EAX
                AND	AL,0FEh
                ;
                ADD	EAX,ECX
                ADD     EAX,8+2*SIZE COFF_ARCHIVE_HDR
      		MOV     GENOBJ1_FOFFSET,EAX

		MOV	EDX,MODNAME_LENGTH
                MOV     ECX,EDX
                ADD     ECX,ECX
                ADD     EAX,ECX
                INC     EDX
                AND     DL,0FEh
                ADD     EAX,EDX
		ADD     EAX,628
		MOV     GENOBJ2_FOFFSET,EAX

		ADD     EAX,186
		MOV     GENOBJ3_FOFFSET,EAX

                ADD     EAX,EDX
		ADD     EAX,208
		MOV     CURRENT_FOFFSET,EAX
                MOV     INITIAL_FOFFSET,EAX

		MOV	EDX,ARCHIVES_N
                ADD     EDX,3
                CALL    APPEND_CH2_OFFSET_EDX

                ADD     EDX,EDX
                ADD     EDX,EDX
		ADD	EDX,4
		ADD	NEXT_CH2_INDEX,EDX

                MOV     EDX,HELPER_STRINGS_N
                CALL    APPEND_CH2_INDEX_DX
                SUB     DX,DX
                CALL    APPEND_CH2_INDEX_DX

                MOV     EDX,GENOBJ1_FOFFSET
                CALL    APPEND_CH2_OFFSET_EDX

                MOV     EDX,GENOBJ2_FOFFSET
                CALL    APPEND_CH2_OFFSET_EDX

                MOV     EDX,GENOBJ3_FOFFSET
                JMP     APPEND_CH2_OFFSET_EDX

FORMAT_ONE      ENDP


FORMAT_TWO	PROC NEAR

                XOR	EDX,EDX
		CALL	CHECK_INSERT_PROC
                ;
                ;Insert generated object 3 into Helper Strings and Tables
                ;
                MOV     EAX,OFF MODULE_NAME+3
                MOV     BPTR [EAX],7Fh
                MOV     ECX,MODNAME_LENGTH
                INC	ECX
		MOV	EBX,NEXT_HELPER_STRING
		ADD	NEXT_HELPER_STRING,ECX
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
		CALL	STORE_EAXECX_EDXEBX_RANDOM
                MOV     BPTR MODULE_NAME+3,0

                MOV     ECX,OFF GENOBJ3_CONST
                CALL    APPEND_HELPER_STRING_ECX

                TEST    HELPER_STRINGS_LENGTH,1
                JZ      L1$

                MOV     BPTR DWORDOUT,0Ah
                LEA	EAX,DWORDOUT
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
		MOV	ECX,1
		MOV	EBX,NEXT_HELPER_STRING
		CALL	STORE_EAXECX_EDXEBX_RANDOM
L1$:
                MOV     DX,3
                CALL    APPEND_CH2_INDEX_DX

                MOV     EDX,GENOBJ3_FOFFSET
                CALL    APPEND_CH1_OFFSET_EDX
                ;
		;Generate Linker Helper One  (Unix Format) RECORD
                ;
		;EAX IS DESTINATION STRING
		;ECX IS BYTE COUNT TO READ
		;EDX IS DATA-TYPE STRUCTURE TO USE
		;EBX IS SOURCE ADDRESS
		;
                LEA	EAX,TEMP_FILE_HDR
                MOV	ECX,HELPER_TWO		;Write up to next record
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
                XOR	EBX,EBX
		CALL	READ_EAXECX_EDXEBX_RANDOM
                ;
                MOV	EAX,OFF TEMP_RECORD
                MOV	ECX,HELPER_TWO		;Write up to next record
                ADD     ECX,SIZEOF COFF_ARCHIVE_HDR
		CALL	IMP_OUTALL
                ;
		;Generate Linker Helper Two  (MS Format) RECORD
                ;
                LEA	EAX,TEMP_FILE_HDR
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
                MOV	EBX,HELPER_TWO
                MOV     ECX,NEXT_CH2_INDEX
		SUB	ECX,EBX
                PUSHM	ECX
		CALL	READ_EAXECX_EDXEBX_RANDOM

                POPM	ECX
                LEA	EAX,TEMP_FILE_HDR
                ADD	EAX,ECX
                MOV     EBX,HELPER_STRINGS
                MOV     ECX,HELPER_STRINGS_LENGTH
                INC     ECX
                AND     CL,0FEh
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
		CALL	READ_EAXECX_EDXEBX_RANDOM
;
                MOV     EAX,HELPER_STRINGS_N
                ADD     EAX,EAX
		MOV	EDX,ARCHIVES_N
                ADD     EDX,3
                ADD     EDX,EDX
                ADD     EDX,EDX
                ADD     EAX,EDX
                ADD     EAX,8
                ADD     EAX,HELPER_STRINGS_LENGTH

                PUSH	EAX
                CALL    STORE_ARCHIVE_SIZE
                POP	ECX

                INC	ECX
                AND	CL,0FEh
                MOV	EAX,OFF TEMP_RECORD
                ADD     ECX,SIZEOF COFF_ARCHIVE_HDR
		CALL	IMP_OUTALL
                ;
                ;Free all COFF_HELPER_STRUCTURE allocations
                ;
                LEA	ECX,COFF_HELPERS_STRUCTURE._SEQ_TABLE
                JMP	L3$
L2$:
		CALL	RELEASE_BLOCK
		ADD	ECX,4
L3$:
                XOR	EDX,EDX
		MOV	EAX,[ECX]
		MOV	[ECX],EDX
		TEST	EAX,EAX					;DOES THIS BLOCK EXIST?
		JNZ	L2$
		;
		;MOVE .DLL NAME to the Archive header
		;
                MOV     EDI,OFF TEMP_CAH_NAME
                CALL	MOVE_MODNAME
                MOV     DPTR [EDI],'lld.'
                MOV     BPTR [EDI+4],'/'
                ;
                ;Move Size to Archive Header
                ;
                MOV	EAX,MODNAME_LENGTH
                INC	EAX
                MOV	ECX,EAX
                INC     EAX
                AND     AL,0FEh
                ADD	ECX,ECX
                ADD	EAX,ECX
                ADD	EAX,566
                CALL    STORE_ARCHIVE_SIZE
                ;
                ;FORMAT COFF File and Optional Headers
                ;
                MOV	EDI,OFF TEMP_FILE_HDR
                MOV	EAX,MODNAME_LENGTH
		INC	EAX
                MOV     ECX,EAX
                ADD     EAX,EAX
                INC     ECX
                AND	CL,0FEh
                ADD     ECX,EAX
                ADD	ECX,120 + SIZE PEXE - PEXE._PEXE_CPU_REQUIRED + 7*SIZE COFF_SYMTBL_ENTRY
                SHR	ECX,2
                INC	ECX
                XOR	EAX,EAX
                REP     STOSD
		;
		;NOW DO PEXEHEADER
		;
		MOV	EDI,(OFF TEMP_FILE_HDR)-4
                ASSUME	EDI:PTR PEXE

		MOV	[EDI]._PEXE_CPU_REQUIRED,PEXE_80386
		MOV	[EDI]._PEXE_FLAGS,100h
                MOV     ECX,2
		MOV	[EDI]._PEXE_N_OBJECTS,CX
		MOV	EAX,CURN_C_TIME
		MOV	[EDI]._PEXE_TIME_DATE,EAX
                MOV	EAX,MODNAME_LENGTH
                ADD     EAX,6
                AND	AL,0FEh
                MOV     GOBJ1_SECT_RAW2_LEN,EAX
                ADD     EAX,374
		MOV	[EDI]._PEXE_RES1,EAX
		MOV	[EDI]._PEXE_RES2,7
		MOV	[EDI]._PEXE_HDR_XTRA,0e0h
		MOV	[EDI]._PEXE_MAJIC,10Bh
		MOV	[EDI]._PEXE_LMAJOR,RELEASE_NUM/256
		MOV	[EDI]._PEXE_LMINOR,RELEASE_NUM AND 0FFH
		MOV	[EDI]._PEXE_OBJECT_ALIGN,1000h
		MOV	[EDI]._PEXE_FILE_ALIGN,200h
		MOV	[EDI]._PEXE_OS_MAJOR,1
		MOV	[EDI]._PEXE_STACK_COMMIT,1000h
		MOV	[EDI]._PEXE_STACK_RESERVE,100000h
		MOV	[EDI]._PEXE_HEAP_RESERVE,100000h
		MOV	[EDI]._PEXE_HEAP_COMMIT,1000h
                ASSUME	EDI:NOTHING
                ;
                ;Format Generated Object One Section Table
                ;
		MOV	EDI,OFF TEMP_G1_SECT_HDR
                MOV	EDX,OFF GOBJ1_SECTION_STUFF
                CALL    GENERATE_SECTION_HEADERS
                ;
                ;Format Generated Object One Raw Data and Relocations
                ;
                ADD	EDI,5*4
		MOV	ESI,OFF GOB1_RELO
                MOVSD
                MOVSD
                MOVSD
                MOVSD
                MOVSD
                MOVSD
                MOVSD
                MOVSW
                CALL	MOVE_MODNAME
                MOV	DPTR [EDI],'lld.'
                ADD	EDI,6		;Pad AsciiZ w/Zero
                AND     DI,0FFFEh	;to EVEN alignment
                ;
                ;Format Generated Object One Symbol Table
                ;
		ASSUME	EDI:PTR COFF_SYMTBL_ENTRY

		MOV	[EDI]._CSE_NAME_DD2,4
		MOV	[EDI]._CSE_SECTION,1
		MOV	[EDI]._CSE_STORAGE_CLASS,2
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		MOV	[EDI]._CSE_NAME_DD1,'adi.'
		MOV	[EDI]._CSE_NAME_DD2,'2$at'
		MOV	[EDI]._CSE_VALUE,0C0000040h
		MOV	[EDI]._CSE_SECTION,1
		MOV	[EDI]._CSE_STORAGE_CLASS,68h
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		MOV	[EDI]._CSE_NAME_DD1,'adi.'
		MOV	[EDI]._CSE_NAME_DD2,'6$at'
		MOV	[EDI]._CSE_SECTION,2
		MOV	[EDI]._CSE_STORAGE_CLASS,3
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		MOV	[EDI]._CSE_NAME_DD1,'adi.'
		MOV	[EDI]._CSE_NAME_DD2,'4$at'
		MOV	[EDI]._CSE_VALUE,0C0000040h
		MOV	[EDI]._CSE_STORAGE_CLASS,68h
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		MOV	[EDI]._CSE_NAME_DD1,'adi.'
		MOV	[EDI]._CSE_NAME_DD2,'5$at'
		MOV	[EDI]._CSE_VALUE,0C0000040h
		MOV	[EDI]._CSE_STORAGE_CLASS,68h
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY
                MOV	EAX,MODNAME_LENGTH
                ADD	EAX,4+ SIZEOF GENOBJ1_CONST
		MOV	[EDI]._CSE_NAME_DD2,EAX
		MOV	[EDI]._CSE_STORAGE_CLASS,2
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

                ADD	EAX,SIZEOF GENOBJ1_CONST + SIZEOF GENOBJ2_CONST
		MOV	[EDI]._CSE_NAME_DD2,EAX
		MOV	[EDI]._CSE_STORAGE_CLASS,2
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY

		ASSUME	EDI:NOTHING
                ;
                ;Format Generated Object One String Table
                ;
                MOV	EAX,MODNAME_LENGTH
		INC	EAX
                ADD     EAX,EAX
                ADD	EAX,62
                MOV	[EDI],EAX		;Store String Table size
                ADD	EDI,4
                CALL	MOVE_MODNAME
		MOV	ESI,OFF GENOBJ1_CONST
                MOV     ECX,SIZEOF GENOBJ1_CONST
                REP	MOVSB
		MOV	ESI,OFF GENOBJ2_CONST
                MOV     ECX,SIZEOF GENOBJ1_CONST + SIZEOF GENOBJ2_CONST
                REP	MOVSB
                MOV	BPTR [EDI],7Fh
                INC	EDI
                CALL	MOVE_MODNAME
		MOV	ESI,OFF GENOBJ3_CONST
                MOV     ECX,SIZEOF GENOBJ3_CONST
                REP	MOVSB
                ;
		;SEND Archive Member Header,Generated Object One RECORD
                ;
		CALL	PUT_ARCHIVE
                ;
                ;
		;Generate Static Object Archive Two
                ;
                ;
                MOV	EDI,OFF TEMP_SECT_HDR
		MOV	ECX,32
                XOR	EAX,EAX
                REP     STOSD

                MOV	EAX,125
                CALL    STORE_ARCHIVE_SIZE

                MOV	TEMP_FILE_HDR._CFH_HDR_XTRA,0
                MOV	ECX,1
                MOV	TEMP_FILE_HDR._CFH_N_OBJECTS,CX
                MOV	TEMP_FILE_HDR._CFH_SYMTBL_PTR,50h
                MOV	TEMP_FILE_HDR._CFH_N_SYMBOLS,1
                ;
                ;Format Generated Object Two Section Table
                ;
		MOV	EDI,OFF TEMP_SECT_HDR
                MOV	EDX,OFF GOBJ2_SECTION_STUFF
                CALL    GENERATE_SECTION_HEADERS
                ;
                ;Format Generated Object Two Raw Data
                ;
                ADD	EDI,14h
                ;
                ;Format Generated Object Two Symbol Table
                ;
		ASSUME	EDI:PTR COFF_SYMTBL_ENTRY
		MOV	[EDI]._CSE_NAME_DD2,4
		MOV	[EDI]._CSE_SECTION,1
		MOV	[EDI]._CSE_STORAGE_CLASS,2
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY
		ASSUME	EDI:NOTHING
                ;
                ;Format Generated Object Two String Table
                ;
                MOV	BPTR [EDI],27		;Store String Table size
                ADD	EDI,4
		MOV	ESI,OFF GENOBJ2_CONST
                MOV     ECX,SIZEOF GENOBJ1_CONST + SIZEOF GENOBJ2_CONST
                REP	MOVSB
                MOV	BPTR [EDI],0Ah
                ;
		;SEND Archive Member Header,Generated Object Two RECORD
                ;
                MOV	EAX,OFF TEMP_RECORD
                MOV	ECX,186
		CALL	IMP_OUTALL
                ;
                ;
		;Generate Static Object Archive Three
                ;
		;
                MOV	EDI,OFF TEMP_SECT_HDR
		MOV	ECX,MODNAME_LENGTH
                ADD	ECX,148
                MOV	EDX,ECX
                INC	ECX
                SHR	ECX,2
                XOR	EAX,EAX
                REP     STOSD

                MOV	EAX,EDX
                CALL    STORE_ARCHIVE_SIZE

                MOV	ECX,2
                MOV	BPTR TEMP_FILE_HDR._CFH_N_OBJECTS,CL
                MOV	BPTR TEMP_FILE_HDR._CFH_SYMTBL_PTR,6Ch
                ;
                ;Format Generated Object Three Section Table
                ;
		MOV	EDI,OFF TEMP_SECT_HDR
                MOV	EDX,OFF GOBJ3_SECTION_STUFF
                CALL    GENERATE_SECTION_HEADERS
                ;
                ;Format Generated Object Three Raw Data and Relo
                ;
                ADD	EDI,8
                ;
                ;Format Generated Object Three Symbol Table
                ;
		ASSUME	EDI:PTR COFF_SYMTBL_ENTRY
		MOV	[EDI]._CSE_NAME_DD2,4
		MOV	[EDI]._CSE_SECTION,1
		MOV	[EDI]._CSE_STORAGE_CLASS,2
		ADD	EDI,SIZE COFF_SYMTBL_ENTRY
		ASSUME	EDI:NOTHING
                ;
                ;Format Generated Object Three String Table
                ;
                MOV	ECX,MODNAME_LENGTH
                ADD     ECX,1 + SIZEOF GENOBJ3_CONST
                MOV	[EDI],ECX		;Store String Table size
                ADD	DPTR [EDI],4		;include length dword in size
                ADD	EDI,5
                MOV	BPTR [EDI-1],7Fh
                CALL	MOVE_MODNAME
		MOV	ESI,OFF GENOBJ3_CONST
                MOV     ECX,SIZEOF GENOBJ3_CONST
                REP	MOVSB
                JMP	PUT_ARCHIVE

FORMAT_TWO	ENDP


GENERATE_SECTION_HEADERS PROC NEAR

		PUSHM	ESI
                MOV	ESI,EDX
L1:
		MOVSD				;Name 1-4
		MOVSD                           ;Name 5-8
                ADD	EDI,8                   ;Size and RVA are 0
                MOVSD                           ;Raw Size
                MOVSD                           ;Raw Ptr
                MOVSD                           ;Relocs Ptr
                ADD	EDI,4			;Linenum Ptr = 0
                MOVSD				;Relocs Cnt
                                                ;0 Linenum Cnt (MSW)
                MOVSD				;Flags

                DEC	ECX
		JNZ	L1

		POPM	ESI
		RET

GENERATE_SECTION_HEADERS ENDP


INSERT_FIRST_OBJ	PROC 	NEAR
                ;
                ;Insert generated object 1 into Helper Strings and Tables
                ; Preserve EAX, EBX, EDX, ESI
                PUSHM	EBX
                MOV     EAX,OFF MODULE_NAME+4
                MOV     ECX,MODNAME_LENGTH
		MOV	EBX,NEXT_HELPER_STRING
		ADD	NEXT_HELPER_STRING,ECX
		MOV	EDX,OFF COFF_HELPERS_STRUCTURE
		CALL	STORE_EAXECX_EDXEBX_RANDOM
                POPM	EBX

                MOV     ECX,OFF GENOBJ1_CONST
                CALL    APPEND_HELPER_STRING_ECX

                MOV     DX,1
                CALL    APPEND_CH2_INDEX_DX

                MOV     EDX,GENOBJ1_FOFFSET
                CALL    APPEND_CH1_OFFSET_EDX

                XOR	EDX,EDX
                MOV     DX,[EBX]._ENT_COFF_ARCHIVE_HINT
		RET

INSERT_FIRST_OBJ	ENDP


INSERT_SECOND_OBJ	PROC 	NEAR
                ;
                ;Insert generated object 2 into Helper Strings and Tables
                ; Preserve EAX, EBX, EDX, ESI
                ;
                MOV     ECX,OFF GENOBJ2_CONST
                CALL    APPEND_HELPER_STRING_ECX

                MOV     DX,2
                CALL    APPEND_CH2_INDEX_DX

                MOV     EDX,GENOBJ2_FOFFSET
                CALL    APPEND_CH1_OFFSET_EDX

                XOR	EDX,EDX
                MOV     DX,[EBX]._ENT_COFF_ARCHIVE_HINT
                RET

INSERT_SECOND_OBJ	ENDP


INSERT__IMP_	PROC 	NEAR
                ;
                ;Insert generated __imp_s into Helper Strings and Tables
                ; Preserve EAX, EBX, EDX, ESI
                ;
		PUSHM	EAX,EBX,EDX,ESI
                MOV     EAX,WM_PTR
                MOV     INSERT_SAVE_PTR,EAX
                MOV     EAX,WM_CNT
                MOV     INSERT_SAVE_CNT,EAX
                MOV     EAX,WM_BLK_PTR
                MOV     INSERT_SAVE_BLK_PTR,EAX

                MOV     EAX,INITIAL_FOFFSET
                MOV     INNER_FOFFSET,EAX
                ;
                ;Walk sorted export list for __imp_ entries
                ;
		CALL	TBLINIT_1
		JMP	L82$
L81$:
		CALL	COFF_IMPLIB_SECOND_PASS_INNER
L82$:
		CALL	TBLNEXT_1
		JNZ	L81$

                MOV     EAX,INSERT_SAVE_PTR
                MOV     WM_PTR,EAX
                MOV     EAX,INSERT_SAVE_CNT
                MOV     WM_CNT,EAX
                MOV     EAX,INSERT_SAVE_BLK_PTR
                MOV     WM_BLK_PTR,EAX
		POPM	ESI,EDX,EBX,EAX
		RET

INSERT__IMP_	ENDP


MOVE_MODNAME	PROC 	NEAR PRIVATE

                MOV     ESI,OFFSET MODULE_NAME+4
                MOV     ECX,MODNAME_LENGTH
		REP	MOVSB
                RET

MOVE_MODNAME	ENDP


PUT_ARCHIVE	PROC 	NEAR

                TEST	EDI,1
                JZ	L1$

                MOV	BPTR [EDI],0Ah
                INC	EDI
L1$:
                ;
		;SEND Archive Member Header,Object RECORD
                ;
                MOV	EAX,OFF TEMP_RECORD
                MOV	ECX,EDI
                SUB	ECX,EAX
		JMP	IMP_OUTALL

PUT_ARCHIVE	ENDP


STORE_ARCHIVE_SIZE	PROC	NEAR

                PUSHM	ECX,EDX,EAX
                MOV     ECX,'    '
                MOV     DPTR TEMP_CAH_SIZE,ECX
                MOV     DPTR TEMP_CAH_SIZE+4,ECX
                MOV     WPTR TEMP_CAH_SIZE+8,CX
                MOV     ECX,OFF TEMP_CAH_SIZE
                CALL    CBTA32
                POPM	EAX,EDX,ECX
                RET

STORE_ARCHIVE_SIZE	ENDP


TBLINIT_1 	PROC	NEAR PRIVATE
		;
		;
		;
		MOV	ECX,OFF EXP_BUFFER+8	;TABLE OF BLOCKS OF INDEXES
		MOV	EBX,EXP_BUFFER+4	;FIRST BLOCK
		MOV	WM_BLK_PTR,ECX		;POINTER TO NEXT BLOCK

		TEST	EBX,EBX
		JZ	L9$
		;
		MOV	ECX,PAGE_SIZE/4
		MOV	WM_PTR,EBX		;PHYSICAL POINTER TO NEXT INDEX TO PICK

		MOV	WM_CNT,ECX
		OR	AL,1
L9$:
		RET

TBLINIT_1 	ENDP


TBLNEXT_1 	PROC	NEAR PRIVATE
		;
		;GET NEXT SYMBOL INDEX IN EBX
		;
		MOV	EDX,WM_CNT
		MOV	ECX,WM_PTR

		DEC	EDX			;LAST ONE?
		JZ	L5$

		MOV	EBX,[ECX]		;NEXT INDEX
		ADD	ECX,4

		TEST	EBX,EBX
		JZ	L9$
;
		MOV	WM_PTR,ECX		;UPDATE POINTER
		MOV	WM_CNT,EDX		;UPDATE COUNTER

L9$:
		RET

L5$:
		;
		;NEXT BLOCK
		;
		MOV	EBX,[ECX]
		MOV	ECX,WM_BLK_PTR

		MOV	WM_CNT,PAGE_SIZE/4

		MOV	EDX,[ECX]
		ADD	ECX,4

		MOV	WM_PTR,EDX
		MOV	WM_BLK_PTR,ECX

		TEST	EBX,EBX

		RET


TBLNEXT_1 	ENDP

		.CONST
DBO_SECTION_STUFF	DB	'.idata$5'
                        DD	4,64h,0h,0,0c0301040h
                        DB	'.idata$4'
                        DD	4,068h,0,0,0c0301040h


FBO_SECTION_STUFF	DB	'.text',0,0,0
                        DD	6,8Ch,92h,1,060201020h
                        DB	'.idata$5'
                        DD	4,9Ch,0,0,0c0301040h
                        DB	'.idata$4'
                        DD	4,0A0h,0,0,0c0301040h


GENOBJ2_CONST	DB	'NULL'
GENOBJ1_CONST	DB	'_IMPORT_DESCRIPTOR',0
GENOBJ3_CONST	DB	'_NULL_THUNK_DATA',0
__IMP_CONST	DB	'__imp_'


GOB1_RELO	 	DD	0Ch,2
	                DW	7
	                DD	0,3
	                DW	7
	                DD	10h,4
	                DW	7

GOBJ2_SECTION_STUFF	DB	'.idata$3'
                        DD	14h,03Ch,0,0,0c0100040h

GOBJ3_SECTION_STUFF	DB	'.idata$5'
                        DD	4,64h,0,0,0c0300040h
                        DB	'.idata$4'
                        DD	4,68h,0,0,0c0300040h


endif

		END

