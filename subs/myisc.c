
#include "all.h"

void _release_myi_ptr(MYI_STRUCT *EAX)
{
    void *ECX = EAX->MYI_BLOCK;
    EAX->MYI_BLOCK = 0;
    EAX->MYI_PTRA = 0;
    if (ECX)
	_release_io_segment(ECX);
}

void _close_myi_handle(MYI_STRUCT *EAX)
{
    void *ECX = EAX->MYI_HANDLE;
    EAX->MYI_HANDLE = 0;
    LAST_FILENAME_GINDEX = 0;

    if (ECX)
	_close_handle(ECX);
}

