
#include <windows.h>

#include "all.h"

static ubyte TERR_FLAG;

void _oerr_abort(int EAX, unsigned char *ECX)
{
    _terr_abort(EAX, ECX);
}

void _terr_abort(int errnum, unsigned char *ESI)
{
    // ESI is ASCIZ, errnum is error message #
    ubyte flag;
    _asm
    {
	mov	DL,-1
	xchg	TERR_FLAG,DL
	mov	flag,DL
    }

    if (flag)		// someone already doing thread-error-abort
	ExitThread(0);	// just quit

    _capture_eax(&ERROR_SEM);		// non-reentrant error handling

#if 1
    ++_ERR_COUNT;
    errnum &= 0xFF;
    ERR_NUMBER = errnum;
    char *p = ERR_TABLE[errnum];
    printf("%s : Error %d: %.*s \r\n", ESI, errnum, *p & 0x7F, p + 1);
#else
    EDI = &EOUTBUF;
    while ((*EDI++ = *ESI++) != 0)
	;
    EDI--;
    EDI[0] = ' ';
    EDI[1] = ':';
    EDI[2] = ' ';
    EDI += 3;

    ++_ERR_COUNT;

    memcpy(EDI, " Error ", 7);
    EDI += 7;

    errnum &= 0xFF;

    ERR_NUMBER = errnum;
    EDI = _cbta16(errnum, EDI);

    EDI[0] = ':';
    EDI[1] = ' ';
    EDI += 2;
    ESI = ERR_TABLE[errnum];

    n = *ESI++ & 0x7F;
    memcpy(EDI, ESI, n);
    EDI += n;
    EDI[0] = ' ';
    EDI[1] = '\r';
    EDI[2] = '\n';
    EDI += 3;
    _loutall_con(EDI - &EOUTBUF, &EOUTBUF);
#endif
    _release_eax(&ERROR_SEM);
    _abort();
}


