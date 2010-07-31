		TITLE	LOCAL_STORAGE - Copyright (C) 1994 SLR Systems

		INCLUDE	MACROS

		PUBLIC	_release_local_storage
		PUBLIC	RELEASE_LOCAL_STORAGE,INIT_LOCAL_STORAGE


		.DATA

		EXTERNDEF	LNAME_STUFF:ALLOCS_STRUCT


		.CODE	ROOT_TEXT

		EXTERNDEF	_release_minidata:proc
		EXTERNDEF	_release_minidata:proc,INIT_ARRAYS:PROC


_release_local_storage	PROC
		mov	EAX,OFF LNAME_STUFF
		push	EAX
		call	_release_minidata
		add	ESP,4
		ret
_release_local_storage	ENDP

RELEASE_LOCAL_STORAGE	PROC
		;
		;
		;
		MOV	EAX,OFF LNAME_STUFF
		push	EAX
		call	_release_minidata
		add	ESP,4
		RET

RELEASE_LOCAL_STORAGE	ENDP


INIT_LOCAL_STORAGE	PROC
		;
		;
		;
		JMP	INIT_ARRAYS
;		RET

INIT_LOCAL_STORAGE	ENDP


		END

