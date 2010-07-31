		TITLE	LIBRARY - Copyright (c) SLR Systems 1994

		INCLUDE MACROS
		INCLUDE	IO_STRUC
		INCLUDE	SYMBOLS
		INCLUDE	LIBRARY
		INCLUDE	WIN32DEF

		PUBLIC	PROCESS_LIBRARIES,CLOSE_LIB_FILES
		PUBLIC	REFERENCE_LIBSYM


		.CODE	PASS1_TEXT

		externdef	_process_libraries:proc
		externdef	_reference_libsym:proc
		externdef	_close_lib_files:proc


PROCESS_LIBRARIES	PROC
		jmp	_process_libraries
PROCESS_LIBRARIES	ENDP

REFERENCE_LIBSYM	PROC
		push	ECX
		push	EAX
		push	ECX
		push	EAX
		call	_reference_libsym
		add	ESP,8
		pop	EAX
		pop	ECX
		ret
REFERENCE_LIBSYM	ENDP


CLOSE_LIB_FILES PROC
		jmp	_close_lib_files
CLOSE_LIB_FILES ENDP

		END

