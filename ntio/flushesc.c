
#include "all.h"

static MYO_STRUCT *FTC_1(MYO_STRUCT *EAX, int flags)
{
    if (EAX)
    {
	unsigned char *ECX = EAX->MYO_PTR;
	unsigned count = (unsigned)ECX;
	unsigned char *EDX = EAX->MYO_BLOCK;	// is there anything to flush?
	if (EDX)
	{
	    flags |= F_RELEASE_BLOCK;
	    count = ECX - EDX;
	    EAX->MYO_PTR = 0;
	}
	EAX->MYO_SPEC_FLAGS |= flags;
#if 0
printbyte(count);
print("\r\n");
if (count)
{
    if (count)
    {
	print("  ");
	for (int i = 0; i < count; i++)
	    printbyte(EDX[i]);
	print("\r\n");
    }
}
#endif
	EAX = _do_doswrite(EAX, count);
	EAX->MYO_COUNT = 0;
    }
    return EAX;
}

MYO_STRUCT *_flush_trunc(MYO_STRUCT *myo)
{
    return FTC_1(myo, F_TRUNC_FILE | F_CLEAR_BLOCK);
}

MYO_STRUCT *_flush_close(MYO_STRUCT *myo)
{   //print("_flush_close\r\n");
    return FTC_1(myo, F_CLOSE_FILE | F_CLEAR_BLOCK);
}

MYO_STRUCT *_flush_trunc_close(MYO_STRUCT *myo)
{
    //print("_flush_trunc_close\r\n");
    return FTC_1(myo, F_TRUNC_FILE | F_CLOSE_FILE | F_CLEAR_BLOCK);
}

#if 1
void *GET_NEW_IO_LOG_BLK();
//__declspec(naked)
MYO_STRUCT *_flush_buffer(MYO_STRUCT *myo)
{
#if 0
    __asm
    {
                mov     EAX,4[ESP]
                mov     ECX,04000h
                mov     EDX,[EAX]
                mov     [EAX],ECX
                sub     ECX,EDX
                mov     EDX,0Ch[EAX]
                je      L60
                test    EDX,EDX
                je      L64
                mov     8[EAX],EDX
                push    EDX
                push    ECX
                push    EAX
                call    near ptr _doswrite
                add     ESP,0Ch
L5F:            ret
L60:            test    EDX,EDX
                jne     L5F
L64:            push    EBX
                mov     EBX,EAX
                call    near ptr GET_NEW_IO_LOG_BLK
                mov     0Ch[EBX],EAX
                mov     8[EBX],EAX
                mov     EAX,EBX
                pop     EBX
                ret
    }
#else
    unsigned ECX = PAGE_SIZE - myo->MYO_COUNT;
    myo->MYO_COUNT = PAGE_SIZE;
    void *EDX = myo->MYO_BLOCK;
    if (EDX)
    {
	if (ECX)
	{
	    myo->MYO_PTR = EDX;
	    return _doswrite(myo, ECX, EDX);
	}
    }
    else
    {
	void *EAX = GET_NEW_IO_LOG_BLK(); 	// leave in fast
	myo->MYO_BLOCK = EAX;
	myo->MYO_PTR = EAX;
    }
    return myo;
#endif
}

#endif

