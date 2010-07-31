
#include <windows.h>

#include "all.h"

void _release_large_segment(void *p)
{
	int result = VirtualFree(p, 0, MEM_RELEASE);
	if (!result)
	    _seg_released();
}
