

#include "all.h"


MYI_STRUCT *OPEN_READING_1(FILE_LIST_STRUCT *fls);

/*************************
 * Returns:
 *	EAX	MYI_STRUCT *
 *	NULL	if error
 */
MYI_STRUCT *_open_reading(FILE_LIST_STRUCT *EAX)
{
    if (!_HOST_THREADED)
	return OPEN_READING_1(EAX);

    // Calc current thread - always valid for OBJ's

    // EAX is FILE_LIST_GINDEX
    CURN_FILE_LIST_GINDEX = EAX;

    MYI_STRUCT *ms = &MYI_STUFF[EAX->FILE_LIST_THREAD];
    MYI2_STRUCT *EBX = MYI_STUFF[EAX->FILE_LIST_THREAD].MYI_LOCALS;

    // Get MYNAMS structure
    int i = (EBX->MYI2_NEXT_OPEN_FILE + 1) & 3;

    NFN_STRUCT *ESI = &EBX->MYI2_NAMS[i];
    EBX->MYI2_NEXT_OPEN_FILE = i;

    OPEN_STRUCT *os = &EBX->MYI2_OPEN_STRUC[i];

    EBX->MYI2_LAST_OPENFILE = os;

    // Make sure file open before touching anything in OPENFILE or NFN_STRUC
    _capture_eax(&EBX->OPENFILE_OPEN_SEM);

    // OK, it's open, and I own it

    int flags = os->OPENFILE_FLAGS;
    FILE_LIST_STRUCT *ECX = os->OPENFILE_PATH_GINDEX;

    FILE_LIST_STRUCT *EDI = CURN_FILE_LIST_GINDEX;

    if (ECX)
	EDI->FILE_LIST_PATH_GINDEX = ECX;

    if (!(EDI->FILE_LIST_NFN.NFN_FLAGS & NFN_TIME_VALID))
    {
	EDI->FILE_LIST_NFN.NFN_FLAGS |= NFN_TIME_VALID;
	EDI->FILE_LIST_NFN.NFN_TIME = ESI->NFN_TIME;
	EDI->FILE_LIST_NFN.NFN_FILE_LENGTH = ESI->NFN_FILE_LENGTH;
    }

    unsigned char *m = &MODULE_NAME[4];	// set MODULE_NAME to FILENAME
    unsigned len = ESI->NFN_PRIMLEN;
    *(unsigned *)(m - 4) = len;
    memcpy(m, &ESI->NFN_TEXT[ESI->NFN_PATHLEN], len);

    ms->MYI_BYTE_OFFSET = 0;
    ms->MYI_PHYS_ADDR = 0;
    ms->MYI_COUNT = 0;	// buffer empty
    ms->MYI_BUFCNT = 0;

    ms->MYI_FILLBUF = &_read_16k_threaded;

    OBJ_DEVICE = ms;

    ms->MYI_FILE_LIST_GINDEX = CURN_FILE_LIST_GINDEX;

    if (flags)
	return ms;

    _delete_phantom_path(ESI);

    ubyte DL = ESI->NFN_TYPE;
    if (DL != NFN_OLD_TTYPE && DL != NFN_STUB_TTYPE && DL != NFN_RES_TTYPE)
	_err_nfn_abort(FILE_NOT_FOUND_ERR, ESI);
    _warn_nfn_ret(FILE_NOT_FOUND_ERR, ESI);
    return 0;
}


MYI_STRUCT *OPEN_READING_1(FILE_LIST_STRUCT *fls)
{
    // fls is FILE_LIST_GINDEX
    CURN_FILE_LIST_GINDEX = fls;

    NFN_STRUCT OR_FILNAM;
    NFN_STRUCT *EBX = &OR_FILNAM;

    _move_nfn(EBX, &fls->FILE_LIST_NFN);	// copy to that structure

    unsigned char *EDI = &MODULE_NAME[4];	// set MODULE_NAME to FILENAME
    unsigned len = EBX->NFN_PRIMLEN;
    *(unsigned *)(EDI - 4) = len;
    memcpy(EDI, &EBX->NFN_TEXT[EBX->NFN_PATHLEN], len);
    *(unsigned *)(EDI + len) = 0;

    FILE_LISTS *ECX = &STUBPATH_LIST;
    // OLD AND STUB USE PATH=
    if (EBX->NFN_TYPE == NFN_OLD_TTYPE || EBX->NFN_TYPE == NFN_STUB_TTYPE)
        goto L1;

    // Others use OBJ= and LIB=
    MYI_STRUCT *EAX = _fancy_open_input(EBX, &OBJPATH_LIST);
    if (EAX)
	goto L2;
    ECX = &LIBPATH_LIST;
L1:
    EAX = _fancy_open_input(EBX, ECX);
    if (!EAX)
	goto FNF;
L2:
    OBJ_DEVICE = EAX;

    // Update NFN_FLAGS, NFN_TIME, NFN_DATE, NFN_FILE_LENGTH
    CURN_FILE_LIST_GINDEX->FILE_LIST_NFN.NFN_FLAGS = EBX->NFN_FLAGS;
    CURN_FILE_LIST_GINDEX->FILE_LIST_NFN.NFN_TYPE = EBX->NFN_TYPE;
    CURN_FILE_LIST_GINDEX->FILE_LIST_NFN.NFN_TIME = EBX->NFN_TIME;
    CURN_FILE_LIST_GINDEX->FILE_LIST_NFN.NFN_DATE = EBX->NFN_DATE;
    CURN_FILE_LIST_GINDEX->FILE_LIST_NFN.NFN_FILE_LENGTH = EBX->NFN_FILE_LENGTH;

    return OBJ_DEVICE;

FNF:
    ubyte DL = EBX->NFN_TYPE;
    // OLD, STUB, RES warning
    if (DL == NFN_OLD_TTYPE || DL == NFN_STUB_TTYPE)
        goto FNF1;
    if (DL == NFN_RES_TTYPE)
        goto FNF2;

    _err_nfn_abort(FILE_NOT_FOUND_ERR, EBX);	// OBJ error

FNF2:
    if (RC_GUESSED)		// don't warn if I just guessed at a filename anyway
	goto FNF3;
FNF1:
    _warn_nfn_ret(FILE_NOT_FOUND_ERR, EBX);
FNF3:
    return 0;
}

