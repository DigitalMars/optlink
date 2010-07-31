		TITLE	GET_FN - Copyright (c) SLR Systems 1994

		INCLUDE	MACROS
		INCLUDE	IO_STRUC


		PUBLIC	GET_FILENAME

		.CODE	FILEPARSE_TEXT

		externdef	_get_filename:proc


GET_FILENAME	PROC
	push	EDX
	push	ECX
	push	EAX
	call	_get_filename
	add	ESP,12
	ret
GET_FILENAME	ENDP


		END


