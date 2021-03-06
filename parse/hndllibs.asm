		TITLE	HNDLLIBS - Copyright (c) SLR Systems 1994

		INCLUDE MACROS
		INCLUDE	IO_STRUC

		PUBLIC	DO_OBJS,DO_SRCNAM


		.DATA

		EXTERNDEF	FILNAM:NFN_STRUCT,SRCNAM:NFN_STRUCT

		EXTERNDEF	CURN_PLTYPE:BYTE,DEBUG_TYPES_SELECTED:BYTE

		EXTERNDEF	LISTTYPE:DWORD,FILE_HASH_MOD:DWORD,CURN_SECTION_GINDEX:DWORD

		EXTERNDEF	LIB_LIST:FILE_LISTS,OBJ_LIST:FILE_LISTS,LIBPATH_LIST:FILE_LISTS
		EXTERNDEF	STUBPATH_LIST:FILE_LISTS,OBJPATH_LIST:FILE_LISTS,STUB_LIST:FILE_LISTS
		EXTERNDEF	RC_LIST:FILE_LISTS,OLD_LIST:FILE_LISTS,LOD_LIST:FILE_LISTS

		EXTERNDEF	_FILE_LIST_GARRAY:STD_PTR_S


		.CODE	PHASE1_TEXT

		externdef	_do_srcnam:proc
		EXTERNDEF	_move_nfn:proc,_do_findfirst:proc,_do_findnext:proc,_filename_install:proc,ERR_INBUF_ABORT:PROC
		EXTERNDEF	LINK_TO_THREAD:PROC,_close_findnext:proc,WARN_ASCIZ_RET:PROC

		EXTERNDEF	DUP_SECTION_ERR:ABS,IMPROBABLE_RES_ERR:ABS

;		public _handle_libs
;_handle_libs	proc
;		mov	EAX,4[ESP]
;_handle_libs	endp

		PUBLIC	HANDLE_LIBS

HANDLE_LIBS	PROC
		;
		;EAX IS NFN_STRUCT
		;
		ASSUME	EAX:PTR NFN_STRUCT

		TEST	BPTR [EAX].NFN_FLAGS,MASK NFN_PRIM_SPECIFIED+MASK NFN_EXT_SPECIFIED
		JNZ	DO_SEARCH_LIBRARY
		MOV	ECX,[EAX].NFN_PRIMLEN
		ADD	ECX,[EAX].NFN_EXTLEN
		SUB	[EAX].NFN_TOTAL_LENGTH,ECX
		MOV	ECX,[EAX].NFN_TOTAL_LENGTH
		XOR	EDX,EDX

		MOV	[EAX].NFN_PRIMLEN,EDX
		MOV	[EAX].NFN_EXTLEN,EDX
		MOV	DPTR [EAX+ECX].NFN_TEXT,EDX
		JMP	STORE_LIBPATH

HANDLE_LIBS	ENDP


DO_SRCNAM	PROC
		;
		;EAX IS NFN_STRUCT
		;
		MOV	ECX,EAX
		MOV	EAX,OFF SRCNAM
		push	ECX
		push	EAX
		call	_move_nfn
		add	ESP,8
		RET

DO_SRCNAM	ENDP


;		public _do_search_library
;_do_search_library	proc
;		mov	EAX,4[ESP]
;_do_search_library	endp

		PUBLIC	DO_LIBRARY,DO_SEARCH_LIBRARY

DO_LIBRARY	LABEL	PROC

DO_SEARCH_LIBRARY	LABEL	PROC

		MOV	[EAX].NFN_TYPE,NFN_LIB_TTYPE
		MOV	ECX,OFF LIB_LIST
		JMP	DO_OBJS_1


DO_OBJS 	PROC
		;
		;IF NOT NUL, ADD NEW TO LIST...
		;
		MOV	ECX,OFF OBJ_LIST
		JMP	DO_OBJS_1

