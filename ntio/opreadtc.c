
#include <windows.h>

#include "all.h"

// The only public is OPREADT


/**********************************
 *	STANDARD OBJ'S SCAN OBJ PATHLIST IF NOT FOUND		8-17-89
 *	LIBOPENT MUST HANDLE 'STUB' AND 'OLD'
 *
 *		1: WAIT TILL ALL OPEN & READ THREADS HAVE FINISHED, FLAG FACT
 *		2: CLEAR LIBS_DONE FLAG
 *		3: FIND, OPEN, START A READ THREAD ON 'STUB'
 *		4: FIND, OPEN, START A READ THREAD ON 'OLD'
 *		5: WHEN SIGNALLED, GO THROUGH LIBS_DONE PROCESS AGAIN...
 */

#pragma pack(1)
struct LibHeader
{
    unsigned char       recTyp;      // 0xF0
    unsigned short      pagesize;
    long                lSymSeek;
    unsigned short      ndicpages;
    unsigned char       flags;
    char		filler[6];
};
#pragma pack()

unsigned NE_BASE;
unsigned NONRES_POSITION;

extern void XCNOTIFY();

// Forward references
void opreadt2(MYI2_STRUCT *EBX);
void _read_old_proc(MYI2_STRUCT *EBX, OPEN_STRUCT *EDI);
int _read_40H(MYI2_STRUCT *EBX, OPEN_STRUCT *EDI);
int _position(int EAX, OPEN_STRUCT *EDI);
void _get_next_open_struc(MYI2_STRUCT *EBX);
FILE_LIST_STRUCT *_get_next_file_name(MYI2_STRUCT *EBX);
int _open_file(FILE_LIST_STRUCT *EAX, MYI2_STRUCT *EBX, NFN_STRUCT **pESI, OPEN_STRUCT **pEDI);
int _try_search(FILE_LIST_STRUCT *EAX, NFN_STRUCT *ESI, OPEN_STRUCT *EDI, MYI2_STRUCT *EBX);
int _try_open(NFN_STRUCT *ESI, OPEN_STRUCT *EDI, MYI2_STRUCT *EBX);
INPUT_STRUCT *_get_next_buffer(MYI2_STRUCT *EBX);
unsigned _read_next_block(OPEN_STRUCT *EDI, MYI2_STRUCT *EBX);
unsigned _read_first_block(OPEN_STRUCT *EDI, MYI2_STRUCT *EBX);
unsigned _read_block(INPUT_STRUCT *ESI, OPEN_STRUCT *EDI);
void _read_16(MYI2_STRUCT *EBX, OPEN_STRUCT *EDI);
void _readt_abort(MYI2_STRUCT *EBX);

__declspec(naked) void OPREADT(MYI2_STRUCT *EBX)
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
	call	opreadt2
	; Never returns
    }
}

