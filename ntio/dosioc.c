
#include <windows.h>

#include "all.h"

extern void _disk_full(MYO_STRUCT *);


void _try_disk_full(void *p)
{
    int r = GetLastError();
    if (r == 39 || r == 112)
	_disk_full(p);
}

void _dos_fail_read()
{
    _FLUSH_DISABLE_MAPOUT();
    _cerr_abort(DOS_READ_ERR);
}

void _dos_fail_write(void *p)
{
    _try_disk_full(p);
    _FLUSH_DISABLE_MAPOUT();
    _cerr_abort(DOS_WRITE_ERR);
}

void _dos_fail_close()
{
    _try_disk_full(0);
    _FLUSH_DISABLE_MAPOUT();
    _cerr_abort(CLOSE_ERR);
}

void _dos_fail_move(void *p)
{
    _try_disk_full(p);
    _FLUSH_DISABLE_MAPOUT();
    _cerr_abort(DOS_POS_ERR);
}

void _dos_fail_seteof(void *p)
{
    _try_disk_full(p);
    _FLUSH_DISABLE_MAPOUT();
    _cerr_abort(DOS_NEWSIZE_ERR);
}

static unsigned char _ABORT_FLAG;

void _cancel_terminate()
{
    if (_ABORT_FLAG)
	_critical_error();
    else
	_cerr_abort(CTRLC_ERR);
}

void _critical_error()
{
    _FLUSH_DISABLE_MAPOUT();
    _cerr_abort(DOS_ERR);
}

void _cerr_abort(int errcode)
{
    if (_CRITI_IN_PROGRESS)
	_abort();
    _CRITI_IN_PROGRESS = -1;
    _err_abort(errcode);
}

void _err_abort(int errcode)
{
    if (!_NO_CANCEL_FLAG)
	_err_ret(errcode & 0xFF);
    _abort();
}

void _abort()
{
    if (_ERR_COUNT && _DELETE_EXE_ON_ERROR)
	_delete_exe();
    __exit(_ERR_COUNT);
}

void __exit(int exitcode)
{
	// cannot exit this way and thru XCNOTIFY...
	_capture_eax(&_ABORTER_SEM);
	ExitProcess(exitcode);
}

void _close_handle(void *handle)
{
    int r = CloseHandle(handle);
    if (_FINAL_ABORTING)
	return;
    if (r == 0)
	_dos_fail_close();
}

int _mesout(char *p)
{
    return _loutall_con(*(unsigned char *)p, p + 1);
}

extern HANDLE _STDOUT;

int _loutall_con(unsigned nbytes, char *ptr)
{
    _capture_eax(&_SAY_VERBOSE_SEM);
    DWORD nwritten = 0;
    int r = WriteFile(_STDOUT, ptr, nbytes, &nwritten, NULL);
    _release_eax(&_SAY_VERBOSE_SEM);
    if (r == 0)
	__exit(r);
    return nwritten;
}

void halt()
{
    __asm { hlt }
}

#if 1 || FGH_ANYTHREADS

void _do_dossleep_0()
{
    Sleep(0);
}

#endif

#if 1
extern struct MYO_STRUCT *_EXE_DEVICE;
extern struct OUTFILE_STRUCT *_OUTFILE_GINDEX;

void _delete_exe()
{
    _DELETE_EXE_ON_ERROR = 0;	// in case this fails...

    if (_EXE_DEVICE)
    {
	void *h = _EXE_DEVICE->MYO_HANDLE;
	_EXE_DEVICE->MYO_HANDLE = 0;

	if (h)
	    _close_handle(h);
    }
    if (_OUTFILE_GINDEX)
    {
	DeleteFile((char *)_OUTFILE_GINDEX->_OF_FILE_LIST_GINDEX->FILE_LIST_NFN.NFN_TEXT);
    }
}
#endif

MYO_STRUCT *_doswrite(MYO_STRUCT *s, unsigned bytecount, unsigned char *buf)
{
    DWORD nwritten = 0;
    int n;

    n = WriteFile(s->MYO_HANDLE, buf, bytecount, &nwritten, NULL);
    if (n == 0)
	_dos_fail_write(s);
    s->MYO_PHYS_ADDR += n;
    if (bytecount != nwritten)
	_disk_full(s);
    return s;
}

MYI_STRUCT *_dosread(MYI_STRUCT *s, unsigned bytecount, unsigned char *buf)
{
    s->MYI_BYTE_OFFSET = s->MYI_PHYS_ADDR;
    DWORD nread = 0;
    int n = ReadFile(s->MYI_HANDLE, buf, bytecount, &nread, NULL);
    if (n == 0)
	_dos_fail_read();
    s->MYI_PHYS_ADDR += nread;
    s->MYI_COUNT = nread;
    if (bytecount != nread)
	_err_abort(UNEXP_EOF_ERR);
    return s;
}

MYI_STRUCT *_dosposition_i(MYI_STRUCT *s, unsigned offset)
{
    s->MYI_BYTE_OFFSET = offset;
    if (offset != s->MYI_PHYS_ADDR)
    {
	s->MYI_PHYS_ADDR = offset;
	SetFilePointer(s->MYI_HANDLE, offset, NULL, 0);
    }
    return s;
}

