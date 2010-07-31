// MVLISTNF - Copyright (c) SLR Systems 1994

#include "all.h"

//__declspec(naked)
void _move_file_list_gindex_nfn(NFN_STRUCT* EAX, FILE_LIST_STRUCT *ECX)
{
	//printf("_move_file_list_gindex_nfn()\r\n");
	// MOVE COMPLETE NFN FROM FILE LIST TO NFN STRUCTURE

	// ECX IS FILE_LIST_GINDEX
#if 0
    _asm
    {
		mov	ECX,8[ESP]
		mov	EAX,4[ESP]

		mov	EDX,EAX
		mov	EAX,ECX
		push	EDI
		push	ESI
		mov	ECX,034h[EAX]
		mov	EDI,EDX
		add	ECX,020h
		lea	ESI,01Ch[EAX]
		shr	ECX,2
		mov	EAX,8[EAX]
		rep	movsd
		test	EAX,EAX
		je	L2C
		lea	ECX,01Ch[EAX]
		mov	EAX,EDX
		push	ECX
		push	EAX
		call	near ptr _move_ecxpath_eax
		add	ESP,8
L2C:		pop	ESI
		pop	EDI
		ret
    }
#else
	unsigned len = ECX->FILE_LIST_NFN.NFN_TOTAL_LENGTH +
		sizeof(NFN_STRUCT) - NFN_TEXT_SIZE + 4;

	NFN_STRUCT *ESI = &ECX->FILE_LIST_NFN;
	FILE_LIST_STRUCT *fls = ECX->FILE_LIST_PATH_GINDEX;
	memcpy(EAX, ESI, (len / 4) * 4);

	if (fls)
	{
	    _move_ecxpath_eax(EAX, &fls->FILE_LIST_NFN);
	}
#endif
}

