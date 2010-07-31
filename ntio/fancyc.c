
#include "all.h"

/******
 * returns NULL on error
 */

MYI_STRUCT *_fancy_open_input(NFN_STRUCT *ESI, FILE_LISTS *ECX)
{
    // ESI is NFN_STRUCT containing raw filename
    // ECX is list of paths

    if (!_open_input(ESI, &OBJ_DEVICE))
	return OBJ_DEVICE;

    // Well, it would have been nice

    if (ESI->NFN_FLAGS & NFN_PATH_SPECIFIED)
	goto FANCY_FAIL;

    FILE_LIST_STRUCT *EDI = ECX->FILE_FIRST_GINDEX;

    goto TEST_PATH;

PATH_LOOP:
    if (EDI->FILE_LIST_NFN.NFN_FLAGS & NFN_PATH_SPECIFIED)	// skip 'NUL'
    {
	// Need to move just path
	if (!_open_input(_move_ecxpath_eax(ESI, &EDI->FILE_LIST_NFN), &OBJ_DEVICE))
	{
	    FILE_LIST_STRUCT *EDX = CURN_FILE_LIST_GINDEX;
	    if (EDX)
		EDX->FILE_LIST_PATH_GINDEX = EDI;
	    return OBJ_DEVICE;
	}
    }

    // Try next path
TEST_PATH:
    EDI = EDI->FILE_LIST_NEXT_GINDEX;
    if (EDI)
	goto PATH_LOOP;
FANCY_FAIL:
    _delete_phantom_path(ESI);
    return 0;
}

void _delete_phantom_path(NFN_STRUCT *ESI)
{
    static NFN_STRUCT NUL_PATH;

    if (!(ESI->NFN_FLAGS & NFN_PATH_SPECIFIED))
    {
	_move_ecxpath_eax(ESI, &NUL_PATH);
    }
}
