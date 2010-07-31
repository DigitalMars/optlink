		TITLE	SIZESEGS - Copyright (c) SLR Systems 1991

		INCLUDE MACROS
		INCLUDE	SECTS

if	any_overlays

		PUBLIC	SIZE_OVERLAY_SEGMENTS

		.DATA

	SOFT	EXTB	MODEL_FAR_BYTE,TRACK_BYTE,RELOAD_BYTE

	SOFT	EXTW	VECTOR_COUNT,VECTOR_SIZE,SECTION_NUMBER,SECTION_SIZE,RELOAD_DEPTH,OUTFILE_NUMBER

	SOFT	EXTD	SLR_VECTORS,SLR_SECTIONS,SLR_RELOAD_STACK,SLR_CODE,SLR_FNAMES

		.CODE	MIDDLE_TEXT

		ASSUME	DS:NOTHING

SIZE_OVERLAY_SEGMENTS	PROC
		;
		;DECLARE SIZES OF OVERLAY SEGMENTS BASED ON QUANTITIES KNOWN
		;AT THIS TIME
		;
		LDS	SI,SLR_VECTORS
		SYM_CONV_DS
		MOV	AX,VECTOR_COUNT
		MUL	VECTOR_SIZE		;SIZE OF VECTORS
		;
		;PLUS 5 BYTES FOR A FAR JUMP IF NEAR MODEL WITH RELOAD OR TRACK
		;
		CMP	MODEL_FAR_BYTE,'Y'
		JZ	1$
		CMP	TRACK_BYTE,'Y'
		JZ	11$
		CMP	RELOAD_BYTE,'Y'
		JNZ	1$
11$:
		ADD	AX,5			;THIS IS A FAR JUMP TO RELOAD CODE?
		ADC	DX,0
1$:
		MOV	[SI]._SM_LEN.LW,AX
		MOV	[SI]._SM_LEN.HW,DX

		LDS	SI,SLR_SECTIONS
		SYM_CONV_DS
		MOV	AX,SECTION_NUMBER
		MUL	SECTION_SIZE		;VARIABLE, SIZE OF SECTION
		MOV	[SI]._SM_LEN.LW,AX
		MOV	[SI]._SM_LEN.HW,DX

		LDS	SI,SLR_RELOAD_STACK
		MOV	CX,DS
		JCXZ	5$
		SYM_CONV_DS
		MOV	AX,6
		CMP	MODEL_FAR_BYTE,'Y'
		JZ	3$
		MOV	AL,4			;ONLY 4 BYTES PER IF NEAR CALLS...
3$:
		MUL	RELOAD_DEPTH
		MOV	[SI]._SM_LEN.LW,AX
		MOV	[SI]._SM_LEN.HW,DX
5$:
		LDS	SI,SLR_FNAMES
		SYM_CONV_DS
		MOV	AX,FNAME_SIZE
		MUL	OUTFILE_NUMBER
;		ADD	AX,SIZE SLRCODE_STRUCT
;		ADC	DX,0
		MOV	[SI]._SM_LEN.LW,AX
		MOV	[SI]._SM_LEN.HW,DX

		RET

SIZE_OVERLAY_SEGMENTS	ENDP

endif

		END

