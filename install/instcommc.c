
#include "all.h"


void _common_inst_init(ALLOCS_STRUCT *EBX)
{
    // EBX POINTS TO _STUFF ALLOCS STRUCTURE THAT IS TO BE INITIALIZED WITH A HASH TABLE

    void *EDI = _get_new_log_blk();
    EBX->ALLO_HASH_TABLE_PTR = EDI;
    EBX->ALLO_BLK_LIST[0] = EDI;	// first block
    EBX->ALLO_HASH = PRIME_2K;
    EBX->ALLO_PTR = (char *)EDI + 2 * 1024 * 4;
    EBX->ALLO_CNT = PAGE_SIZE - 2 * 1024 * 4;
    EBX->ALLO_NEXT_BLK = &EBX->ALLO_BLK_LIST[1];
    EBX->ALLO_BLK_CNT = 1;
    memset(EDI, 0, 2 * 1024 * 4);
}
