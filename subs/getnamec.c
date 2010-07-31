

#include "all.h"

int _get_omf_name_length_routine(unsigned char **pp)
{
    unsigned char *p = *pp;
    int n = *p++;
    if (n == 0xFF && *p == 0)
    {
	n = *(short *)(p + 1);
	p += 3;
	if (n > SYMBOL_TEXT_SIZE)
	    _err_abort(TOO_LONG_ERR);
    }
    *pp = p;
    return n;
}

void xTOO_LONG()
{
    _err_abort(TOO_LONG_ERR);
}

/*
GET_OMF_NAME_LENGTH_EDI	PROC
		;
		;
		;
		XOR	EAX,EAX

		MOV	AL,[EDI]
		INC	EDI

		CMP	AL,-1
		JZ	L1$
L2$:
		RET

L1$:
		CMP	BPTR [EDI],AH
		JNZ	L2$

		MOV	AX,[EDI+1]
		ADD	EDI,3

		CMP	EAX,SYMBOL_TEXT_SIZE
		JA	TOO_LONG

		RET

GET_OMF_NAME_LENGTH_EDI	ENDP
*/
