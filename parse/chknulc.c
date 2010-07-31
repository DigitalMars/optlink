
#include "all.h"

void _check_nul(NFN_STRUCT* EAX)
{
	// SEE IF [EAX] IS NUL DEVICE..., SET FLAG IF SO...

	if (EAX->NFN_PRIMLEN == 0 || _check_nul1(EAX))
	    EAX->NFN_FLAGS |= NFN_NUL;
}

// Return true if [EAX] primary name is 'NUL'
int _check_nul1(NFN_STRUCT* EAX)
{
	// SEE  IF [EAX] PRIMARY NAME IS 'NUL'

	// RETURNS EAX INTACT
	if (EAX->NFN_PRIMLEN != 3)
		return 0;

	unsigned char *p = &EAX->NFN_TEXT[EAX->NFN_PATHLEN];
	return	(p[0] == 'N' || p[0] == 'n') &&
		(p[1] == 'U' || p[1] == 'u') &&
		(p[2] == 'L' || p[2] == 'l');
}

