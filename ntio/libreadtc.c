
#include <windows.h>

#include "all.h"

extern void XCNOTIFY();

// Forward references
void _read_a_block(MYL2_STRUCT *);
void _read_this_library(MYL2_STRUCT *, LIBRARY_STRUCT *);
void libread2(MYL2_STRUCT *EBX);

// Entry point for library reading thread
__declspec(naked) void LIBREADT(MYL2_STRUCT *EBX)
{
    __asm
    {
	mov     EBX,4[ESP]

	; Set XCNOTIFY as exception handler
	push    -1
	push    0
	push    offset XCNOTIFY
	push    dword ptr FS:[00h]
	mov     FS:[00h],ESP

	push	EBX
	call	libread2
    }
}

void libread2(MYL2_STRUCT *EBX)
{
    SetThreadPriority((HANDLE)EBX->MYL2_LIBREAD_THREAD_HANDLE, THREAD_PRIORITY_ABOVE_NORMAL); // was normal
    while (1)
    {
	// Bugzilla 3372: This call is known to deadlock here on the 2nd example
	_capture_eax(&EBX->MYL2_LIB_BLOCK_SEM);	// wait till a block is requested
	if (LIBS_DONE)
	    break;
	_read_a_block(EBX);
    }
    ExitThread(0);
}

void _read_a_block(MYL2_STRUCT *EBX)
{
    // Scan MYL_REQUESTED looking for a library with a desired block

    _capture_eax(&LIB_REQUEST_ORDER_SEM);	// library request order cannot change

    LIBRARY_STRUCT *ECX;
    for (ECX = EBX->MYL2_FIRST_REQUEST_LIB_GINDEX;
	    ECX;
	    ECX = ECX->_LS_NEXT_TRLIB_GINDEX)
    {
	if (ECX->_LS_OS2_BLOCKS_REQUESTED)
	    break;
    }
    _release_eax(&LIB_REQUEST_ORDER_SEM);

    if (LIBS_DONE)
	ExitThread(0);

    if (ECX)
    {
	EBX->MYL2_CURRENT_LIB_GINDEX = ECX;
	_read_this_library(EBX, ECX);
    }
}

void _lerr_abort(int EAX, MYL2_STRUCT *EBX)
{
    LIBRARY_STRUCT *ECX = EBX->MYL2_CURRENT_LIB_GINDEX;

    if (LIBS_DONE)
	ExitThread(0);

    FILE_LIST_STRUCT *fls = ECX->_LS_FILE_LIST_GINDEX;
    _terr_abort(EAX, &fls->FILE_LIST_NFN.NFN_TEXT[0]);
}

void _capture_verify_handle(MYL2_STRUCT *EBX, LIBRARY_STRUCT *ESI)
{
    if (!ESI->_LS_HANDLE)
    {   // Someone has closed this handle
	_lerr_abort(0, EBX);
    }
}

void _read_this_library(MYL2_STRUCT *EBX, LIBRARY_STRUCT *ESI)
{
    //printf("_read_this_library(%x, %x)\r\n", EBX, ESI);
    // this lib has at least one outstanding block request, read the first one
    _capture_verify_handle(EBX, ESI);		// assure exclusive access to handle

    unsigned EDI = *ESI->_LS_NEXT_TRLIB_BLOCK++;	// next block for this thread to read

    // ESI is library logical, ECX is block #

    // Make sure DOSFILEPOSITION is correct
    int pos = EDI << PAGE_BITS;
    if (pos != ESI->_LS_FILE_POSITION)
    {
	pos = SetFilePointer(ESI->_LS_HANDLE, pos, 0, FILE_BEGIN);
	if (pos == -1)
	    _lerr_abort(DOS_POS_ERR, EBX);
	ESI->_LS_FILE_POSITION = pos;
    }

    // Calculate # of bytes
    int nbytes;
    if (ESI->_LS_BLOCKS != EDI + 1)
	nbytes = PAGE_SIZE;
    else
    {
	nbytes = ESI->_LS_FILE_LIST_GINDEX->FILE_LIST_NFN.NFN_FILE_LENGTH;
	nbytes &= (PAGE_SIZE - 1);
	if (!nbytes)
	    nbytes = PAGE_SIZE;
    }
    EBX->MYL2_DESIRED_BYTES = nbytes;

    // allocate logical block
    void *buf = _get_new_phys_blk();

    if (LIBS_DONE)
	ExitThread(0);

    EBX->MYL2_BUFFER_ADDRESS = buf;
    unsigned long result;
    if (!ReadFile(ESI->_LS_HANDLE, buf, EBX->MYL2_DESIRED_BYTES, &result, 0))
	_lerr_abort(DOS_READ_ERR, EBX);

    if (result != EBX->MYL2_DESIRED_BYTES)
	_lerr_abort(UNEXP_XEOF_ERR, EBX);

    ESI->_LS_FILE_POSITION += result;

    // Store block address
    void *EDX = ESI->_LS_BLOCK_TABLE[0];

    if (ESI->_LS_BLOCKS >= PAGE_SIZE/8/2)
    {
	// There are PAGE_SIZE/8 blocks per page
	int EAX = EDI >> (PAGE_BITS - 3);
	EDI &= (PAGE_SIZE - 1) >> 3;
	EDX = ESI->_LS_BLOCK_TABLE[EAX];
    }
    --ESI->_LS_OS2_BLOCKS_REQUESTED;
    ((void **)EDX)[EDI * 2] = buf;		// store block segment

    _release_eax(&EBX->MYL2_BLOCK_READ_SEM);	// signal another completion
}

