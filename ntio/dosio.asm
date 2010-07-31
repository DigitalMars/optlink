		TITLE DOSIO - Copyright (c) 1994 by SLR Systems

		INCLUDE MACROS

		INCLUDE	IO_STRUC
		INCLUDE	EXES
		INCLUDE	WINMACS
		INCLUDE	WIN32DEF


		PUBLIC	MESOUT,ERR_ABORT,AX_MESOUT,LOUTALL_CON

		EXTERNDEF	CRLF:BYTE


		.DATA

		.CODE	ROOT_TEXT

		externdef	_cancel_terminate:proc
		externdef	_critical_error:proc
		externdef	_cerr_abort:proc
		externdef	_err_abort:proc
		externdef	_abort:proc
		externdef	__exit:proc
		externdef	_do_dossleep_0:proc
		externdef	_capture_eax:proc
		externdef	_release_eax:proc
		externdef	_release_eax_bump:proc
		externdef	_dos_fail_read:proc
		externdef	_dos_fail_write:proc
		externdef	_dos_fail_close:proc
		externdef	_dos_fail_move:proc
		externdef	_dos_fail_seteof:proc
		externdef	_close_handle:proc
		externdef	_loutall_con:proc
		externdef	_mesout:proc
		externdef	_delete_exe:proc
		externdef	_doswrite:proc
		externdef	_dosread:proc
		externdef	_dosposition_i:proc
		externdef	_release_block:proc
		externdef	_read_16k_threaded:proc
		externdef	_err_nfn_abort:proc
		externdef	_err_file_list_abort:proc
		externdef	_n_nonres_table:proc
		externdef	_n_res_table:proc

ERR_ABORT	PROC
		push	EAX
		call	_err_abort
ERR_ABORT	endp

MESOUT		PROC
		push	EAX
		call	_mesout
		add	ESP,4
		ret
MESOUT		ENDP


AX_MESOUT	EQU	(MESOUT)


LOUTALL_CON	PROC
		;EAX IS POINTER, ECX IS BYTE COUNT, DO IT...
		push	EAX
		push	ECX
		call	_loutall_con
		add	ESP,8
		ret
LOUTALL_CON	ENDP

		END

