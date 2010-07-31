#include <windows.h>

#include "all.h"

void FORCE_SIGNON();

void _proper_prompt()
{
    _issue_prompt();
    unsigned long count;

    if (!ReadFile(_get_stdin(), TEMP_RECORD + 4, MAX_RECORD_LEN, &count, 0))
	_err_abort(0);

    *(int*)&TEMP_RECORD[0] = count;
    CURN_INPTR = TEMP_RECORD + 4;
    CURN_COUNT = count + 1;

    *(int *)(CURN_INPTR+CURN_COUNT-3) = 0x1A0A0D;
}

extern void *GET_STDIN();

void *_get_stdin()
{
//    return GET_STDIN();
    if (STDIN == INVALID_HANDLE_VALUE)
    {
	STDIN = GetStdHandle(STD_INPUT_HANDLE);
	if (STDIN == INVALID_HANDLE_VALUE)
	    _err_abort(0);
    }
    return STDIN;
}

void _issue_prompt()
{
    FORCE_SIGNON();
    _do_print_defaults();
}


void _do_print_defaults()
{
    NFN_STRUCT MY_FILNAM;
    memset(&MY_FILNAM, 0, sizeof(NFN_STRUCT));
    _do_defaults(&MY_FILNAM, FILESTUFF_PTR);		// set in FILNAM

#if 1
    printf("%.*s(%.*s):", FILESTUFF_PTR->CMD_PMSG[0], FILESTUFF_PTR->CMD_PMSG + 1,
			  MY_FILNAM.NFN_TOTAL_LENGTH, &MY_FILNAM.NFN_TEXT);
#else
    unsigned count = FILESTUFF_PTR->CMD_PMSG[0];
    memcpy(OUTBUF, FILESTUFF_PTR->CMD_PMSG + 1, count);
    OUTBUF[count] = '(';

    memcpy(&OUTBUF[count + 1], &MY_FILNAM.NFN_TEXT, MY_FILNAM.NFN_TOTAL_LENGTH);

    unsigned char *EAX = &OUTBUF[count + 1 + MY_FILNAM.NFN_TOTAL_LENGTH];
    EAX[0] = ')';
    EAX[1] = ':';
    _loutall_con(count + 1 + MY_FILNAM.NFN_TOTAL_LENGTH + 2, (char *)OUTBUF);
#endif
}

