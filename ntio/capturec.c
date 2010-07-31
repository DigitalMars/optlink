
#include <windows.h>

#include "all.h"

void _capture_eax(struct GLOBALSEM_STRUCT *s)
{
    _asm
    {
	mov	EAX,s
	lock	inc	dword ptr [EAX]._SEM_COUNTER
	jz	L1
	js	L1
    }

    if (s->_SEM_ITSELF)
    {
	if (WaitForSingleObject(s->_SEM_ITSELF, -1) == -1)
	{
	    _err_abort(_TIMEOUT_ERR);
	}
    }
L1:
    ;
}


void _release_eax(struct GLOBALSEM_STRUCT *s)
{
    _asm
    {
	mov	EAX,s
	lock	dec	dword ptr [EAX]._SEM_COUNTER
	jns	L1
    }
    return;

L1:
    if (s->_SEM_ITSELF)
    {
	ReleaseSemaphore(s->_SEM_ITSELF, 1, 0);
    }
}


void _release_eax_bump(struct GLOBALSEM_STRUCT *s)
{
    _asm
    {
	mov	EAX,s
	lock	dec	dword ptr [EAX]._SEM_COUNTER
	jns	L1
    }
    return;

L1:
    if (s->_SEM_ITSELF)
    {
	ReleaseSemaphore(s->_SEM_ITSELF, 1, 0);
	_do_dossleep_0();
    }
}
