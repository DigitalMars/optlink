
#include "all.h"

int toupper(int c)
{
    return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}

unsigned strlen(const char *p)
{
    unsigned len = 0;
    while (*p++)
    {
	len++;
    }
    return len;
}

void *memset(void *p, int value, unsigned n)
{
    while (n--)
    {
	((unsigned char*)p)[n] = value;
    }
}

void *memcpy(void *dst, const void *src, unsigned n)
{
    for (unsigned i = 0; i < n; i++)
	((char *)dst)[i] = ((const char *)src)[i];
    return dst;
}

void *memmove(void *dst, const void *src, unsigned n)
{
    if (dst < src)
    {
	for (unsigned i = 0; i < n; i++)
	    ((char *)dst)[i] = ((const char *)src)[i];
    }
    else
    {
	for (unsigned i = n; i--; )
	    ((char *)dst)[i] = ((const char *)src)[i];
    }
    return dst;
}

NFN_STRUCT *_move_ecxpath_eax(NFN_STRUCT* EAX, NFN_STRUCT* ECX)
{
	// MOVE PATH FROM ECX TO EAX, PRESERVE EAX

	NFN_STRUCT *dst = EAX;
	NFN_STRUCT *src = ECX;

	// Make room
	memmove(&dst->NFN_TEXT[src->NFN_PATHLEN],
		&dst->NFN_TEXT[dst->NFN_PATHLEN],
		dst->NFN_TOTAL_LENGTH - dst->NFN_PATHLEN);

	// Copy path
	memcpy(dst->NFN_TEXT, src->NFN_TEXT, src->NFN_PATHLEN);

	dst->NFN_TOTAL_LENGTH += src->NFN_PATHLEN - dst->NFN_PATHLEN;
	dst->NFN_PATHLEN = src->NFN_PATHLEN;

	*(int*)&dst->NFN_TEXT[dst->NFN_TOTAL_LENGTH] = 0;
	return dst;
}