MYI_STRUCT *_read_16k_threaded(MYI_STRUCT *myi)
{
    // get next buffer of data...

    void *p = myi->MYI_BLOCK;
    if (p)
    {
	myi->MYI_PTRA = 0;
	myi->MYI_BLOCK = 0;
	_release_block(p);		// get rid of last block
    }

    MYI2_STRUCT *myi2 = myi->MYI_LOCALS;

    // # of 16k buffers+1
    if (myi->MYI_BUFCNT == 1)
	_err_abort(UNEXP_EOF_ERR);

    myi->MYI_BYTE_OFFSET = myi->MYI_PHYS_ADDR;		// update addr at beginning of next block

    int bufi = (myi2->MYI2_NEXT_16K_BLOCK + 1) & 3;
    myi2->MYI2_NEXT_16K_BLOCK = bufi;	// next buffer...

    _capture_eax(&myi2->INS_FULL_SEM);	// wait for buf full, mark empty

    // ok, have this buffer.  set up stuff
    p = myi2->MYI2_INPUT_STRUC[bufi].INS_BLOCK;
    myi2->MYI2_INPUT_STRUC[bufi].INS_BLOCK = 0;

    myi->MYI_PTRA = p;
    myi->MYI_BLOCK = p;

    int nbytes = myi2->MYI2_INPUT_STRUC[bufi].INS_BYTES;
    myi->MYI_COUNT = nbytes;
    myi->MYI_PHYS_ADDR += nbytes;

    if (nbytes == PAGE_SIZE)
	myi->MYI_BUFCNT = -1;
    else
	myi->MYI_BUFCNT = 1;
    // mark last buffer available
    _release_eax_bump(&myi2->INS_AVAIL_SEM);

    return myi;
}

void _err_nfn_abort(int errnum, NFN_STRUCT *);

void _err_file_list_abort(unsigned char errnum, FILE_LIST_STRUCT *ECX)
{
    _err_nfn_abort(errnum, &ECX->FILE_LIST_NFN);
}

int _NONRES_POSITION;
int _NONRES_LENGTH;

int _n_nonres_table(MYI_STRUCT *s)
{
    if (!(_HOST_THREADED & 0xFF))
	_dosposition_i(s, _NONRES_POSITION);
    return _NONRES_LENGTH;
}

#if 1

unsigned RES_LENGTH;

struct NRT_VARS
{
    unsigned char record[0x40];
};


/*********************************
 * Return # of bytes.
 * Return 0 on none or error.
 */
int _n_res_table(MYI_STRUCT *myi)
{
    if (_HOST_THREADED & 0xFF)
    {
	_capture_eax(&_NONRES_LENGTH_SEM);
	return RES_LENGTH;
    }

    struct NRT_VARS MY_TEMP_RECORD;
    _dosread(myi, 0x40, (unsigned char *)&MY_TEMP_RECORD);

    unsigned neoffset = 0;		// offset to NE signature

    struct EXE* pexe = (struct EXE *)&MY_TEMP_RECORD;
    if (pexe->_EXE_SIGN == 'ZM')
    {
	neoffset = *(unsigned *)&MY_TEMP_RECORD.record[0x3C];
	if (pexe->_EXE_RELOC_OFF != 0x40)
	    goto L9;
	_dosposition_i(myi, neoffset);
	_dosread(myi, 0x40, (unsigned char *)&MY_TEMP_RECORD);
    }
    struct NEXE* pnexe = (struct NEXE *)&MY_TEMP_RECORD;
    if (pexe->_EXE_SIGN == 'EN')
    {
	_NONRES_POSITION = pnexe->_NEXE_NRESNAM_OFFSET;	// OFFSET FROM BEG OF EXE
	_NONRES_LENGTH = pnexe->_NEXE_NONRES_LENGTH;

	unsigned EAX = pnexe->_NEXE_RESNAM_OFFSET;
	unsigned EDX = pnexe->_NEXE_MODREF_OFFSET;

	if (EDX >= EAX)
	{   _dosposition_i(myi, neoffset + EAX);	// RESNAM ADDRESS
	    return EDX - EAX;
	}
    }

L9:
    _NONRES_LENGTH = 0;		// for later
    return 0;
}

#endif

#if 1
void _disk_full(MYO_STRUCT *ESI)
{
    // BETTER PANIC-RELEASE SOME MEMORY SO WE CAN TERMINATE...
    void *a;
    _asm
    {
	xor	EAX,EAX
	xchg	SYM_HASH_LOG,EAX
	mov	a,EAX
    }
    if (a)
	_release_block(a);
    _asm
    {
	xor	EAX,EAX
	xchg	MULTI_HASH_LOG,EAX
	mov	a,EAX
    }
    if (a)
	_release_block(a);
    _release_minidata(&SSYM_STUFF);
    _release_local_storage();
    _disable_mapout();

    if (!ESI)
	_err_abort(DISK_FULL_ERR);

    _err_file_list_abort(DISK_FULL_ERR, ESI->MYO_FILE_LIST_GINDEX);
}

#endif

/////////////////////// Our own I/O /////////////////////////////

void print(char *msg)
{
    _loutall_con(strlen(msg), msg);
}

int hexToAscii(unsigned u)
{
    return (u >= 10) ? u + 'A' - 10 : u + '0';
}

void printbyte(unsigned b)
{
    char buf[2];
    buf[0] = hexToAscii((b >> 4) & 15);
    buf[1] = hexToAscii(b & 15);
    _loutall_con(2, buf);
}
