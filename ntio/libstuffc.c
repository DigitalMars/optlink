
#include "all.h"

MYI_STRUCT *_readlib_16k(MYI_STRUCT *myi)
{
    // maybe get rid of this block
    _dec_lib_block();

    CURN_LIB_BLOCK++;

    void **EBX = CURN_LIB_BLOCK_PTR + 2;
    if (CURN_LIB_BLOCK_PTR_LIMIT == EBX)
	    goto L2;
L1:
    CURN_LIB_BLOCK_PTR = EBX;
    void *EAX = *EBX;
    if (EAX)
	    goto L5;
    _read_library_block(CURN_LIB_BLOCK);

    EAX = *EBX;
    goto L5;

L2:
    EBX = *CURN_LS_BLOCK_TABLE_PTR++;
    CURN_LIB_BLOCK_PTR_LIMIT = EBX + PAGE_SIZE;
    goto L1;

L5:
    MYI_STRUCT* ms = OBJ_DEVICE;
    ms->MYI_PTRA = EAX;
    ms->MYI_BLOCK = EAX;

    LIBRARY_STRUCT *ls = CURNLIB_GINDEX;

    unsigned len = FILNAM.NFN_FILE_LENGTH;

    if (ls->_LS_BLOCKS == CURN_LIB_BLOCK + 1)
    {
	len &= PAGE_SIZE-1;
	if (len)
	    goto L6;
    }
    len = PAGE_SIZE;
L6:
    ms->MYI_COUNT = len;
    return 0;
}

ubyte *GET_FIRST_SECTOR()
{
    if (_HOST_THREADED)
    {
	MYI2_STRUCT* ECX = OBJ_DEVICE->MYI_LOCALS;

	OPEN_STRUCT* os = &ECX->MYI2_OPEN_STRUC[ECX->MYI2_NEXT_OPEN_FILE];
	_capture_eax(&ECX->OPENFILE_HEADER_SEM);
	return &os->OPENFILE_HEADER[0];
    }

    // allocate a place to put first sector of library file
    _dosread(OBJ_DEVICE, 16, &LIB_HEADER[0]);
    return &LIB_HEADER[0];
}

void *GET_NEW_IO_LOG_BLK();

unsigned char *READ_NEXT_DIRECTORY_BLOCK()
{
    if (_HOST_THREADED)
    {
	MYI_STRUCT *EAX = _read_16k_threaded(OBJ_DEVICE);
	unsigned char *EDX = EAX->MYI_BLOCK;
	EAX->MYI_BLOCK = 0;
	EAX->MYI_PTRA = 0;
	return EDX;
    }

    // allocate a place to put first sector of library file

    unsigned ECX = DICT_BLOCKS;
    unsigned char *EDX = GET_NEW_IO_LOG_BLK();
    CURN_DICT_LOG = EDX;

    // max dirs per page
    if (ECX > PAGE_SIZE/512)
	ECX = PAGE_SIZE/512;

    _dosread(OBJ_DEVICE, ECX * 512, EDX);
    return EDX;
}
