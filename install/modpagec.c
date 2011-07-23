
#include "all.h"

void _init_install_module()
{
    // build a bit table

    void *p = _get_new_log_blk(); 	// leave in faster memory
    FSYMBEG = p;
    memset(p, 0, 8 * 1024);
    MAX_PAGEDWORD = 0;
}

void _install_module_page(int EAX)
{
    // Install module[EAX] in list of modules

    int EDX = 1 << (EAX & 0x1F);
    EAX >>= 5;
    int *EBX = (int *)FSYMBEG;
    EBX[EAX] |= EDX;
    if (MAX_PAGEDWORD < EAX)
	MAX_PAGEDWORD = EAX;
}

int _binser_module(int EAX, SYMBOL_STRUCT *EBX)
{
	// EAX IS PAGE #, RETURN MODULE #
	//printf("_binser_module(%d, %p)\n", EAX, EBX);

	EAX *= LIB_PAGE_SIZE;
	unsigned EDX = 0;

	unsigned ESI = LIB_MODULES;
	unsigned ECX = EAX;
	if (ESI > PAGE_SIZE / 4)
	    goto L3;

	unsigned EDX2 = EDX;
	void **EBX2 = LS_MODULE_PTR;

	unsigned EDI;

	goto L2;

	// ESI STORES OLD HIGH
	// EDI IS NEW HIGH
	// EDX IS NEW LOW
	// ECX IS VALUE SEARCHING FOR
	// EBX IS BASE

L1:
	EDX += EDI+1;		// NEW LOW
L2:
	EDI = ESI;			// OLD HIGH
	EDI -= EDX;			// DISTANCE BETWEEN TWO POINTS

	EDI >>= 1;			// TAKE HALF OF THAT
	void **px = &EBX2[EDX];
	if (ESI == EDX)
	{   // SAME POINT, NOT FOUND
	    // MAKE DX POINT TO WHERE INSERT BELONGS
	    _library_error(EBX);
        }

	EAX = (unsigned)px[EDI];
	EAX &= 0x7FFFFFFF;

	if (EAX < ECX)
	    goto L1;		// SMALLER, JUMP = 6

	ESI = EDX + EDI;
	if (EAX != ECX)
	    goto L2;		// LARGER, JUMP

	// FOUND IT
	EAX = EDX2;

	unsigned long long x = EAX + ESI;
	if (x > 0xFFFFFFFFLL)
	    _library_error(EBX);
	EAX += ESI;			// DELTA FOR BLOCK #
	return EAX;

L3:
	// MORE THAN PAGE_SIZE/4 MODULES...  FIND CORRECT BLOCK FIRST
	// EDX IS ZERO
	while (1)
	{
	    EAX = (unsigned) EXETABLE[EDX];		// POINTS TO MAXIMUMS
	    ++EDX;
	    if (EAX >= ECX)
		break;
	    if (EDX >= 16)
		_library_error(EBX);
	}

	// BX /4 IS CORRECT BLOCK # TO USE
	// NEED BLOCK# * PAGE_SIZE/4 ON STACK
	// NEED DS:BX AS START OF BLOCK
	// NEED DI AT END OF BLOCK (PAGE_SIZE UNLESS LAST BLOCK...)

	EDI = LIB_MODULE_PTR_BLOCKS;

	--EDX;
	--EDI;

	EBX2 = LS_MODULE_PTR;
	ESI = PAGE_SIZE;
	if (EDX == EDI)
	    ESI = LIB_MODULE_PTR_LAST_OFFSET;
	EBX2 = (void **)EBX2[EDX];

	EDX <<= PAGE_POWER-2;
	ESI >>= 2;
	EDX2 = EDX;
	EDX = 0;
	goto L2;
}
