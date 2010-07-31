
#include "all.h"

int lda_cont(unsigned nbytes,		// # of bytes wanted
	FA_S *EDX, unsigned *pEDX, void* *pECX, FA_BASE_S* *pEAX);

int _farcall_alloc(unsigned EAX, FA_BASE_S* *pEAX, FARCALL_HEADER_TYPE**pECX)
{   unsigned EDX;
    return lda_cont(EAX, &FCA_STUFF, &EDX, (void **)pECX, pEAX);
}

int _linnum_alloc(unsigned EAX, unsigned *pEDX, void* *pECX, FA_BASE_S* *pEAX)
{
    return lda_cont(EAX, &DBG_SYM_STUFF, pEDX, pECX, pEAX);
}

int _ldata_alloc(unsigned EAX, unsigned *pEDX, void* *pECX, FA_BASE_S* *pEAX)
{
    /* RETURNS ECX = PHYS ADDRESS
     *	 EAX = BLOCK BASE
     *
     * IF CARRY SET, EDX IS ACTUAL NUMBER ALLOCATED, NOT ENOUGH...
     *
     * ALLOCATION STRATEGY - KINDA LIKE DOSSEG...
     *
     *	'CODE' SEGMENTS
     *	NOT IN DGROUP
     *	DGROUP MEMBERS
     *	DEBUG INFO
     */
    FA_S *EDX = &DBG_SYM_STUFF;
    if (!(CLASS_TYPE & (SEG_CV_SYMBOLS1 | SEG_CV_TYPES1)))
    {	EDX = &COD_STUFF;
	if (!(CLASS_TYPE & SEG_CLASS_IS_CODE))
	    EDX = &LDA_STUFF;
    }
    return lda_cont(EAX, EDX, pEDX, pECX, pEAX);
}

int _forref_alloc(unsigned EAX, unsigned *pEDX, void* *pECX, FA_BASE_S* *pEAX)
{
    return _ldata_alloc(EAX, pEDX, pECX, pEAX);
}

int _fixupp_alloc(unsigned EAX, unsigned *pEDX, void* *pECX, FA_BASE_S* *pEAX)
{
    return _ldata_alloc(EAX, pEDX, pECX, pEAX);
}

int lda_cont(unsigned nbytes,		// # of bytes wanted
	FA_S *EDX, unsigned *pEDX, void* *pECX, FA_BASE_S* *pEAX)
{

    while (1)
    {
	nbytes = (nbytes + 3) & ~3;

	if (nbytes <= EDX->FA_CNT)
	{
	    *pECX = EDX->FA_PTR;
	    EDX->FA_CNT -= nbytes;
	    (unsigned char *)EDX->FA_PTR += nbytes;
	    EDX->FA_BASE->usage_count++;		// # items this block
	    *pEAX = EDX->FA_BASE;
	    //*pEDX = EDX;	// *pEDX not referenced?
	    return 0;
	}

	if (EDX->FA_CNT)
	{
	    *pECX = EDX->FA_PTR;
	    *pEDX = EDX->FA_CNT;
	    EDX->FA_CNT = 0;
	    EDX->FA_BASE->usage_count++;		// # items this block
	    *pEAX = EDX->FA_BASE;
	    return 1;
	}

	// FA_CNT was zero, need a new block
	FA_BASE_S *EAX = (FA_BASE_S *)_get_new_phys_blk();
	if (EDX->FA_BASE)
	{
	    EDX->FA_BASE->next_ptr = EAX;
	}

	EDX->FA_BASE = EAX;			// set current base
	EAX->usage_count = 0;			// zero items so far
	EAX->next_ptr = 0;			// no next-block yet
	EDX->FA_CNT = PAGE_SIZE - BLOCK_BASE;
	EDX->FA_PTR = (void *)(EAX + 1);
    }
}

