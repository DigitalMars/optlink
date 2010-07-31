

#include "all.h"

void _end_of_indirect()
{
    struct MYI_STRUCT *EBX = IND_DEVICE;

    INDIRECT_MODE = 0;
    if (EBX)
    {
	void *EAX = EBX->MYI_BLOCK;
	EBX->MYI_BLOCK = 0;
	if (EAX)
	    _release_io_segment(EAX);
	EAX = EBX->MYI_HANDLE;
	EBX->MYI_HANDLE = 0;
	IND_DEVICE = 0;
	if (EAX)
	    _close_handle(EAX);
    }
}