void opreadt2(MYI2_STRUCT *EBX)
{
    // File opener - reader... this guys sole function in life is
    // to open and read files

    // Make this slightly higher priority than main
    SetThreadPriority((HANDLE)EBX->MYI2_OPREAD_THREAD_HANDLE, THREAD_PRIORITY_ABOVE_NORMAL); // was normal

    NFN_STRUCT *ESI;
    OPEN_STRUCT *EDI;

OPENT_LOOP:
    _get_next_open_struc(EBX);		// get my next 'OPEN' structure in SI
					// will wait if 4 files already open

    FILE_LIST_STRUCT *fls = _get_next_file_name(EBX);		// get AX GINDEX to next FILENAME_LIST
					// will wait if no files in list

    if (_open_file(fls, EBX, &ESI, &EDI))	// sets ESI, EDI
	goto OPENT_LOOP;

    EDI->OPENFILE_ADDR = 0;
    ubyte AL = ESI->NFN_TYPE;
    if (AL != NFN_OBJ_TTYPE)
	goto READ_LIBS;		// special file types

READ_LIBS_NOPE:
    if (_read_first_block(EDI, EBX) == PAGE_SIZE)	// read next 16K from file (adjusts size if LIB)
    {
	// Read 16K at a time
	while (_read_next_block(EDI, EBX) == PAGE_SIZE)
	    ;			// more in file, loop
    }

    void *h;
    OPEN_STRUCT *p = EDI;
    _asm
    {
	mov	EDI,p
	xor	EAX,EAX
	xchg	EAX,[EDI].OPENFILE_HANDLE	;IN CASE ANOTHER THREAD TERMINATING ME
	mov	h,EAX
    }

    if (h)
	_close_handle(h);
    goto OPENT_LOOP;

READ_LIBS:
    // EITHER A LIBRARY FILE, STUB FILE, OR 'OLD' FILE
    if (AL == NFN_RES_TTYPE ||	// .RES FILES READ SEQUENTIALLY LIKE .OBJ'S
	AL == NFN_STUB_TTYPE ||	// STUB FILES READ SEQUENTIALLY...
	AL == NFN_LOD_TTYPE)
	goto READ_LIBS_NOPE;

    if (AL == NFN_OLD_TTYPE)
    {
	_read_old_proc(EBX, EDI);
	goto OPENT_LOOP;
    }

    // Starting library handling
    // EDI is OPENFILE structure
    _read_16(EBX, EDI);		// reads 16 bytes

    struct LibHeader *lh = (struct LibHeader *)EBX->MYI2_TEMP_RECORD;

    if (lh->recTyp != 0xF0)
	goto READ_LIBS_NOPE;	// might be searching an OBJ file

    if (lh->pagesize >= 4096)
	goto READ_LIBS_NOPE;

    /* Assume it really is a library.
     * Read in the entire dictionary
     */

    // Seek to dictionary in library file
    if (SetFilePointer(EDI->OPENFILE_HANDLE, lh->lSymSeek, 0, FILE_BEGIN) == -1)
	_oerr_abort(DOS_POS_ERR, EDI->OPENFILE_NAME->NFN_TEXT);

    EDI->OPENFILE_ADDR = lh->lSymSeek;

    // Adjust length reported in FN_LENGTH to skip extended
    EDI->OPENFILE_NAME->NFN_FILE_LENGTH = lh->lSymSeek + (lh->ndicpages * 512);

    unsigned nblocks = lh->ndicpages;
    if (nblocks)
    {
	// Read directory in PAGE_SIZE chunks
	while (1)
	{
	    _read_next_block(EDI, EBX);		// get buffer and fill it
	    if (nblocks <= PAGE_SIZE/512)
		break;
	    nblocks -= PAGE_SIZE/512;
	}
    }
    goto OPENT_LOOP;
}

void _read_old_proc(MYI2_STRUCT *EBX, OPEN_STRUCT *EDI)
{
    /* http://www.digitalmars.com/ctg/ctgDefFiles.html#old
       This directive tells OPTLINK to search another executable file (normally a DLL) for EXPORT
       ordinals. OPTLINK searches the directories listed in the path environment variable to locate
       the executable file, if it is not in the current working directory. If the file is not found,
       a warning is issued. Exported names in the current executable file which match those in the
       old executable file are assigned ordinal values from the old executable file.
     */

    // Read stuff needed for 'OLD' entry points
    if (_read_40H(EBX, EDI))
	goto L19;

    EXE *pexe = (EXE *)&EBX->MYI2_TEMP_RECORD;	// BUG in asm optlink - didn't set ESI

    if (pexe->_EXE_SIGN != 'ZM')
	goto L12;

    if (pexe->_EXE_RELOC_OFF != 0x40)
	goto L19;

    NE_BASE = *(unsigned *)((char *)pexe + 0x3C);
    if (_position(NE_BASE, EDI))
	goto L19;			// error positioning

    if (_read_40H(EBX, EDI))
	goto L19;

L12:
    NEXE *pnexe = (NEXE *)pexe;

    if (pnexe->_NEXE_SIGN != 'EN')
	goto L19;

    NONRES_LENGTH = pnexe->_NEXE_NONRES_LENGTH;
    NONRES_POSITION = pnexe->_NEXE_NRESNAM_OFFSET;

    unsigned EDX = pnexe->_NEXE_RESNAM_OFFSET;
    unsigned nbytes = pnexe->_NEXE_MODREF_OFFSET;
    if (nbytes < EDX)
	goto L19;
    nbytes -= EDX;		// MODREF - RESNAM

    if (!_position(NE_BASE + EDX, EDI))
	goto L1;
L19:
    nbytes = 0;
L1:
    RES_LENGTH = nbytes;

    // Set FILE_LENGTH based on bytes left to read

    NFN_STRUCT *ESI = EDI->OPENFILE_NAME;

    ESI->NFN_FILE_LENGTH = NONRES_LENGTH + NONRES_POSITION;
    _release_eax(&_NONRES_LENGTH_SEM);	// tell him length is valid

    // NOW, READ CX BYTES FROM FILE - IN NORMAL FASHION...
    unsigned n = RES_LENGTH;
    if (!n)
	goto L5;
L2:
    _read_next_block(EDI, EBX);			// READ NEXT 16K FROM FILE

    if (n > PAGE_SIZE)
    {   n -= PAGE_SIZE;
	goto L2;
    }
L5:
    n = NONRES_LENGTH;

    if (!n)
	goto L9;

    _position(NONRES_POSITION, EDI);
L6:
    _read_next_block(EDI, EBX);			// read next 16K from file

    if (n > PAGE_SIZE)
    {   n -= PAGE_SIZE;
	goto L6;
    }
L9:
	// Close ASAP so we can open it write-mode
    void *h;
    OPEN_STRUCT *p = EDI;
    _asm
    {
	xor	EAX,EAX
	mov	EDI,p
	xchg	EAX,[EDI].OPENFILE_HANDLE	;IN CASE ANOTHER THREAD TERMINATING ME
	mov	h,EAX
    }

    if (h)
	_close_handle(h);
}


