
#include <windows.h>

#include "all.h"

void _seg_released()
{
    _err_abort(SEG_ALREADY_RELEASED_ERR);
}

void _oom_error()
{
    void *a;

    _asm
    {
	xor	EAX,EAX
	xchg	EAX,SLR_STRUCT_BASE
	mov	a,EAX
    }
    if (a)
	_release_large_segment(a);

    _asm
    {
	xor	EAX,EAX
	xchg	EAX,SLR_DICTIONARY_BASE
	mov	a,EAX
    }
    if (a)
	_release_large_segment(a);

    _capture_eax(&OOM_ERROR_SEM);
    _disable_mapout();
    _err_abort(OOM_ERR);
}

void *_sbrk()
{
    void *p = VirtualAlloc(0, 0x4000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!p && !(_FINAL_ABORTING & 0xFF))
	_oom_error();
    return p;
}

void *_get_new_phys_blk()
{
    // Allocate a PAGE_SIZE block.
    // Returns EAX = address
    // We keep our own 'cache' of blocks hopefully for faster alloc and release

    _capture_eax(&GLOBALALLOC_SEM);
    int n = PHYS_TABLE_ENTRIES - 1;
    if (n < 0)
    {
	_release_eax(&GLOBALALLOC_SEM);
	void *p = _sbrk();
	//printf("_get_new_phys_blk() sbrk %p\n", p);
	return p;
    }
    PHYS_TABLE_ENTRIES = n;
    void *p = PHYS_TABLE[0];
    if (n)
	memcpy(&PHYS_TABLE[0], &PHYS_TABLE[1], n * sizeof(void*));
    _release_eax(&GLOBALALLOC_SEM);
    //printf("_get_new_phys_blk() %p\n", p);
    return p;
}

void *_get_new_log_blk()
{
    return _get_new_phys_blk();
}

void *_get_new_io_log_blk()
{
    return _get_new_phys_blk();
}

void *_get_4k_segment()
{
    void *p = VirtualAlloc(0, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!p)
	_oom_error();
    return p;
}

void _release_4k_segment(void *p)
{
    VirtualFree(p, 0, MEM_RELEASE);	// release all
}

void _release_block(void *EAX)
{
    // EAX IS BLOCK TO RELEASE
    _capture_eax(&GLOBALALLOC_SEM);
    int ECX = PHYS_TABLE_ENTRIES;
    if (ECX == 8)
    {
	_release_eax(&GLOBALALLOC_SEM);
	VirtualFree(EAX, 0, MEM_RELEASE);	// release all
    }
    else
    {
	PHYS_TABLE[ECX] = EAX;
	ECX++;
	PHYS_TABLE_ENTRIES = ECX;
	_release_eax(&GLOBALALLOC_SEM);
    }
}

void _release_io_segment(void *EAX)
{
    _release_block(EAX);
}

void _release_segment(void *EAX)
{
    _release_block(EAX);
}

void _release_io_block(void *EAX)
{
    _release_block(EAX);
}


