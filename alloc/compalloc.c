
#include <windows.h>

#include "all.h"


void *_get_large_segment(int EAX)
{
    void *p = VirtualAlloc(0, EAX, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!p)
	_oom_error();
    return p;
}
