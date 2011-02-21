
#include "all.h"

extern unsigned BYTES_SO_FAR;
extern unsigned CV_HEADER_LOC;
extern unsigned char CV_TEMP_RECORD;
extern unsigned char ZEROS_16;

void _flush_cv_temp(unsigned char *EDI)
{
    if (EDI != &CV_TEMP_RECORD)
    {	_xdebug_write(EDI);
	EDI = &CV_TEMP_RECORD;
    }
}

void _cv_dword_align_rtn()
{
    unsigned ECX = -BYTES_SO_FAR & 3;
    if (ECX)
	_xdebug_normal(&ZEROS_16, ECX);
}

void _big_xdebug_write(unsigned EAX, void* *ECX)
{
	// ECX IS BLOCK TABLE
	// EAX IS BYTE COUNT

	if (EAX)
	{
		void* *EBX = ECX;
		unsigned count = EAX;
L1:
		void *p = *EBX;
		*EBX = 0;

		unsigned n = PAGE_SIZE;
		if (count <= n)
		    n = count;
		_xdebug_normal(p, n);

		count -= n;
		++EBX;
		_release_block(p);
		if (count)
		    goto L1;
		return;
	}
	void *a = (void *)EAX;
	void **c = ECX;
	__asm
	{
		mov	EAX,a
		mov	ECX,c
		xchg	[ECX],EAX
		mov	a,EAX
	}
	if (a)
	    _release_block(a);
}

void _xdebug_write(unsigned char *EDI)
{
    // MOVE DATA FROM TEMP_RECORD TO FINAL
    _xdebug_normal(&CV_TEMP_RECORD, EDI - &CV_TEMP_RECORD);
}

void _xdebug_normal(void *EAX, unsigned ECX)
{
	unsigned EDX = BYTES_SO_FAR;
	BYTES_SO_FAR += ECX;
	EDX += CV_HEADER_LOC;
	_move_eax_to_edx_final(EAX, ECX, EDX);
}
