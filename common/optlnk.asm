		TITLE OPTLNK - Copyright (c) SLR Systems 1994

		INCLUDE MACROS
		INCLUDE	INITCODE
		INCLUDE	IO_STRUC
		INCLUDE	WIN32DEF
		INCLUDE	WINMACS

		PUBLIC	OPTLINK

		.DATA

		.CODE	ROOT_TEXT

		EXTERNDEF	_optlink:proc
		EXTERNDEF	_optlink_restartc:proc
		EXTERNDEF	_abort:proc
		EXTERNDEF	XCNOTIFY:PROC


;This is left here to cause optlnk.obj to be pulled in from the library
		public 	_optlink_restart
_optlink_restart	PROC
		jmp	_optlink_restartc
_optlink_restart	ENDP


		public	main
main		PROC
		INITCODE0
		call	_optlink
main		ENDP


OPTLINK		EQU	main


		END	OPTLINK

