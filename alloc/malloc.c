#include <windows.h>

HANDLE heap;

void *malloc(unsigned size)
{
    if (!heap)
	heap = GetProcessHeap();
    return HeapAlloc(heap, 0, size);
}

void *calloc(unsigned count, unsigned each)
{
    if (!heap)
	heap = GetProcessHeap();
    return HeapAlloc(heap, HEAP_ZERO_MEMORY, count * each);
}

void *realloc(void *oldp, unsigned size)
{
    if (!heap)
	heap = GetProcessHeap();
    if (!oldp)
        return malloc(size);
    if (!size)
    {   free(oldp);
        return NULL;
    }
    return HeapReAlloc(heap, 0, oldp, size);
}

void free(void *p)
{
    if (p)
        HeapFree(heap, 0, p);
}
