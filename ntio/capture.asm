		TITLE	CAPTURE - Copyright (C) 1994 SLR Systems

		INCLUDE	MACROS
		INCLUDE	WIN32DEF

		PUBLIC	CAPTURE_EAX,RELEASE_EAX,RELEASE_EAX_BUMP


		.CODE	ROOT_TEXT

		externdef	_capture_eax:proc
		externdef	_release_eax:proc
		externdef	_release_eax_bump:proc

		EXTERNDEF	ERR_ABORT:PROC,_do_dossleep_0:PROC

		EXTERNDEF	_TIMEOUT_ERR:ABS


CAPTURE_EAX	PROC
		push	ECX
		push	EDX

		push	EAX
		call	_capture_eax
		add	ESP,4

		pop	EDX
		pop	ECX
		ret
		;
		;
		;
		ASSUME	EAX:PTR GLOBALSEM_STRUCT

		LOCK	INC	[EAX]._SEM_COUNTER
		JZ	L1$

		MOV	EAX,[EAX]._SEM_ITSELF	;HANDLE FOR THIS SEMAPHORE
		JS	L1$

		TEST	EAX,EAX
		JZ	L1$

		PUSHM	EDX,ECX

		PUSH	-1			;WAIT FOREVER
		PUSH	EAX

		CALL	WaitForSingleObject

		POPM	ECX,EDX

		INC	EAX
		JZ	L9$			;ERROR
L1$:
		RET

L9$:
		MOV	AL,_TIMEOUT_ERR
		CALL	ERR_ABORT

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
		;
		;
		;
		LOCK	DEC	[EAX]._SEM_COUNTER
		JNS	L1$

		RET

L1$:
		MOV	EAX,[EAX]._SEM_ITSELF
		PUSH	EDX

		TEST	EAX,EAX
		JZ	L9$

		PUSHM	ECX,0			;I DON'T WANT PREVIOUS COUNT

		PUSH	1			;INCREASE COUNT BY 1
		PUSH	EAX

		CALL	ReleaseSemaphore

		POP	ECX
L9$:
		POP	EDX

		RET

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
		;
		;
		;
		LOCK	DEC	[EAX]._SEM_COUNTER
		JNS	L1$

		RET

L1$:
		MOV	EAX,[EAX]._SEM_ITSELF
		PUSH	EDX

		TEST	EAX,EAX
		JZ	L9$

		PUSHM	ECX,0			;I DON'T WANT PREVIOUS COUNT

		PUSH	1			;INCREASE COUNT BY 1
		PUSH	EAX

		CALL	ReleaseSemaphore

		CALL	_do_dossleep_0

		POP	ECX
L9$:
		POP	EDX

		RET

RELEASE_EAX_BUMP	ENDP


		END

