		TITLE	CAPTURE - Copyright (C) 1994 SLR Systems

		INCLUDE	MACROS
		INCLUDE	WIN32DEF

		PUBLIC	CAPTURE_EAX,RELEASE_EAX,RELEASE_EAX_BUMP


		.CODE	ROOT_TEXT

		externdef	_capture_eax:proc
		externdef	_release_eax:proc
		externdef	_release_eax_bump:proc

CAPTURE_EAX	PROC
		push	ECX
		push	EDX

		push	EAX
		call	_capture_eax
		add	ESP,4

		pop	EDX
		pop	ECX
		ret
CAPTURE_EAX	ENDP


RELEASE_EAX	PROC
		push	ECX
		push	EDX

		push	EAX
		call	_release_eax
		add	ESP,4

		pop	EDX
		pop	ECX
		ret
RELEASE_EAX	ENDP


RELEASE_EAX_BUMP	PROC
		push	ECX
		push	EDX

		push	EAX
		call	_release_eax_bump
		add	ESP,4

		pop	EDX
		pop	ECX
		ret
RELEASE_EAX_BUMP	ENDP


		END