;		public _do_objs_1
;_do_objs_1	LABEL	PROC
;		mov	EAX,4[ESP]
;		mov	ECX,8[ESP]

DO_OBJS_1	LABEL	PROC
		;
		;EAX IS NFN_STRUCT
		;ECX IS FILE_LIST TO PUT IT IN
		;
		MOV	DH,[EAX].NFN_FLAGS
		XOR	DL,DL
		AND	DH,MASK NFN_AMBIGUOUS
		JNZ	L5$
		RESS	THIS_AMBIGUOUS,DL	;CURRENTLY UNAMBIGUOUS
		CALL	NOT_AMBIGUOUS
		RET

L5$:
		;
		;OK, NAME IN SI IS AMBIGUOUS...
		;
		PUSHM	EDI,ESI
		DEC	EDX
		MOV	EDI,ECX		;LISTTYPE
		MOV	ESI,EAX		;NOPE, GOT WHOLE FILENAME

		SETT	THIS_AMBIGUOUS,DL	;OOPS...
		push	EAX
		CALL	_do_findfirst	;EAX IS NFN_STRUCT
		add	ESP,4
		test	EAX,EAX
		jz	L11$
		;JC	L11$		;ALL DONE, NOT FOUND...
L1$:
		MOV	EAX,ESI
		MOV	ECX,EDI
		CALL	NOT_AMBIGUOUS
		MOV	EAX,ESI
		push	EAX
		CALL	_do_findnext
		add	ESP,4
		test	EAX,EAX
		jnz	L1$
		;JNC	L1$
		call	_close_findnext
L11$:
		POPM	ESI,EDI
		RET

DO_OBJS		ENDP


COPY_FIRST_OBJ:
		CMP	EDI,OFF OBJ_LIST
		JNZ	COPY_FIRST_OBJ_RET
		push	ECX
		push	EAX
		call	_do_srcnam
		add	ESP,8
		MOV	EAX,ESI
		JMP	COPY_FIRST_OBJ_RET

		public	_not_ambiguous
_not_ambiguous	proc
		mov	EAX,4[ESP]
		mov	ECX,8[ESP]
_not_ambiguous	endp

NOT_AMBIGUOUS	PROC
		;
		;EAX IS NFN_STRUCT
		;ECX IS FILE_LIST
		;
		PUSHM	EDI,ESI
		MOV	EDX,SRCNAM.NFN_PRIMLEN
		MOV	EDI,ECX
		MOV	ESI,EAX
		TEST	EDX,EDX
		JZ	COPY_FIRST_OBJ
COPY_FIRST_OBJ_RET::

		push	ECX
		mov	ECX,ESP
		push	ECX
		push	EAX
		call	_filename_install
		add	ESP,8
		pop	ECX
		;CALL	FILENAME_INSTALL;RETURNS ECX AS SYMBOL ADDRESS
					;EAX IS GINDEX
		ASSUME	ECX:PTR FILE_LIST_STRUCT

		MOV	EDX,EAX
		;
		;ADD TO LIST IF NOT ALREADY THERE...
		;DS:BX IS SYMBOL, DX IS LOGICAL ADDR
		;
		GETT	AH,DOING_NODEF
		PUSH	EBX
		MOV	AL,[ECX].FILE_LIST_FLAGS
		TEST	AH,AH
		JNZ	L15$
		TEST	EAX,MASK MOD_IGNORE	;MODEFAULT, IGNORE
		JNZ	L9$
		TEST	EAX,MASK MOD_ADD
		JNZ	L2$		;ALREADY IN ADD LIST...
		;

		GETT	AH,THIS_AMBIGUOUS
		OR	AL,MASK MOD_ADD
		TEST	AH,AH
		JNZ	L21$
