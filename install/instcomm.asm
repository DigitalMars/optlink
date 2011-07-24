		TITLE	INST_INIT - Copyright (C) 1994 SLR Systems

		INCLUDE	MACROS

		PUBLIC	COMMON_INST_INIT


		.CODE	ROOT_TEXT

		externdef	_common_inst_init:proc


COMMON_INST_INIT	PROC
		push	ECX
		push	EAX
		call	_common_inst_init
		add	ESP,4
		pop	ECX
		ret
COMMON_INST_INIT	ENDP

		END