int _read_40H(MYI2_STRUCT *EBX, OPEN_STRUCT *EDI)
{
	// Read 40H bytes from file into MYI2_TEMP_RECORD
	unsigned long result;
	int EAX = ReadFile(EDI->OPENFILE_HANDLE, &EBX->MYI2_TEMP_RECORD, 0x40, &result, 0);
	if (!EAX || result != 0x40)
	    return 1;

	EDI->OPENFILE_ADDR += 0x40;
	return 0;
}


int _position(int EAX, OPEN_STRUCT *EDI)
{
	// EAX is seek address

	int r = SetFilePointer(EDI->OPENFILE_HANDLE, EAX, 0, FILE_BEGIN);
	if (r == -1)
	    goto POS_ERR;

	EDI->OPENFILE_ADDR = r;
	return (r == EAX) ? 0 : 1;

POS_ERR:
	return 1;
}


void _get_next_open_struc(MYI2_STRUCT *EBX)
{
    // Wait for next available OPENFILE structure

    int i = (EBX->MYI2_NEXT_OPEN_STRUC + 1) & 3;
    EBX->MYI2_NEXT_OPEN_STRUC = i;

    _capture_eax(&EBX->OPENFILE_AVAIL_SEM);		// make sure main thread is done with this

    if (OPREADS_DONE)
	_readt_abort(EBX);

    EBX->MYI2_OPEN_STRUC[i].OPENFILE_FLAGS = 0;
}

FILE_LIST_STRUCT *_get_next_file_name(MYI2_STRUCT *EBX)
{
    // Get next filename in AX
    // This will wait if no files in list & not time for libraries

    // Wait for another file in list
    _capture_eax(&EBX->MYI2_FILENAME_LIST_SEM);

    if (OPREADS_DONE)
	_readt_abort(EBX);

    // Get next filename to open

    FILE_LIST_STRUCT *EAX = EBX->MYI2_LAST_FILENAME_OPENED_GINDEX;
    EAX = EAX->FILE_LIST_MY_NEXT_GINDEX;

    EBX->MYI2_LAST_FILENAME_OPENED_GINDEX = EAX;
    return EAX;
}