if	any_overlays
		TEST	EAX,MASK MOD_UNAMBIG
		JZ	L20$
		CMP	[ECX].FILE_LIST_SECTION_GINDEX,0
		JZ	L20$
		MOV	CL,DUP_SECTION_ERR
		CALL	ERR_INBUF_ABORT

L20$:
endif
		OR	AL,MASK MOD_UNAMBIG
L21$:
		MOV	[ECX].FILE_LIST_FLAGS,AL

PUT_IN_ADD_LIST:
		;
		;ESI IS NFN_STRUCT
		;EDI IS LISTTYPE
		;EDX IS CURN FILE_LIST_GINDEX
		;ECX IS CURN FILE_LIST_ADDRESS
		;
		ASSUME	EDI:PTR FILE_LISTS
if	any_overlays
		MOV	EAX,CURN_SECTION
		MOV	BL,CURN_PLTYPE
		MOV	[ECX].FILE_LIST_SECTION,EAX
		MOV	[ECX].FILE_LIST_PLTYPE,BL
endif
		MOV	BL,DEBUG_TYPES_SELECTED
		MOV	EAX,[EDI].FILE_LAST_GINDEX
		OR	[ECX].FILE_LIST_PLINK_FLAGS,BL
		MOV	[EDI].FILE_LAST_GINDEX,EDX

		CONVERT	EAX,EAX,_FILE_LIST_GARRAY
		ASSUME	EAX:PTR FILE_LIST_STRUCT
		MOV	[EAX].FILE_LIST_NEXT_GINDEX,EDX	;CAUSE THIS IS NOT THREAD
								;ORDER, THIS IS LOGICAL ORDER
if	fgh_inthreads
		GETT	AL,_HOST_THREADED
		GETT	BL,OBJS_DONE

		AND	AL,BL
		JZ	L9$

		MOV	EAX,EDX

		MOV	OBJ_LIST.FILE_LAST_GINDEX,EAX
		CALL	LINK_TO_THREAD
endif

L9$:
		POPM	EBX,ESI,EDI
		RET

L15$:
		POP	EBX
		OR	AL,MASK MOD_IGNORE
		POP	ESI
		MOV	[ECX].FILE_LIST_FLAGS,AL
		POP	EDI
		RET

L2$:
		ASSUME	EDI:PTR FILE_LISTS
		;
		;ALREADY THERE, WAS IT UNAMBIGUOUS?
		;
		TEST	EAX,MASK MOD_UNAMBIG
		MOV	BL,THIS_AMBIGUOUS
		JNZ	L9$		;YES, IGNORE.
		;
		;WAS AMBIGUOUS, WHAT ABOUT NOW?
		;
		TEST	BL,BL
		JNZ	L9$		;STILL AMBIGUOUS
		;
		;NOW UNAMBIGUOUS, MOVE IT TO END OF LINKED LIST...
		;
		OR	AL,MASK MOD_UNAMBIG
		;
		;FIRST, IF .FILE_LAST_ITEM MATCHES, SKIP SEARCH, JUST FLAG IT..
		;

		CMP	[EDI].FILE_LAST_GINDEX,EDX
		MOV	[ECX].FILE_LIST_FLAGS,AL
		JZ	L9$
L3$:
		;
		;SEARCH LIST FOR IT (DX:BX)
		;
		PUSH	EDI
		MOV	EAX,[EDI].FILE_FIRST_GINDEX
L4$:
		CONVERT	EDI,EAX,_FILE_LIST_GARRAY
		ASSUME	EDI:PTR FILE_LIST_STRUCT

		MOV	EAX,[EDI].FILE_LIST_NEXT_GINDEX
		CMP	EAX,EDX
		JNZ	L4$
		;
		;MAKE [DI:CX] POINT TO [DS:SI]
		;
		XOR	EAX,EAX
		MOV	EBX,[ECX].FILE_LIST_NEXT_GINDEX
		MOV	[ECX].FILE_LIST_NEXT_GINDEX,EAX
		MOV	[EDI].FILE_LIST_NEXT_GINDEX,EBX
		POP	EDI
		;
		;NOW, MAKE .LAST PT TO THIS...
		;
		;DS:BX MUST BE SYMBOL, DX=LOGICAL
		;
		JMP	PUT_IN_ADD_LIST

