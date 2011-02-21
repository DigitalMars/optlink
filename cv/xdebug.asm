		TITLE	XDEBUG - Copyright (c) SLR Systems 1994

		INCLUDE	MACROS

		PUBLIC	FLUSH_CV_TEMP

		.CODE	PASS2_TEXT

		externdef	_flush_cv_temp:proc


FLUSH_CV_TEMP	PROC
		push	EDI
		call	_flush_cv_temp
		add	ESP,4
		ret
FLUSH_CV_TEMP	ENDP


		END

