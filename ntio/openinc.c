#include <windows.h>

#include "all.h"

// Open file NFN in EAX
// Returns: EAX is 'DEVICE'
// Carry set/cleared

void *OPEN_INPUT()
{
    NFN_STRUCT *nfn;

    _asm
    {
	mov nfn,EAX
    }

    MYI_STRUCT *EBX = &MYI_STUFF[0];		// only one for DOS

    if (EBX->MYI_HANDLE)
	_close_handle(EBX->MYI_HANDLE);
    EBX->MYI_HANDLE = NULL;
    EBX->MYI_FILE_LIST_GINDEX = CURN_FILE_LIST_GINDEX;
L15:
    FILE_LIST_STRUCT *ECX = EBX->MYI_FILE_LIST_GINDEX;
    HANDLE EAX = CreateFile((char *)&nfn->NFN_TEXT[0], GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
	(ECX && ECX->FILE_LIST_FLAGS & FLF_RANDOM) ? FILE_FLAG_RANDOM_ACCESS : FILE_FLAG_SEQUENTIAL_SCAN,
	0);
    if (EAX == INVALID_HANDLE_VALUE)
    {
	if (!_recover_handle())
	    goto L15;
	_asm
	{
	    stc
	}
	return EBX;
    }

    EBX->MYI_HANDLE = EAX;

    // Do we need to get file T&D?
    if (!(nfn->NFN_FLAGS & NFN_TIME_VALID))
    {
	nfn->NFN_FLAGS |= NFN_TIME_VALID;
	nfn->NFN_FILE_LENGTH = GetFileSize(EBX->MYI_HANDLE, 0);
    }
    EBX->MYI_BYTE_OFFSET = 0;
    EBX->MYI_FILE_LENGTH = nfn->NFN_FILE_LENGTH;
    EBX->MYI_PHYS_ADDR = 0;
    EBX->MYI_COUNT = 0;			// buffer empty
    EBX->MYI_FILLBUF = &_read_16k_input;
    _asm
    {
	clc
    }
    return EBX;
}

/***********************
 * Returns:
 *	0	success
 *	1	failure
 */
int _open_input(NFN_STRUCT *nfn, MYI_STRUCT **pdevice)
{   MYI_STRUCT *device;
    int result = 0;
    _asm
    {
	mov	EAX,nfn
	call	OPEN_INPUT
	mov	device,EAX
	jnc	L1
	mov	result,1
     L1:
    }
    *pdevice = device;
    return result;
}


MYI_STRUCT *_read_16k_input(MYI_STRUCT *myi)
{
    void *buf = myi->MYI_BLOCK;
    unsigned bytecount = myi->MYI_FILE_LENGTH;

    if (!buf)
    {
	    buf = _get_new_phys_blk();
	    myi->MYI_BLOCK = buf;	// leave in fast
    }
    if (bytecount < myi->MYI_PHYS_ADDR)
	bytecount = 0;
    else
    {
	bytecount -= myi->MYI_PHYS_ADDR;
	if (bytecount > PAGE_SIZE)
	    bytecount = PAGE_SIZE;
    }
    myi->MYI_PTRA = buf;
    return _dosread(myi,bytecount,buf);
}

