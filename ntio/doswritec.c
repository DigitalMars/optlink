
#include <windows.h>

#include "all.h"

/*********************
 * Write block.
 *	EAX	device
 *	ECX	# of bytes
 *	EDX	buffer
 */

#if 0
DO_DOSWRITE_CLEAR(MYO_STRUCT* EAX, unsigned ECX, void* EDX)
{
    EAX->MYO_SPEC_FLAGS |= F_CLEAR_BLOCK;
    return _do_doswrite2(EAX, ECX, EDX);
}
#endif

MYO_STRUCT *_do_doswrite_release(MYO_STRUCT* EAX, unsigned ECX, void* EDX)
{
    EAX->MYO_SPEC_FLAGS |= F_RELEASE_BLOCK;
    return _do_doswrite2(EAX, ECX, EDX);
}


MYO_STRUCT *_do_doswrite(MYO_STRUCT* EAX, unsigned ECX)
{
    return _do_doswrite2(EAX, ECX, EAX->MYO_BLOCK);
}

MYO_STRUCT *_do_doswrite2(MYO_STRUCT* ESI, unsigned ECX, void* EDX)
{
    // f_seek_first	;do dosposition please
    // cx <>0 	;write stuff
    // f_trunc_file	;truncate file at this point
    // f_clear_block	;clear block used
    // f_release_block;release block used
    // f_set_time	;set file time-date stamp
    // f_close_file	;close file now

    if (ECX)
	ESI->MYO_BLOCK = EDX;

    // # of bytes being written
    ESI->MYO_BYTES = ECX;

    if (ESI->MYO_SPEC_FLAGS & F_SEEK_FIRST)
    {
	// first, seek to correct location in file

	ECX = ESI->MYO_DESPOT;
	if (ECX != ESI->MYO_PHYS_ADDR)
	{
	    ESI->MYO_PHYS_ADDR = ECX;
	    if (SetFilePointer(ESI->MYO_HANDLE, ECX, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	    {
		_dos_fail_move(ESI);
	    }
	}
    }

    if (ESI->MYO_BLOCK)
    {
	_doswrite(ESI, ESI->MYO_BYTES, ESI->MYO_BLOCK);
    }

    if (ESI->MYO_SPEC_FLAGS & F_TRUNC_FILE)
    {
	if (SetEndOfFile(ESI->MYO_HANDLE) == 0)
	    _dos_fail_seteof(ESI);
    }

    if (ESI->MYO_SPEC_FLAGS & (F_CLEAR_BLOCK | F_RELEASE_BLOCK))
    {
	void *p = ESI->MYO_BLOCK;
	ESI->MYO_BLOCK = NULL;

	if (ESI->MYO_SPEC_FLAGS & F_RELEASE_BLOCK && p)
	    _release_io_segment(p);
    }

    // eof?
    if (ESI->MYO_SPEC_FLAGS & F_CLOSE_FILE)
    {
	HANDLE EAX = ESI->MYO_HANDLE;
	ESI->MYO_BUSY = 0;
	if (EAX)
	{
	    ESI->MYO_HANDLE = 0;
	    _close_handle(EAX);
	}
    }
    ESI->MYO_SPEC_FLAGS = 0;
    return ESI;
}