int _open_file(FILE_LIST_STRUCT *EAX, MYI2_STRUCT *EBX, NFN_STRUCT **pESI, OPEN_STRUCT **pEDI)
{
    // EAX IS FILE_LIST_GINDEX STRUCTURE.

//printf("_open_file()\r\n");
    NFN_STRUCT *ESI = &EBX->MYI2_NAMS[EBX->MYI2_NEXT_OPEN_STRUC];

    memcpy(ESI, &EAX->FILE_LIST_NFN,
	    ((EAX->FILE_LIST_NFN.NFN_TOTAL_LENGTH + sizeof(NFN_STRUCT)-NFN_TEXT_SIZE + 4) / 4) * 4);

    OPEN_STRUCT *EDI = &EBX->MYI2_OPEN_STRUC[EBX->MYI2_NEXT_OPEN_STRUC];

    void *h;
    OPEN_STRUCT *p = EDI;
    _asm
    {
	xor	EAX,EAX
	mov	EDI,p
	xchg	EAX,[EDI].OPENFILE_HANDLE	;multithread
	mov	h,EAX
    }

    if (h)
	_close_handle(h);

    EDI->OPENFILE_NAME = ESI;
    EDI->OPENFILE_PATH_GINDEX = 0;
    if (!_try_open(ESI, EDI, EBX))		// try to open it as stated
	goto L8;

    // Nope, was path specified?
    if (ESI->NFN_FLAGS & NFN_PATH_SPECIFIED)
	goto L5;			// cannot try any more

    // Different search lists for OBJ vs LIB vs OLD/STUB
    FILE_LIST_STRUCT *ECX = OBJPATH_LIST.FILE_FIRST_GINDEX;

    if (ESI->NFN_TYPE == NFN_OBJ_TTYPE)
	goto L1;

    if (ESI->NFN_TYPE != NFN_RES_TTYPE)	// RES files use same paths
	goto L2;
L1:
    if (!_try_search(ECX, ESI, EDI, EBX))		// try OBJ= first
	goto L8;
    goto L3;			// nope, try LIB= too

L2:
    ECX = STUBPATH_LIST.FILE_FIRST_GINDEX;
    if (ESI->NFN_TYPE == NFN_OLD_TTYPE || ESI->NFN_TYPE == NFN_STUB_TTYPE)
	goto L4;
L3:
    ECX = LIBPATH_LIST.FILE_FIRST_GINDEX;
L4:
    if (!_try_search(ECX, ESI, EDI, EBX))
	goto L8;

    // OBJ AND RES FILES ARE FATAL, REST ARE WARNINGS...
L5:
    EDI->OPENFILE_HANDLE = 0;
    _release_eax(&EBX->OPENFILE_OPEN_SEM);
//printf("_open_file() 1\r\n");
    return 1;			// not found

L8:
    // Tell main we got the file opened
    _release_eax(&EBX->OPENFILE_OPEN_SEM);

    *pESI = ESI;
    *pEDI = EDI;
//printf("_open_file() 0\r\n");
    return 0;			// found
}


int _try_search(FILE_LIST_STRUCT *EAX, NFN_STRUCT *ESI, OPEN_STRUCT *EDI, MYI2_STRUCT *EBX)
{
    // EAX is GINDEX of paths to try

    goto TEST_PATH;

PATH_LOOP:
    // Move just path

    NFN_STRUCT *ECX = &EAX->FILE_LIST_NFN;

    if (ECX->NFN_FLAGS & NFN_PATH_SPECIFIED)	// skip NUL
    {
	_move_ecxpath_eax(ESI, ECX);	// move new path

	if (!_try_open(ESI, EDI, EBX))
	{
	    EDI->OPENFILE_PATH_GINDEX = EAX;
	    return 0;
	}
    }

TEST_PATH:
    EAX = EAX->FILE_LIST_NEXT_GINDEX;
    if (EAX)
	goto PATH_LOOP;
    return 1;
}


int _try_open(NFN_STRUCT *ESI, OPEN_STRUCT *EDI, MYI2_STRUCT *EBX)
{
    //printf("_try_open(%s)\r\n", ESI->NFN_TEXT);

    // Define OPEN_FLAGS based on file type
    // .OBJ, .RES, STUB sequential access, others unknown

    if (OPREADS_DONE)
	_readt_abort(EBX);

    int ECX = FILE_FLAG_SEQUENTIAL_SCAN;
    if (ESI->NFN_TYPE == NFN_LIB_TTYPE || ESI->NFN_TYPE == NFN_OLD_TTYPE)
	ECX = FILE_FLAG_RANDOM_ACCESS;

    void *EAX = CreateFile((char *)ESI->NFN_TEXT, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, ECX, 0);
    if (EAX == INVALID_HANDLE_VALUE)
    {
	// Here we see if OUT_OF_HANDLES was cause of open failure
	return 1;
    }

    EDI->OPENFILE_HANDLE = EAX;

    EDI->OPENFILE_FLAGS |= 1;		// open successful

    // Valid if ambiguous filename
    if (!(ESI->NFN_FLAGS & NFN_TIME_VALID))
    {
	ESI->NFN_FLAGS |= NFN_TIME_VALID;
	int size = GetFileSize(EDI->OPENFILE_HANDLE, 0);
	if (size == -1)
	    _oerr_abort(QFH_FAILED_ERR, ESI->NFN_TEXT);
	ESI->NFN_FILE_LENGTH = size;
    }
    return 0;
}


/*********
 * Get next empty input buffer, will wait if no buffers available
 */
INPUT_STRUCT *_get_next_buffer(MYI2_STRUCT *EBX)
{
    unsigned i = (EBX->MYI2_NEXT_FILE_BUFFER + 1) & 3;
    EBX->MYI2_NEXT_FILE_BUFFER = i;
    _capture_eax(&EBX->INS_AVAIL_SEM);

    if (OPREADS_DONE)
	_readt_abort(EBX);
    return &EBX->MYI2_INPUT_STRUC[i];
}


