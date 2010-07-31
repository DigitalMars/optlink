		TITLE	PUTDATA - Copyright (c) SLR Systems 1994

		INCLUDE	MACROS
		INCLUDE	IO_STRUC


		PUBLIC	LST_OUT


		.CODE	PASS2_TEXT

		EXTERNDEF	_imp_out:proc
		EXTERNDEF	_lst_out:proc



if	fg_segm
		PUBLIC	IMP_OUT

IMP_OUT		LABEL	PROC
	push	ECX
	push	EAX
	call	_imp_out
	add	ESP,8
	ret

endif

LST_OUT		LABEL	PROC
	push	ECX
	push	EAX
	call	_lst_out
	add	ESP,8
	ret

PUT_DATA	PROC
PUT_DATA	ENDP


		END

