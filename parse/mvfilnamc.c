
// MVFILNAM - Copyright (c) SLR Systems 1994

#include "all.h"

void *_move_file_list_gindex_path_prim_ext(void *EAX, FILE_LIST_STRUCT *ECX)
{
	// ECX IS GINDEX, EAX IS DESTINATION

	char *EDI = EAX;
	FILE_LIST_STRUCT* EDX = ECX;
	FILE_LIST_STRUCT *fls = EDX->FILE_LIST_PATH_GINDEX;
	if (fls)
	{
	    unsigned len = fls->FILE_LIST_NFN.NFN_PATHLEN;
	    memcpy(EDI, fls->FILE_LIST_NFN.NFN_TEXT, len);
	    EDI += len;
	}

	// OUTPUT FILENAME IN ASCII
	// FROM [SI] TO [DI]

	unsigned len = EDX->FILE_LIST_NFN.NFN_TOTAL_LENGTH;
	memcpy(EDI, EDX->FILE_LIST_NFN.NFN_TEXT, len);
	return EDI + len;
}

void *_move_path_prim_ext(void* EAX, NFN_STRUCT* ECX)
{
	// OUTPUT FILENAME IN ASCII

	// FROM [ECX] TO [EAX]
	char *p = EAX;
	memcpy(p, ECX->NFN_TEXT, ECX->NFN_TOTAL_LENGTH);
	p += ECX->NFN_TOTAL_LENGTH;
	return p;
}
