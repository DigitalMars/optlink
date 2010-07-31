
#include <windows.h>

#include "all.h"

WIN32_FIND_DATA WIN32FINDDATA;
HANDLE FINDNEXT_HANDLE = INVALID_HANDLE_VALUE;

static NFN_STRUCT *process_file(NFN_STRUCT *ESI);

NFN_STRUCT *_do_findfirst(NFN_STRUCT* EAX)
{
    // Find first matching file and return it

    FINDNEXT_HANDLE = FindFirstFile((char *)EAX->NFN_TEXT, &WIN32FINDDATA);
    if (FINDNEXT_HANDLE != INVALID_HANDLE_VALUE)
	return process_file(EAX);
    return NULL;
}


NFN_STRUCT *_do_findnext(NFN_STRUCT *EAX)
{
	int h = FindNextFile(FINDNEXT_HANDLE, &WIN32FINDDATA);
	if (!h)
	{
	    return NULL;
	}
	return process_file(EAX);
}

static NFN_STRUCT *process_file(NFN_STRUCT *ESI)
{
    ESI->NFN_FILE_LENGTH = WIN32FINDDATA.nFileSizeLow;

    unsigned char *ECX = (unsigned char *)&WIN32FINDDATA.cFileName[0];
    unsigned char *EDX = &ESI->NFN_TEXT[ESI->NFN_PATHLEN];

    // move till 0
    char c;
    do
    {
	c = *ECX++;
	*EDX++ = c;
    } while (c);

    ECX = &ESI->NFN_TEXT[0] + 1;
    *((int*)EDX) = 0;

    ESI->NFN_TOTAL_LENGTH = EDX - ECX;

    NFN_STRUCT *s = _parse_filename(ESI);
    ESI->NFN_FLAGS |= NFN_TIME_VALID | NFN_AMBIGUOUS;
    return s;
}

void _close_findnext()
{
    if (FINDNEXT_HANDLE != INVALID_HANDLE_VALUE)
    {
	FindClose(FINDNEXT_HANDLE);
	FINDNEXT_HANDLE = INVALID_HANDLE_VALUE;
    }
}

#if 0
__declspec(naked) void DO_FINDFIRST()
{
    __asm
    {
	push	EAX
	call	_do_findfirst
	pop	ECX
	ret
    }
}

__declspec(naked) void DO_FINDNEXT()
{
    __asm
    {
	push	EAX
	call	_do_findnext
	pop	ECX
	ret
    }
}


__declspec(naked) void CLOSE_FINDNEXT()
{
    __asm
    {
	call	_close_findnext
	ret
    }
}
#endif


