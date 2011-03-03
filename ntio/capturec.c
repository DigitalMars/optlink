
#include <windows.h>

#include "all.h"

void _init_semaphores()
{
    // SETUP GLOBAL SEMAPHORES FOR FAST USAGE
    if (_HOST_THREADED & 0xFF)
    {
	for (int i = 0; i < GLOBAL_SEMAPHORE_COUNT; i++)
	{
	    _init_eax(&GLOBAL_SEMAPHORE_LIST[i]);
	}
    }
}


void _init_eax(GLOBALSEM_STRUCT *EAX)
{
    void *h = CreateSemaphore(0,	// no security
		0,			// initial count
		1,			// max count
		0);			// no name

    if (!h)
    {
	_err_abort(CREATESEM_ERR);
    }
    EAX->_SEM_COUNTER = -1;
    EAX->_SEM_ITSELF = h;
    //if (EAX != &_SAY_VERBOSE_SEM) printf("sem init %p, %p\n", EAX, h);
}

void _close_semaphore(GLOBALSEM_STRUCT *s)
{
    void *h = s->_SEM_ITSELF;
    //if (s != &_SAY_VERBOSE_SEM && h) printf("sem close %p, %p\n", s, s->_SEM_ITSELF);
    s->_SEM_ITSELF = 0;
    if (h)
	_close_handle(h);
}

void _capture_eax(struct GLOBALSEM_STRUCT *s)
{
//if (s == &_SAY_VERBOSE_SEM) return;
//if (s != &_SAY_VERBOSE_SEM) printf("capture %p, h = %p, counter = %d\n", s, s->_SEM_ITSELF, s->_SEM_COUNTER);
//if (s != &_SAY_VERBOSE_SEM && s->_SEM_ITSELF == (HANDLE)0x6C8) printf("capture %p, h = %p, counter = %d\n", s, s->_SEM_ITSELF, s->_SEM_COUNTER);
//if (s != &_SAY_VERBOSE_SEM /*&& s->_SEM_ITSELF == (HANDLE)0x6C8*/ && s->_SEM_COUNTER == -10)  *(char*)0=0;
#if 1
    _asm
    {
	mov	EAX,s
	lock	inc	dword ptr [EAX]._SEM_COUNTER
	jz	L1
	js	L1
    }
#endif
    if (s->_SEM_ITSELF)
    {
//if (s != &_SAY_VERBOSE_SEM && s->_SEM_ITSELF == (HANDLE)0x6C8) printf("capturing %p\n", s);
	if (WaitForSingleObject(s->_SEM_ITSELF, -1) == -1)
	{
	    _err_abort(_TIMEOUT_ERR);
	}
//if (s != &_SAY_VERBOSE_SEM && s->_SEM_ITSELF == (HANDLE)0x6C8) printf("captured %p\n", s);
    }
L1:
    ;
}


void _release_eax(struct GLOBALSEM_STRUCT *s)
{
//if (s == &_SAY_VERBOSE_SEM) return;
//if (s != &_SAY_VERBOSE_SEM) printf("release %p, h = %p, counter = %d\n", s, s->_SEM_ITSELF, s->_SEM_COUNTER);
//if (s != &_SAY_VERBOSE_SEM && s->_SEM_ITSELF == (HANDLE)0x6C8) printf("release %p, h = %p, counter = %d\n", s, s->_SEM_ITSELF, s->_SEM_COUNTER);
#if 1
    _asm
    {
	mov	EAX,s
	lock	dec	dword ptr [EAX]._SEM_COUNTER
	jns	L1
    }
    return;
#endif
L1:
    if (s->_SEM_ITSELF)
    {
//if (s != &_SAY_VERBOSE_SEM && s->_SEM_ITSELF == (HANDLE)0x6C8) printf("releasing %p\n", s);
	ReleaseSemaphore(s->_SEM_ITSELF, 1, 0);
    }
}


void _release_eax_bump(struct GLOBALSEM_STRUCT *s)
{
//if (s == &_SAY_VERBOSE_SEM) return;
//if (s != &_SAY_VERBOSE_SEM) printf("release_bump %p, h = %p, counter = %d\n", s, s->_SEM_ITSELF, s->_SEM_COUNTER);
//if (s != &_SAY_VERBOSE_SEM && s->_SEM_ITSELF == (HANDLE)0x6C8) printf("release_bump %p, h = %p, counter = %d\n", s, s->_SEM_ITSELF, s->_SEM_COUNTER);
#if 1
    _asm
    {
	mov	EAX,s
	lock	dec	dword ptr [EAX]._SEM_COUNTER
	jns	L1
    }
    return;
#endif
L1:
    if (s->_SEM_ITSELF)
    {
//if (s != &_SAY_VERBOSE_SEM && s->_SEM_ITSELF == (HANDLE)0x6C8) printf("release_bump %p\n", s);
	ReleaseSemaphore(s->_SEM_ITSELF, 1, 0);
	_do_dossleep_0();
    }
}
