
#include <windows.h>

#include "all.h"

extern MYO_STRUCT *GET_OUTPUT_DEVICE();
extern void _err_file_list_abort(unsigned char errnum, FILE_LIST_STRUCT *ECX);


/***************************
 * FILE ALREADY EXISTS, JUST RESTORE POINTERS
 *
 * RETURN EAX = MYO_STRUCT
 */

MYO_STRUCT *REOPEN_OUTPUT(OUTFILE_STRUCT *ESI)
{

    MYO_STRUCT* EAX = GET_OUTPUT_DEVICE();

    EAX->MYO_PHYS_ADDR = ESI->_OF_PHYS_ADDR;
    EAX->MYO_FILE_LIST_GINDEX = ESI->_OF_FILE_LIST_GINDEX;
    EAX->MYO_HANDLE = ESI->_OF_HANDLE;

    if (ESI->_OF_HANDLE)
	return EAX;

    MYO_STRUCT* EBX = EAX;

    // REALLY NEED TO REOPEN IT...
    FILE_LIST_STRUCT *EDI = ESI->_OF_FILE_LIST_GINDEX;

    do
    {
	EAX = CreateFile((char *)EDI->FILE_LIST_NFN.NFN_TEXT,
	    GENERIC_WRITE,
	    0,				// deny all
	    0,				// security attributes
	    OPEN_EXISTING,
	    FILE_FLAG_RANDOM_ACCESS,
	    0);				// attribute file

	if (EAX != INVALID_HANDLE_VALUE)
	{
	    EBX->MYO_HANDLE = EAX;
	    ESI->_OF_HANDLE = EAX;

	    SetFilePointer(EAX,		// handle
		ESI->_OF_PHYS_ADDR,	// distance low
		0,			// distance high
		FILE_BEGIN);
	   return EAX;
	}
    } while (!_recover_handle());

    _err_file_list_abort(CANT_REOPEN_ERR, EBX->MYO_FILE_LIST_GINDEX);
}

