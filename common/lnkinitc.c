
#include "all.h"

#include <windows.h>

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
}

