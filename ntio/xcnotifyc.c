
#include <windows.h>

#include "all.h"

char context_msg[] = "Unexpected OPTLINK Termination at EIP=????????";

char registers_msg[] = "\
EAX=???????? EBX=???????? ECX=???????? EDX=????????\r\n\
ESI=???????? EDI=???????? EBP=???????? ESP=????????\r\n\
First=????????";

#define EAX 4
#define EBX (EAX + 8 + 5)
#define ECX (EBX + 8 + 5)
#define EDX (ECX + 8 + 5)
#define ESI (51 + 2 + 4)
#define EDI (ESI + 8 + 5)
#define EBP (EDI + 8 + 5)
#define ESP (EBP + 8 + 5)
#define FIRST (sizeof(registers_msg) - 9)

int _loutall_con(unsigned nbytes, char *ptr);

EXCEPTION_DISPOSITION XCNOTIFY(
            EXCEPTION_RECORD *exception_record,
            void *EstablisherFrame,
            CONTEXT *context,
            void *dispatcher_context)
{
    static char format[] = "%p";
    char *preg = registers_msg;

    _capture_eax(&_ABORTER_SEM);		// only one thread can GP-fault
    if (context->ContextFlags & CONTEXT_i386)
    {
	sprintf(preg + FIRST, format, &FIRST_LABEL);

	if ((context->ContextFlags & ~CONTEXT_i386) & 1)
	{
	    sprintf(context_msg + sizeof(context_msg) - 9, format, context->Eip);

	    sprintf(preg + EBP, format, context->Ebp);
	    preg[EBP + 8] = ' ';
	    sprintf(preg + ESP, format, context->Esp);
	    preg[ESP + 8] = ' ';
	}
	if ((context->ContextFlags & ~CONTEXT_i386) & 2)
	{
	    sprintf(preg + EAX, format, context->Eax);
	    preg[EAX + 8] = ' ';

	    sprintf(preg + EBX, format, context->Ebx);
	    preg[EBX + 8] = ' ';

	    sprintf(preg + ECX, format, context->Ecx);
	    preg[ECX + 8] = ' ';

	    sprintf(preg + EDX, format, context->Edx);
	    preg[EDX + 8] = ' ';

	    sprintf(preg + ESI, format, context->Esi);
	    preg[ESI + 8] = ' ';

	    sprintf(preg + EDI, format, context->Edi);
	    preg[EDI + 8] = ' ';
	}
    }
#if 1
    _loutall_con(strlen(context_msg), context_msg);
    _loutall_con(1, "\n");
    _loutall_con(strlen(preg), preg);
    _loutall_con(1, "\n");
#else
    MessageBox(0, preg, context_msg, MB_SETFOREGROUND | MB_ICONHAND);
#endif
    ExitProcess(1);
}


