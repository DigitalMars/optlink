// MVSRCFIL - Copyright (c) SLR Systems 1994

#include "all.h"

void _move_ecxprim_to_eax(NFN_STRUCT* EAX, NFN_STRUCT* ECX);

#if 0
void _move_srcprim_to_eax_clean(NFN_STRUCT* EAX)
{
    EAX->NFN_PRIMLEN = 0;
    EAX->NFN_PATHLEN = 0;
    EAX->NFN_EXTLEN = 0;
    EAX->NFN_TOTAL_LENGTH = 0;
    _move_srcprim_to_eax(EAX);
}
#endif

void _move_srcprim_to_eax(NFN_STRUCT* EAX)
{
    // MOVE PRIMARY PART OF SRCNAM TO FILNAM

    _move_ecxprim_to_eax(EAX, &SRCNAM);
}

void _move_ecxprim_to_eax(NFN_STRUCT* EAX, NFN_STRUCT* ECX)
{
    NFN_STRUCT *dst = EAX;
    NFN_STRUCT *src = ECX;

    // Make room
    memmove(&dst->NFN_TEXT[dst->NFN_PATHLEN + src->NFN_PRIMLEN],
	    &dst->NFN_TEXT[dst->NFN_PATHLEN + dst->NFN_PRIMLEN],
	    dst->NFN_EXTLEN);

    // Copy path
    memcpy(&dst->NFN_TEXT[dst->NFN_PATHLEN],
	   &src->NFN_TEXT[src->NFN_PATHLEN],
	   src->NFN_PRIMLEN);

    dst->NFN_TOTAL_LENGTH += src->NFN_PRIMLEN - dst->NFN_PRIMLEN;
    dst->NFN_PRIMLEN = src->NFN_PRIMLEN;

    *(int*)&dst->NFN_TEXT[dst->NFN_TOTAL_LENGTH] = 0;
//    return dst;
}

