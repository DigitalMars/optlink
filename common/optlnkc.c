
#include <windows.h>

#include "all.h"

void LNKINIT();
void PASS1_STUFF();
void MIDDLE_STUFF();
void PASS2_STUFF();

void _optlink()
{
    LNKINIT();

    // command lines all read, ready to go...
    // start at head of OBJ_LIST and process OBJ_FILES

    PASS1_STUFF();		// reads objs and libs

    MIDDLE_STUFF();		// org segs and publics, etc

    PASS2_STUFF(); 		// fixup and relocate

    // NEED TO WAIT FOR ANY OUTPUT THREADS TO FINISH...

    // WAIT FOR MAP THREAD TO FINISH

    if (_HOST_THREADED)
    {
	if (!MAP_THREAD_HANDLE)
	    goto L7;
	WaitForSingleObject(MAP_THREAD_HANDLE, -1);
	_close_handle(MAP_THREAD_HANDLE);
	MAP_THREAD_HANDLE = 0;
    }
    if (MAPFILE_GINDEX)
    {
	// FLUSH LAST BUFFER OF MAP FILE
	MAP_FLUSHED = 0xFF;
	_flush_trunc_close(MAP_DEVICE);
    }
L7: ;
    _NO_CANCEL_FLAG = 0xFF;
    _abort();
}

void _optlink_restartc()
{
    _err_abort(FATAL_RESTART_ERR);
}

void CLEAN_UP()
{
    CLEAN_UP_IN_PROGRESS = 0xFF;
}
