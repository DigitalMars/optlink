// MVFNASC - Copyright (c) SLR Systems 1994

#include "all.h"


void _move_fn_to_asciz(NFN_STRUCT* EAX)
{
    // MOVE FILNAM [EAX] TO ASCIZ STRING

    unsigned ECX = EAX->NFN_TOTAL_LENGTH;
    ASCIZ_LEN = ECX;	// length in bytes, not 0
    memcpy(ASCIZ, EAX->NFN_TEXT, ECX);
    *(int*)(ASCIZ + ECX) = 0;
}
