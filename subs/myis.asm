		TITLE	MYIS - Copyright (c) SLR Systems 1994

		INCLUDE MACROS
		INCLUDE	IO_STRUC

		PUBLIC	RELEASE_MYI_PTR


		.DATA

		EXTERNDEF	LAST_FILENAME_GINDEX:DWORD


		.CODE	PASS1_TEXT

		EXTERNDEF	_release_io_segment:proc,_close_handle:proc
		externdef	_release_myi_ptr:proc, _close_myi_handle:proc


RELEASE_MYI_PTR PROC
	push	EAX
	call	_release_myi_ptr
	add	ESP,4
	ret

		;
		;EAX IS MYI_STUFF
		;
		ASSUME	EAX:PTR MYI_STRUCT

		XOR	EDX,EDX
		MOV	ECX,[EAX].MYI_BLOCK

		MOV	[EAX].MYI_BLOCK,EDX
		MOV	[EAX].MYI_PTRA,EDX

		TEST	ECX,ECX
		JZ	L91$

		MOV	EAX,ECX

		push	ECX
		push	EDX
		push	EAX
		call	_release_io_segment
		add	ESP,4
		pop	EDX
		pop	ECX
L91$:
		RET

RELEASE_MYI_PTR ENDP



;		PUBLIC	CLOSE_MYI_HANDLE

CLOSE_MYI_HANDLE	PROC
	push	EAX
	call	_close_myi_handle
	add	ESP,4
	ret
		;
		;EAX IS MYI_STUFF
		;
		XOR	EDX,EDX
		MOV	ECX,[EAX].MYI_HANDLE

		MOV	[EAX].MYI_HANDLE,EDX
		MOV	LAST_FILENAME_GINDEX,EDX

		TEST	ECX,ECX
		JZ	L92$

		push	ECX
		call	_close_handle
		add	ESP,4

L92$:
		RET

CLOSE_MYI_HANDLE	ENDP

		END

