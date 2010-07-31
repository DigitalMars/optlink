
#include "all.h"

extern unsigned char FNTBL[256];

NFN_STRUCT* _do_defaults(NFN_STRUCT* ESI, CMDLINE_STRUCT* EDI);

NFN_STRUCT* _do_filename(NFN_STRUCT* ECX, CMDLINE_STRUCT* EAX)
{
	// EAX IS FILESTUFF_PTR

	_parse_filename(ECX);

	// DEAL WITH DEFAULTS
	return _do_defaults(ECX, EAX);
}

NFN_STRUCT* _do_defaults(NFN_STRUCT* ESI, CMDLINE_STRUCT* EDI)
{
    if (!(ESI->NFN_FLAGS & NFN_PRIM_SPECIFIED))
    {
	// NO PRIMARY FILENAME, MOVE EITHER NUL OR SRC...

	if ((*EDI->CMD_SELECTED)() ||	// WANT IT, MOVE SOURCE NAME
	// OK, IF PATH OR EXTENTION SPECIFIED, MOVE SOURCE TOO
	    ESI->NFN_FLAGS & (NFN_PATH_SPECIFIED | NFN_EXT_SPECIFIED))
	{
	    _move_srcprim_to_eax(ESI);
	}
    }

    if (!(ESI->NFN_FLAGS & NFN_EXT_SPECIFIED))
    {
	// MOVE DEFAULT EXTENT (UNLESS PRIMARY IS NUL)
	if (!_check_nul1(ESI))
	{
	    // MOVE DEFAULT EXTENT
	    unsigned xlen = EDI->CMD_EXTENT[0];
	    unsigned ECX = xlen;
	    xlen -= ESI->NFN_EXTLEN;
	    ESI->NFN_EXTLEN = ECX;
	    ESI->NFN_TOTAL_LENGTH += xlen;
	    unsigned EAX = ESI->NFN_PATHLEN + ESI->NFN_PRIMLEN;
	    memcpy(ESI->NFN_TEXT + EAX, EDI->CMD_EXTENT + 1, ECX);
	    *(int*)(ESI->NFN_TEXT + EAX + ECX) = 0;
	}
    }
    return ESI;
}

