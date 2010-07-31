
#include "all.h"


void _release_minidata(ALLOCS_STRUCT *a)
{
    if (!a->ALLO_BLK_CNT)
	return;

    while (1)
    {
	for (int i = 0; i < 8; i++)
	{
	    void *p = a->ALLO_BLK_LIST[i];
	    if (!p)
		    break;
	    _release_block(p);
	}

	// ANY MORE LISTS OF BLOCKS?
	void **ESI = a->ALLO_LAST_LIST;
	if (!ESI)
	    break;
	a->ALLO_LAST_LIST = (void**)(*ESI++);
	memcpy(&a->ALLO_BLK_LIST[0], ESI, 8 * sizeof(int));
    }

    memset(a, 0, sizeof(ALLOCS_STRUCT));
}
