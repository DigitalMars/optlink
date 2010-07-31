
// Copyright (C) SLR Systems 1994

#include <windows.h>

#include "all.h"

void _close_handle(void *handle);

/******************
 * Returns:
 *	0	ok
 *	!=0	error
 */

int _recover_handle()
{
    // DID WE FAIL FROM LACK OF HANDLES?
    int i = GetLastError();
    if (i != 4)		// OUT OF HANDLES?
	return 1;	// NO, DIFFERENT ERROR, ABORT

    if (!(HANDLES_EXPANDED & 0xFF))
    {
	HANDLES_EXPANDED = 0xFF;
	SetHandleCount(255);
	return 0;
    }

    struct LIBRARY_STRUCT *EAX = LAST_LIB_GINDEX;
    while (EAX)		// while more libraries
    {
	if (EAX != CURNLIB_GINDEX)
	{
	    void *h = EAX->_LS_HANDLE;
	    if (h)
	    {   // CLOSE IF OPEN
		EAX->_LS_HANDLE = 0;
		_close_handle(h);
		return 0;
	    }
	}
	EAX = EAX->_LS_PREV_LIB_GINDEX;	// MORE LIBS?
    }
    return 1;
}

