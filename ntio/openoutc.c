
#include <windows.h>

#include "all.h"

MYO_STRUCT* GET_OUTPUT_DEVICE()
{
	// return eax as next available myo_stuff
	MYO_STRUCT* EAX;

	for (EAX = &MYO_STUFF[0]; EAX->MYO_BUSY; EAX++)
	    ;
	memset(EAX, sizeof(MYO_STRUCT), 0);
	EAX->MYO_BUSY = -1;
	return EAX;
}

MYO_STRUCT *_open_output(FILE_LIST_STRUCT *EDI)
{
    // eax is file_list_gindex
    // returns eax = myo_struct

    // get available device
    MYO_STRUCT *EBX = GET_OUTPUT_DEVICE();

    EBX->MYO_FILE_LIST_GINDEX = EDI;
    HANDLE EAX;
    while (1)
    {
	EAX = CreateFile((char *)EDI->FILE_LIST_NFN.NFN_TEXT,
		GENERIC_WRITE, 0, 0,
		FORCE_CREATE ? CREATE_NEW : OPEN_ALWAYS,
		(EDI->FILE_LIST_FLAGS & FLF_RANDOM)
			?  FILE_FLAG_RANDOM_ACCESS
			: FILE_FLAG_SEQUENTIAL_SCAN,
		0);

	if (EAX != INVALID_HANDLE_VALUE)
		break;

	if (_recover_handle())
	    goto CANT_CREATE;
    }

    EBX->MYO_HANDLE = EAX;
    return EBX;

CANT_CREATE:
    // flush and stop map output
    _FLUSH_DISABLE_MAPOUT();
    _err_file_list_abort(CANT_CREATE_ERR, EBX->MYO_FILE_LIST_GINDEX);
    return 0;
}
