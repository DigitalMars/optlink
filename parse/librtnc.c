
#include "all.h"

extern CMDLINE_STRUCT LIBSTUFF;

unsigned char *_lib_routine_final(unsigned char *EAX)
{
    // EAX IS INPTR1 TO USE

    NFN_STRUCT LRF_NFN_STRUCT;

    unsigned char *p = EAX;

    while (1)
    {
	unsigned char AL;
	do
	{
	    AL = *p++;
	} while (AL == ';' || AL == ',' || AL == ' ');

	if (AL == '\r')
	    break;
	--p;
	if (AL == 0x1A)
	    break;
	p = _get_filename(&LIBSTUFF, &LRF_NFN_STRUCT, p);

	_handle_libs(&LRF_NFN_STRUCT);
    }
    return p;
}

unsigned char *_lib_routine_final_coment(unsigned char *EAX)
{
    // EAX IS INPTR1 TO USE

    unsigned char *p = EAX;

    NFN_STRUCT LRF_NFN_STRUCT;
    p = EAX;
    goto L2;

L1:
    p = _get_filename(&LIBSTUFF, &LRF_NFN_STRUCT, p);

    NFN_STRUCT *ns = &LRF_NFN_STRUCT;
    if (ns->NFN_EXTLEN == 4)
    {
	unsigned char DL = ns->NFN_TEXT[ns->NFN_PATHLEN + ns->NFN_PRIMLEN + 1];
	unsigned char *ECX = &ns->NFN_TEXT[ns->NFN_PATHLEN + ns->NFN_PRIMLEN + 2];

	if (toupper(DL) == 'L' &&
	    toupper(*ECX++) == 'O' &&
	    toupper(*ECX++) == 'D')
	{
	    // HANDLE LIKE A STUB FILE
	    if (LOD_SUPPLIED)
	    {
		_warn_ret(DUP_LOD_ERR);
	    }
	    else
	    {   unsigned char flag;
		_asm
		{   mov		AL,DOSX_EXE_TYPE
		    xchg	AL,EXETYPE_FLAG
		    mov		flag, AL
		}
		if (flag && flag != DOSX_EXE_TYPE)
		    _err_ret(DOSX_NONDOSX_ERR);
		if (EXEPACK_SELECTED)
		    _err_abort(DOSX_EXEPACK_ERR);
		LOD_SUPPLIED = -1;
		_handle_lod(&LRF_NFN_STRUCT);
	    }
	    goto L9;
	}
    }
    // IGNORE LIBSEARCH REQUESTS?
    if (DEFAULTLIBRARYSEARCH_FLAG)
    {
	_handle_libs(&LRF_NFN_STRUCT);
    }
    goto L2;

L2:
    unsigned char AL;
    do
    {
	AL = *p++;
    } while (AL == ';' || AL == ',' || AL == ' ');

    if (AL != '\r')
    {
	--p;
	if (AL != 0x1A)
	    goto L1;
    }
L9:
    return p;
}

#if 1
unsigned char LIB_MSG[] = "\x15Libraries and Paths: ";
unsigned char LIB_EXT[] = "\x04.lib";

int _ret_false();
int _ret_true();

#if 0
int _ret_false()
{
    return 0;
}

int _ret_true()
{
    return 1;
}
#endif

CMDLINE_STRUCT LIBSTUFF = {
	_ret_false,	// DEFAULT TO NUL
	LIB_EXT,	// DEFAULT LIB EXTENSION
	_ret_true,	// YES, THIS IS WANTED
	LIB_MSG,	// IF PROMPTING...
};

#endif

