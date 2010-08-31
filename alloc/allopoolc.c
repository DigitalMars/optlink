
#include "all.h"

void *_get_new_phys_blk();
void **_allo_pool_get1(unsigned, ALLOCS_STRUCT *);

void _ap_fix(unsigned EAX, ALLOCS_STRUCT *EBX, unsigned ECX)
{
    // NEED A BLOCK TO ALLOCATE FROM

    //FIRST, SEE IF LEFT-OVER CAN BE USED AS SUB-ALLOC

    // EAX is byte count desired
    ECX += EAX;

    if (ECX >= 32)
    {
	if (EBX->ALLO_SUB_CNT <= ECX)
	{
	    // this is better than old SUB_PTR
	    EBX->ALLO_SUB_CNT = ECX;
	    EBX->ALLO_SUB_PTR = EBX->ALLO_PTR;
	}
    }
    EBX->ALLO_PTR = _get_new_phys_blk();
    EBX->ALLO_BLK_CNT++;
    EBX->ALLO_CNT = PAGE_SIZE;

    void **p = EBX->ALLO_NEXT_BLK;
    if (p == 0)
	p = &EBX->ALLO_BLK_LIST[0];
    *p = EBX->ALLO_PTR;
    p++;
    EBX->ALLO_NEXT_BLK = p;

    if (p == &EBX->ALLO_BLK_LIST[8])
    {
	// store away these pointers

	void **EAX = _allo_pool_get1(36, EBX);
	*EAX = (void *)EBX->ALLO_LAST_LIST;
	EBX->ALLO_LAST_LIST = EAX;
	memcpy(EAX + 1, &EBX->ALLO_BLK_LIST[0], 8 * sizeof(void*));

	EBX->ALLO_NEXT_BLK = &EBX->ALLO_BLK_LIST[0];
	memset(&EBX->ALLO_BLK_LIST[0], 0, 8 * sizeof(void*));
    }
}

/************************************
 * Input: EAX = # of bytes to allocate
 * Returns: allocated block
 */

void **_allo_pool_get1(unsigned EAX, ALLOCS_STRUCT *EBX)
{
    EAX = (EAX + 3) & ~3;
    while (1)
    {
	if (EAX <= EBX->ALLO_SUB_CNT)
	{
	    // We can use a left-over piece
	    void *p = EBX->ALLO_SUB_PTR;
	    EBX->ALLO_SUB_CNT -= EAX;
	    EBX->ALLO_SUB_PTR = (char *)p + EAX;
	    return (void**)p;
	}

	if (EAX <= EBX->ALLO_CNT)
	{
	    void *p = EBX->ALLO_PTR;
	    EBX->ALLO_CNT -= EAX;
	    EBX->ALLO_PTR = (char *)p + EAX;
	    return (void**)p;
	}

	_ap_fix(EAX, EBX, EBX->ALLO_CNT - EAX);
    }
}


void **_cv_ltype_pool_get(unsigned nbytes)
{
    if (nbytes <= PAGE_SIZE - 8)
	return _allo_pool_get1(nbytes, &CV_LTYPE_STUFF);

    void *EAX = _get_large_segment(nbytes);

    void* EBX = CV_LTYPE_SPECIAL_BLOCK;
    if (!EBX)
    {
	EBX = _get_new_phys_blk();
	CV_LTYPE_SPECIAL_BLOCK = EBX;
    }
    (unsigned char *)EBX += CV_LTYPE_OVERSIZE_CNT;
    CV_LTYPE_OVERSIZE_CNT += 4;
    *(void* *)EBX = EAX;

    return (void **)EAX;
}



void **_cv_gtype_pool_get(unsigned nbytes)
{
    if (nbytes <= PAGE_SIZE - 8)
	return _allo_pool_get1(nbytes, &CV_GTYPE_STUFF);

    void *EAX = _get_large_segment(nbytes);

    void* EBX = CV_GTYPE_SPECIAL_BLOCK;
    if (!EBX)
    {
	EBX = _get_new_phys_blk();
	CV_GTYPE_SPECIAL_BLOCK = EBX;
    }
    (unsigned char *)EBX += CV_GTYPE_OVERSIZE_CNT;
    CV_GTYPE_OVERSIZE_CNT += 4;
    *(void* *)EBX = EAX;

    return (void **)EAX;
}

void **_segment_pool_get(unsigned nbytes)
{
    return _allo_pool_get1(nbytes, &SEGMENT_STUFF);
}

void **_text_pool_alloc(unsigned nbytes)
{
    return _allo_pool_get1(nbytes, &SEGMENT_STUFF);
}

void **_p1only_pool_get(unsigned nbytes)
{
    return _allo_pool_get1(nbytes, &P1ONLY_STUFF);
}

void **_cv_gsym_pool_get(unsigned nbytes)
{
    return _allo_pool_get1(nbytes, &CV_GSYM_STUFF);
}

void **_cv_ssym_pool_get(unsigned nbytes)
{
    return _allo_pool_get1(nbytes, &CV_SSYM_STUFF);
}


