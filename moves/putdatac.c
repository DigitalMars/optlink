
#include "all.h"

void _put_data(void *ps, unsigned count, MYO_STRUCT* pm)
{
    // PUT DATA TO ANY OUTPUT STRUCTURE POINTED TO BY BX
    // FLUSH IF BUFFER IS FULL

    // ps IS SOURCE
    // count IS BYTE COUNT
    // pm IS BUFFER STRUCTURE

    if (count && pm)
    {
	pm->MYO_BYTE_OFFSET = pm->MYO_BYTE_OFFSET + count;
	while (1)
	{
	    if (pm->MYO_COUNT >= count)
	    {	pm->MYO_COUNT -= count;
		break;
	    }

	    if (pm->MYO_COUNT)
	    {
		memcpy(pm->MYO_PTR, ps, pm->MYO_COUNT);
		pm->MYO_PTR += pm->MYO_COUNT;
		count -= pm->MYO_COUNT;
		pm->MYO_COUNT = 0;
	    }
	    pm->MYO_FLUSHBUF(pm);
	}

	memcpy(pm->MYO_PTR, ps, count);
	pm->MYO_PTR += count;
    }
}

void _imp_out(void *EAX, unsigned ECX)
{
    _put_data(EAX, ECX, IMP_DEVICE);
}

void _lst_out(void *EAX, unsigned ECX)
{
    // EAX IS POINTER
    // ECX IS BYTE COUNT

    // ERRORS AND WRITEMAP MAY BOTH WANT..
    _capture_eax(&PUT_DATA_SEM);
    _put_data(EAX, ECX, MAP_DEVICE);
    _release_eax(&PUT_DATA_SEM);
}


