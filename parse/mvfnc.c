
#include "all.h"

NFN_STRUCT *_move_nfn(NFN_STRUCT* EAX, NFN_STRUCT* ECX)
{
    // EAX IS TARGET NFN_STRUCT, ECX IS SOURCE
    return memcpy(EAX, ECX,
//	sizeof(NFN_STRUCT) - NFN_TEXT_SIZE + ECX->NFN_TOTAL_LENGTH + 4);
// Not sure if it needs to be 0 terminated. The +4 was so that the asm version could
// always move by 4 byte increments
	sizeof(NFN_STRUCT) - NFN_TEXT_SIZE + ECX->NFN_TOTAL_LENGTH + 1);
}

