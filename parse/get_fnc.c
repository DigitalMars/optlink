
#include "all.h"

unsigned char *_get_filename(CMDLINE_STRUCT *EAX, NFN_STRUCT* ECX, unsigned char *EDX)
{
	// EAX IS FILESTUFF_PTR
	// ECX IS NFN_STRUCT TO USE
	// EDX IS INPTR1 OR SOMETHING


	// RETURNS:
	// 	EAX IS UPDATED INPTR1


	unsigned char *ESI = EDX;
	CMDLINE_STRUCT *EBX = EAX;
	unsigned char AL;
L0:
	AL = *ESI++;
	if (AL == ' ' || AL == 9)
		goto L0;
	--ESI;
	unsigned char *EDI = &ECX->NFN_TEXT[0];
L1:
	AL = *ESI++;
	if (AL == '"')
	{
	  L6:
	    AL = *ESI++;
	    if (AL == '"')
		    goto L1;
	    *EDI++ = AL;
	    if (AL == ' ' || !(FNTBL[AL] & FNTBL_ILLEGAL))
		goto L6;
	}
	else
	{
	    *EDI++ = AL;
	    if (!(FNTBL[AL] & FNTBL_ILLEGAL))
		goto L1;
	}

	--ESI;
	ECX->NFN_TOTAL_LENGTH = EDI - &ECX->NFN_TEXT[1];
	if (ECX->NFN_TOTAL_LENGTH == 0)
	{
	    _err_inbuf_abort(FILE_EXP_ERR);
	}
	else
	{
	    // EBX IS FILESTUFF_PTR, ECX IS NFN_STRUCT
	    _do_filename(ECX, EBX);
	}
	return ESI;
}