unsigned _read_next_block(OPEN_STRUCT *EDI, MYI2_STRUCT *EBX)
{
    INPUT_STRUCT *ESI = _get_next_buffer(EBX);
    unsigned EAX = _read_block(ESI, EDI);
    // Mark buffer full
    _release_eax(&EBX->INS_FULL_SEM);

    return EAX;
}


unsigned _read_first_block(OPEN_STRUCT *EDI, MYI2_STRUCT *EBX)
{
    INPUT_STRUCT *ESI = _get_next_buffer(EBX);

    // Read 16K block, treat .LIB-format files specially
    unsigned EAX = _read_block(ESI, EDI);

    // First block, see if this is a library file
    if (EAX < 16)
	goto NOT_LIB;

    struct LibHeader *h = (struct LibHeader *)ESI->INS_BLOCK;

    if (h->recTyp != 0x0F)		// LIB-type?
	goto NOT_LIB;

    if (h->pagesize >= 512-3)		// pagesize too big?
	goto NOT_LIB;

    // Need to adjust length reported in FN_LENGTH
    if (EDI->OPENFILE_ADDR < h->lSymSeek)
	EDI->OPENFILE_NAME->NFN_FILE_LENGTH = h->lSymSeek;
NOT_LIB:
    // Mark buffer full
    _release_eax(&EBX->INS_FULL_SEM);

    return EAX;
}


unsigned _read_block(INPUT_STRUCT *ESI, OPEN_STRUCT *EDI)
{
    // Return EAX == # of bytes read

    ESI->INS_OPENFILE = EDI;		// mark owning open file

    if (!ESI->INS_BLOCK)	// assign a segment if not there
    {
	// Go get a segment
	ESI->INS_BLOCK = _get_new_phys_blk();
    }

    // Set up for read
    unsigned long nread;

    // read smaller of left and PAGE_SIZE
    unsigned EDX = EDI->OPENFILE_NAME->NFN_FILE_LENGTH - EDI->OPENFILE_ADDR;
    if (EDX > PAGE_SIZE)
	EDX = PAGE_SIZE;
    ESI->INS_BYTES = EDX;

    if (!ReadFile(EDI->OPENFILE_HANDLE, ESI->INS_BLOCK, EDX, &nread, 0))
	_oerr_abort(DOS_READ_ERR, EDI->OPENFILE_NAME->NFN_TEXT);

    unsigned EAX = ESI->INS_BYTES;
    if (EAX != nread)
	_oerr_abort(UNEXP_EOF_ERR, EDI->OPENFILE_NAME->NFN_TEXT);

    EDI->OPENFILE_ADDR += EAX;

    // Return # of bytes read
    return EAX;
}


void _read_16(MYI2_STRUCT *EBX, OPEN_STRUCT *EDI)
{
    // Read library header
    unsigned long result;

    if (!ReadFile(EDI->OPENFILE_HANDLE, &EDI->OPENFILE_HEADER, 16, &result, 0))
	_oerr_abort(DOS_READ_ERR, EDI->OPENFILE_NAME->NFN_TEXT);

    if (result != 16)
	_oerr_abort(UNEXP_EOF_ERR, EDI->OPENFILE_NAME->NFN_TEXT);

    EDI->OPENFILE_ADDR += 16;

    // Copy this data to someplace safe
    memcpy(&EBX->MYI2_TEMP_RECORD, &EDI->OPENFILE_HEADER, 16);

    // Let other thread see it too
    _release_eax(&EBX->OPENFILE_HEADER_SEM);
}

void _readt_abort(MYI2_STRUCT *EBX)
{
    // Close any open handles
    OPEN_STRUCT *p = &EBX->MYI2_OPEN_STRUC[0];
    for (int i = 0; i < 4; i++)
    {	void *h;
	_asm
	{
		mov	ECX,p
		xor	EAX,EAX
		xchg	EAX,[ECX].OPENFILE_HANDLE
		mov	h,EAX
	}
	if (h)
	    _close_handle(h);
	p++;
    }

    // Claim remaining buffers, release segments
    INPUT_STRUCT *ESI = &EBX->MYI2_INPUT_STRUC[0];
    for (int i = 0; i < 4; i++)
    {
	void *h = ESI->INS_BLOCK;	// segment allocated?
	if (h)
	{   ESI->INS_BLOCK = 0;
	    _release_block(h);
	}
	ESI++;
    }

    ExitThread(0);		// stop this thread
}