NOT_AMBIGUOUS 	ENDP



		public _store_libpath
_store_libpath	proc
		mov	EAX,4[ESP]
_store_libpath	endp

STORE_LIBPATH	PROC
		;
		;EAX IS NFN_STRUCT
		;
		MOV	DL,2
		MOV	ECX,OFF LIBPATH_LIST
LNI_PATHS::
		ASSUME	EAX:PTR NFN_STRUCT
		CMP	[EAX].NFN_PATHLEN,0		;DONT ADD NUL PATH
		MOV	DH,0
		JZ	L9$
LNI_FILES::
		MOV	[EAX].NFN_TYPE,DH
		MOV	BPTR FILE_HASH_MOD,DL
		CALL	DO_OBJS_1
		XOR	EAX,EAX
		MOV	FILE_HASH_MOD,EAX
L9$:
		RET

STORE_LIBPATH	ENDP


		PUBLIC	HANDLE_OBJPATHS

HANDLE_OBJPATHS	PROC
		;
		;EAX IS NFN_STRUCT
		;
		MOV	DL,3
		MOV	ECX,OFF OBJPATH_LIST
		JMP	LNI_PATHS

HANDLE_OBJPATHS	ENDP


if	fg_segm OR fg_pe

		PUBLIC	HANDLE_STUBPATHS,HANDLE_STUB,HANDLE_OLD,HANDLE_RCS,HANDLE_LOD

HANDLE_STUBPATHS	PROC
		;
		;EAX IS NFN_STRUCT
		;
		;PARSING PATH= VARIABLE
		;
		MOV	DL,4
		MOV	ECX,OFF STUBPATH_LIST
		JMP	LNI_PATHS

HANDLE_STUBPATHS	ENDP

HANDLE_STUB	PROC
		;
		;EAX IS NFN_STRUCT
		;
		MOV	EDX,5+256*NFN_STUB_TTYPE
		MOV	ECX,OFF STUB_LIST
		JMP	LNI_FILES

HANDLE_STUB	ENDP

HANDLE_OLD	PROC
		;
		;EAX IS NFN_STRUCT
		;
		MOV	EDX,6+256*NFN_OLD_TTYPE
		MOV	ECX,OFF OLD_LIST
		JMP	LNI_FILES

HANDLE_OLD	ENDP

HANDLE_RCS	PROC
		;
		;EAX IS NFN_STRUCT
		;
		MOV	ECX,[EAX].NFN_PATHLEN
		MOV	EDX,[EAX].NFN_PRIMLEN

		ADD	ECX,EDX

		MOV	ECX,DPTR [EAX+ECX].NFN_TEXT

		CMP	ECX,'SER.'
		JZ	L1$

		CMP	ECX,'ser.'
		JNZ	L5$
L1$:
		MOV	EDX,7+NFN_RES_TTYPE*256
		MOV	ECX,OFF RC_LIST
		JMP	LNI_FILES

L5$:
		PUSH	EAX
		LEA	ECX,[EAX].NFN_TEXT

		MOV	AL,IMPROBABLE_RES_ERR
		CALL	WARN_ASCIZ_RET

		POP	EAX
		JMP	L1$

HANDLE_RCS	ENDP


		public _handle_lod
_handle_lod	proc
		mov	EAX,4[ESP]
_handle_lod	endp

HANDLE_LOD	PROC
		;
		;EAX IS NFN_STRUCT
		;
		MOV	EDX,8+NFN_LOD_TTYPE*256
		MOV	ECX,OFF LOD_LIST
		JMP	LNI_FILES

HANDLE_LOD	ENDP

endif

		END

