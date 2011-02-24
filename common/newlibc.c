
#include <assert.h>

#include <windows.h>

#include "all.h"

unsigned LIB_SYMBOLS;
void **SCAN_EXETABLE_PTR;
unsigned CMP_NEXT_BLOCK;
unsigned CMP_ESI_LIMIT;

int SCAN_UNDECO();
int _scan_undeco();
void _handle_undeco(SYMBOL_STRUCT *ECX);
int SCAN_ALIASED_SYMS();
int SCAN_WEAK_EXTRNS();
int SCAN_LAZY_EXTRNS();
void DEFINE_LIB_SYMBOLS();
void _define_lib_symbols();
void _fix_link(void *EAX, SYMBOL_STRUCT *ECX);
int _fix_comdat(void *EDX, SYMBOL_STRUCT *EDI);
void _fix_undefined(void *EDX, SYMBOL_STRUCT *EDI, SYMBOL_STRUCT *ESI);
void _fix_unref_imp(void *EDX, SYMBOL_STRUCT *EDI);
void _fix_unref_lib(void *EDX, SYMBOL_STRUCT *EDI);
void _fix_unref_weak(void *EDX, SYMBOL_STRUCT *EDI, SYMBOL_STRUCT *ESI);
void _fix_unref_lazy(void *EDX, SYMBOL_STRUCT *EDI, SYMBOL_STRUCT *ESI);
void _fix_unref_communal(void *EDX, SYMBOL_STRUCT *EDI);
void _fix_unref_alias(void *EDX, SYMBOL_STRUCT *EDI, SYMBOL_STRUCT *ESI);
int _scan_aliased_syms1();
int _scan_lazy_extrns();
int _scan_weak_extrns();
int _swe_1();
void _do_module_ptrs();
void _add_extrns_to_list();
void _scan_bits();
void _update_lib_symbols(SYMBOL_STRUCT *EBX);
void _count_mods_per_block();

void _process_libraries()
{
	// Get first library file name, and do path scans to find it

	LIB_TYPE = 0xFF;
	FILE_LIST_STRUCT *EAX = LIB_LIST.FILE_FIRST_GINDEX;	// Next library to search for
	FILE_LIST_STRUCT *tos;
	goto NEXT_LIB;

LIB_LOOP:
	if (_find_library_file(EAX))
	    goto NEXT_LIB;

	_define_lib_symbols();

	if (SELECTED_LIB_COUNT > 1)
	    _process_lib();
NEXT_LIB:
	tos = EAX;
	EAX = EAX->FILE_LIST_NEXT_GINDEX;
	if (EAX)
	    goto LIB_LOOP;
	EAX = tos;
	if (!SELECTED_LIB_COUNT)
	    goto L9;
	_process_libs();			// process remaining libs once
	goto NEXT_LIB;

L9:
	// Scan undecorated exports to see if anything needs to be brought in
	if (_scan_undeco())
	    goto NEXT_LIB;

	// Scan weak extrns, see if any defaults need to be declared
	//if (SCAN_ALIASED_SYMS())
	if (_scan_aliased_syms1())
	    goto NEXT_LIB;
	if (_scan_weak_extrns())
	    goto NEXT_LIB;
	if (_scan_lazy_extrns())
	    goto NEXT_LIB;
	LIB_TYPE = 0;
}

int _scan_undeco()
{
	CURN_TYPE = 0;		// no new references

	CURNMOD_GINDEX = FIRST_MODULE_GINDEX;

	SYMBOL_STRUCT *ECX = FIRST_UNDECORATED_GINDEX;
	goto L1;

L0:
	ECX = ECX->_S_NEXT_SYM_GINDEX;
L1:
	if (!ECX)
	    goto L9;

	SYMBOL_STRUCT *EAX = ECX;
	unsigned char DL = ECX->_S_REF_FLAGS;
	if (DL & (UNDECO_EXACT | UNDECO_REFERENCED_1))
	    goto L5;

	EAX = ECX->_S_NEXT_SYM_GINDEX;
	SYMBOL_STRUCT *ESI = (SYMBOL_STRUCT *)ECX->_S_LAST_NCPP_MATCH;

	unsigned n = ECX->_S_N_NCPP_MATCHES;

	if (ESI)
	    goto L2;

	ESI = (SYMBOL_STRUCT *)ECX->_S_LAST_CPP_MATCH;
	n = ECX->_S_N_CPP_MATCHES;

	if (!ESI)
	    goto L3;
L2:
	ECX->_S_REF_FLAGS |= UNDECO_REFERENCED_1;
	_handle_undeco(n <= 1 ? ESI : *(SYMBOL_STRUCT **)ESI);

L3:
	ECX = EAX;
	goto L1;

L5:
	if (DL & UNDECO_REFERENCED_1)
	    goto L0;

	SYMBOL_STRUCT *EBX = ECX->_S_NEXT_SYM_GINDEX;
	_remove_from_undecorated_list(0, ECX);

	ECX = (SYMBOL_STRUCT *)ECX->_S_LAST_NCPP_MATCH;
	_handle_undeco(ECX);

	ECX = EBX;
	goto L1;

L9:
	CURNMOD_GINDEX = 0;
	return CURN_TYPE;
}

void _handle_undeco(SYMBOL_STRUCT *ECX)
{
	// ECX IS SYMBOL GINDEX TO 'REFERENCE'
	SYMBOL_STRUCT *EAX = ECX;
	if (ECX->_S_REF_FLAGS & S_REFERENCED)
	    goto L9;

	ECX->_S_REF_FLAGS |= S_REFERENCED;

	CURN_TYPE = -1;

	switch (ECX->_S_NSYM_TYPE & NSYM_ANDER)
	{
	    case NSYM_UNDEFINED:     _add_to_external_list(EAX, ECX); return;	// FORCE EXTERNAL - MOVE TO DEFINED LIST
	    case NSYM_ASEG:          goto UNDE_DEFINED;	// ASEG - DEFAULT DEFINED
	    case NSYM_RELOC:         goto UNDE_DEFINED;	// RELOCATABLE - DEFAULT DEFINED
	    case NSYM_COMM_NEAR:     goto UNDE_DEFINED;	// NEAR COMMUNAL - DEFAULT DEFINED
	    case NSYM_COMM_FAR:      goto UNDE_DEFINED;	// FAR COMMUNAL - DEFAULT DEFINED
	    case NSYM_COMM_HUGE:     goto UNDE_DEFINED;	// HUGE COMMUNAL - DEFAULT DEFINED
	    case NSYM_CONST:         goto UNDE_DEFINED;	// CONST - DEFAULT DEFINED
	    case NSYM_LIBRARY:       _reference_libsym(EAX, ECX); return;	// LIBRARY - FORCE REFERENCE
	    case NSYM_IMPORT:        goto UNDE_DEFINED;	// IMPORT - DEFAULT DEFINED
	    case NSYM_PROMISED:      goto UNDE_DEFINED;	// PROMISED - DEFAULT DEFINED
	    case NSYM_EXTERNAL:      goto UNDE_DEFINED;	// EXTERNAL - WILL COMPLAIN LATER...
	    case NSYM_WEAK_EXTRN:    goto UNDE_DEFINED;	// REFERENCED WEAK_EXTRN - DEFAULT DEFINED...
	    case NSYM_POS_WEAK:      goto UNDE_DEFINED;	// POSWEAK - CANNOT HAPPEN
	    case NSYM_LIBRARY_LIST:  goto UNDE_DEFINED;	// LIBLIST - CANNOT HAPPEN
	    case NSYM__IMP__UNREF:   _reference_impsym(EAX, ECX); return;	// __imp__UNREF, FORCE REFERENCE
	    case NSYM_ALIASED:       goto UNDE_DEFINED;	// REFERENCED ALIAS - DEFAULT DEFINED
	    case NSYM_COMDAT:        goto UNDE_REFERENCE_COMDAT;	// COMDAT - REFERENCE IT...
	    case NSYM_WEAK_DEFINED:  goto UNDE_DEFINED;	// WEAK-DEFINED - DEFAULT DEFINED
	    case NSYM_WEAK_UNREF:    _add_to_weak_list(EAX, ECX); return;	// UNREFERENCED WEAK, REFERENCE IT
	    case NSYM_ALIASED_UNREF: _add_to_aliased_list(EAX, ECX); return;	// UNREFERENCED ALIAS, REFERENCE IT
	    case NSYM_POS_LAZY:      goto UNDE_DEFINED;	// POS_LAZY, CANNOT HAPPEN
	    case NSYM_LAZY:          goto UNDE_DEFINED;	// REFERENCED LAZY, DEFAULT DEFINED
	    case NSYM_LAZY_UNREF:    _add_to_lazy_list(EAX, ECX); return;	// UNREFERENCED LAZY, REFERENCE IT
	    case NSYM_ALIAS_DEFINED: goto UNDE_DEFINED;	// ALIAS-DEFINED, DEFAULT DEFINED
	    case NSYM_LAZY_DEFINED:  goto UNDE_DEFINED;	// LAZY-DEFINED, DEFAULT DEFINED
	    case NSYM_NCOMM_UNREF:   goto UNDE_NCOMM_UNREF;	// UNREFERENCED COMMUNAL, REFERENCE IT
	    case NSYM_FCOMM_UNREF:   goto UNDE_FCOMM_UNREF;	// UNREFERENCED COMMUNAL, REFERENCE IT
	    case NSYM_HCOMM_UNREF:   goto UNDE_HCOMM_UNREF;	// UNREFERENCED COMMUNAL, REFERENCE IT
	    case NSYM__IMP__:        goto UNDE_DEFINED;	// __imp__
	    case NSYM_UNDECORATED:   goto UNDE_DEFINED;	// UNDECORATED, CAN'T HAPPEN
//	    default:
//		assert(0);
	}


UNDE_REFERENCE_COMDAT:
	ECX->_S_REF_FLAGS &= ~S_REFERENCED;
	_reference_comdat(EAX,ECX);
	return;

L9:
UNDE_DEFINED:
	return;

UNDE_NCOMM_UNREF:
UNDE_FCOMM_UNREF:
UNDE_HCOMM_UNREF:
	ECX->_S_NSYM_TYPE -= NSYM_NCOMM_UNREF - NSYM_COMM_NEAR;
	_add_to_communal_list(EAX, ECX);
}

int _scan_lazy_extrns()
{
	CURN_TYPE = NSYM_LAZY;
	return _swe_1();
}

int _scan_weak_extrns()
{
	// SCAN REFERENCED WEAK EXTRNS, TILL WE FIND A DEFAULT IN A LIBRARY
	// CARRY CLEAR IF WE CREATED AN EXTERNAL
	CURN_TYPE = NSYM_WEAK_EXTRN;
	return _swe_1();
}

int _swe_1()
{
    SYMBOL_STRUCT *ECX;
    SYMBOL_STRUCT *EDX;
    SYMBOL_STRUCT *ESI;
    SYMBOL_STRUCT *EDI;
    goto SWE_1;

SWE_DEFINED:
	EDI->_S_REF_FLAGS |= S_REFERENCED;
SWE_DEFINED_1:
	// DS:SI (CX) IS SYMBOL TO MOVE TO WEAK_DEFINED LIST
	_fix_link(ECX, ESI);
SWE_1:
	ESI = FIRST_WEAK_GINDEX;
	if (CURN_TYPE == NSYM_WEAK_EXTRN)
	    goto SWE_2;
	ESI = FIRST_LAZY_GINDEX;
SWE_2:
SWE_3:
	if (!ESI)
	    goto L9;

	ECX = ESI;
	EDI = (SYMBOL_STRUCT *)ESI->_S_WEAK_DEFAULT_GINDEX;	// MY ASSIGNED DEFAULT
	EDX = EDI;
	switch (EDI->_S_NSYM_TYPE)
	{
	    case NSYM_UNDEFINED:     goto SWE_UNDEFINED;	// FORCE EXTERNAL - MOVE TO DEFINED LIST
	    case NSYM_ASEG:          goto SWE_DEFINED;	// ASEG - DEFAULT DEFINED
	    case NSYM_RELOC:         goto SWE_DEFINED;	// RELOCATABLE - DEFAULT DEFINED
	    case NSYM_COMM_NEAR:     goto SWE_DEFINED;	// NEAR COMMUNAL - DEFAULT DEFINED
	    case NSYM_COMM_FAR:      goto SWE_DEFINED;	// FAR COMMUNAL - DEFAULT DEFINED
	    case NSYM_COMM_HUGE:     goto SWE_DEFINED;	// HUGE COMMUNAL - DEFAULT DEFINED
	    case NSYM_CONST:         goto SWE_DEFINED;	// CONST - DEFAULT DEFINED
	    case NSYM_LIBRARY:       goto SWE_REF_LIB;	// LIBRARY - FORCE REFERENCE
	    case NSYM_IMPORT:        goto SWE_DEFINED;	// IMPORT - DEFAULT DEFINED
	    case NSYM_PROMISED:      goto SWE_DEFINED;	// PROMISED - DEFAULT DEFINED
	    case NSYM_EXTERNAL:      goto SWE_DEFINED;	// EXTERNAL - WILL COMPLAIN LATER...
	    case NSYM_WEAK_EXTRN:    goto SWE_DEFINED;	// REFERENCED WEAK_EXTRN - DEFAULT DEFINED...
	    case NSYM_POS_WEAK:      goto SWE_DEFINED;	// POSWEAK - CANNOT HAPPEN
	    case NSYM_LIBRARY_LIST:  goto SWE_DEFINED;	// LIBLIST - CANNOT HAPPEN
	    case NSYM__IMP__UNREF:   goto SWE__IMP__UNREF;	// __imp__UNREF, FORCE REFERENCE
	    case NSYM_ALIASED:       goto SWE_DEFINED;	// REFERENCED ALIAS - DEFAULT DEFINED
	    case NSYM_COMDAT:        goto SWE_COMDAT;	// COMDAT - REFERENCE IT...
	    case NSYM_WEAK_DEFINED:  goto SWE_DEFINED;	// WEAK-DEFINED - DEFAULT DEFINED
	    case NSYM_WEAK_UNREF:    goto SWE_WEAK_UNREF;	// UNREFERENCED WEAK, REFERENCE IT
	    case NSYM_ALIASED_UNREF: goto SWE_ALIAS_UNREF;	// UNREFERENCED ALIAS, REFERENCE IT
	    case NSYM_POS_LAZY:      goto SWE_DEFINED;	// POS_LAZY, CANNOT HAPPEN
	    case NSYM_LAZY:          goto SWE_DEFINED;	// REFERENCED LAZY, DEFAULT DEFINED
	    case NSYM_LAZY_UNREF:    goto SWE_LAZY_UNREF;	// UNREFERENCED LAZY, REFERENCE IT
	    case NSYM_ALIAS_DEFINED: goto SWE_DEFINED;	// ALIAS-DEFINED, DEFAULT DEFINED
	    case NSYM_LAZY_DEFINED:  goto SWE_DEFINED;	// LAZY-DEFINED, DEFAULT DEFINED
	    case NSYM_NCOMM_UNREF:   goto SWE_NCOMM_UNREF;	// UNREFERENCED COMMUNAL, REFERENCE IT
	    case NSYM_FCOMM_UNREF:   goto SWE_FCOMM_UNREF;	// UNREFERENCED COMMUNAL, REFERENCE IT
	    case NSYM_HCOMM_UNREF:   goto SWE_HCOMM_UNREF;	// UNREFERENCED COMMUNAL, REFERENCE IT
	    case NSYM__IMP__:        goto SWE_DEFINED;	// __imp__
	    case NSYM_UNDECORATED:   goto SWE_DEFINED;	// UNDECORATED, CAN'T HAPPEN
//	    default:
//		assert(0);
	}

SWE__IMP__UNREF:
	_fix_unref_imp(EDX, EDI);
	goto SWE_FINI_1;

SWE_REF_LIB:
	_fix_unref_lib(EDX, EDI);		// REFERENCE IT
SWE_FINI_1:
	_fix_link(ECX, ECX);
	CURNMOD_GINDEX = 0;
	return 1;

L9:
	CURNMOD_GINDEX = 0;
	return 0;

SWE_COMDAT:
	if (!_fix_comdat(EDX, EDI))
	    goto SWE_FINI_1;		// TERMINATE SCAN IF WE REFERENCED AN UNREFERENCED COMDAT...
	goto SWE_U_1;

SWE_UNDEFINED:
	_fix_undefined(EDX, EDI, ESI);
SWE_U_1:
	ESI = ECX;
	goto SWE_DEFINED_1;

SWE_WEAK_UNREF:
	_fix_unref_weak(EDX, EDI, ESI);		// REFERENCED IT
	if (CURN_TYPE == NSYM_WEAK_EXTRN)
	    goto SWE_U_1;			// CONTINUE SCANNING
	goto SWE_FINI_1;

SWE_LAZY_UNREF:
	_fix_unref_lazy(EDX, EDI, ESI);		// REFERENCE IT
	goto SWE_U_1;				// CONTINUE SCANNING

SWE_NCOMM_UNREF:
SWE_FCOMM_UNREF:
SWE_HCOMM_UNREF:
	_fix_unref_communal(EDX, EDI);	// REFERENCE IT
	goto SWE_U_1;			// CONTINUE SCANNING

SWE_ALIAS_UNREF:
	_fix_unref_alias(EDX, EDI, ESI);	// REFERENCE IT
	goto SWE_FINI_1;		// STOP SCAN, ALIASES HIGHER PRIORITY
}

void _fix_link(void *EAX, SYMBOL_STRUCT *ECX)
{
	// ECX IS PHYSICAL, EAX IS INDEX

	if (ECX->_S_NSYM_TYPE != CURN_TYPE)
	    return;

	if (CURN_TYPE != NSYM_WEAK_EXTRN)
	    goto L5;

	_remove_from_weak_list(EAX, ECX);
	_add_to_weak_defined_list(EAX, ECX);
	return;

L5:
	_remove_from_lazy_list(EAX, ECX);
	_add_to_lazy_defined_list(EAX, ECX);
}

int _fix_comdat(void *EDX, SYMBOL_STRUCT *EDI)
{
	if (EDI->_S_REF_FLAGS & S_REFERENCED)
	    return 1;
	_reference_comdat(EDX, EDI);
	return 0;
}


void _fix_unref_alias(void *EDX, SYMBOL_STRUCT *EDI, SYMBOL_STRUCT *ESI)
{
	// DX:DI IS UNDEFINED DEFAULT TO BE HARD REFERENCED
	// ECX is WEAK EXTRN
	CURNMOD_GINDEX = ESI->_S_REF_MOD_GINDEX;
	EDI->_S_REF_FLAGS |= S_REFERENCED;
	_add_to_aliased_list(EDX, EDI);
}


void _fix_unref_lib(void *EDX, SYMBOL_STRUCT *EDI)
{
	// ES:DI (DX) IS LIBRARY SYMBOL TO BE 'REFERENCED'
	EDI->_S_REF_FLAGS |= S_REFERENCED;
	_reference_libsym(EDX, EDI);	// AX IS INDEX, DS:BX IS PHYSICAL
}


void _fix_unref_imp(void *EDX, SYMBOL_STRUCT *EDI)
{
	// ES:DI (DX) IS LIBRARY SYMBOL TO BE 'REFERENCED'
	EDI->_S_REF_FLAGS |= S_REFERENCED;
	_reference_impsym(EDX, EDI);	// AX IS INDEX, DS:BX IS PHYSICAL
}


void _fix_unref_communal(void *EDX, SYMBOL_STRUCT *EDI)
{
	EDI->_S_NSYM_TYPE -= NSYM_NCOMM_UNREF - NSYM_COMM_NEAR;
	EDI->_S_REF_FLAGS |= S_REFERENCED;
	_add_to_communal_list(EDX, EDI);
}


void _fix_undefined(void *EDX, SYMBOL_STRUCT *EDI, SYMBOL_STRUCT *ESI)
{
	// DX:DI IS UNDEFINED DEFAULT TO BE HARD REFERENCED
	CURNMOD_GINDEX = ESI->_S_REF_MOD_GINDEX;
	EDI->_S_REF_FLAGS |= S_REFERENCED;
	_add_to_external_list(EDX, EDI);
}


void _fix_unref_weak(void *EDX, SYMBOL_STRUCT *EDI, SYMBOL_STRUCT *ESI)
{
	// DX:DI IS UNDEFINED DEFAULT TO BE HARD REFERENCED
	CURNMOD_GINDEX = ESI->_S_REF_MOD_GINDEX;
	EDI->_S_REF_FLAGS |= S_REFERENCED;
	_add_to_weak_list(EDX, EDI);
}


void _fix_unref_lazy(void *EDX, SYMBOL_STRUCT *EDI, SYMBOL_STRUCT *ESI)
{
	// DX:DI IS UNDEFINED DEFAULT TO BE HARD REFERENCED
	CURNMOD_GINDEX = ESI->_S_REF_MOD_GINDEX;
	EDI->_S_REF_FLAGS |= S_REFERENCED;
	_add_to_lazy_list(EDX, EDI);
}

int _scan_aliased_syms1()
{
	// Scan referenced aliased symbols, make sure substitute is referenced

	SYMBOL_STRUCT *ECX;
	SYMBOL_STRUCT *EDX;
	SYMBOL_STRUCT *ESI;
	SYMBOL_STRUCT *EDI;
	goto SCAN_ALIASED_SYMS;

SAS_NCOMM_UNREF:
SAS_FCOMM_UNREF:
SAS_HCOMM_UNREF:
	_fix_unref_communal(EDX, EDI);		// REFERENCE COMMUNALS
	goto SAS_U_1;			// KEEP SCANNING

SAS_WEAK_UNREF:
	_fix_unref_weak(EDX, EDI, ESI);		// REFERENCE WEAK SYMBOL
	goto SAS_U_1;			// KEEP SCANNING

SAS_ALIAS_UNREF:
	_fix_unref_alias(EDX, EDI, ESI);		// REFERENCE ALIAS SYMBOL
	goto SAS_U_1;			// KEEP SCANNING

SAS_LAZY_UNREF:
	_fix_unref_lazy(EDX, EDI, ESI);
	goto SAS_U_1;

SAS_UNDEFINED:
	_fix_undefined(EDX, EDI, ESI);
SAS_U_1:
	ESI = ECX;
	goto SAS_DEFINED_1;

SAS_DEFINED:
	EDI->_S_REF_FLAGS |= S_REFERENCED;
SAS_DEFINED_1:
	// DS:SI (CX) IS SYMBOL TO MOVE TO ALIAS_DEFINED LIST
	_remove_from_aliased_list(ECX, ESI);
	_add_to_alias_defined_list(ECX, ESI);
SCAN_ALIASED_SYMS:
	ESI = FIRST_ALIASED_GINDEX;
	if (!ESI)
	    goto L9;

	ECX = ESI;
	EDI = (SYMBOL_STRUCT *)ESI->_S_ALIAS_SUBSTITUTE_GINDEX;	// MY ASSIGNED DEFAULT
	EDX = EDI;

	switch (EDI->_S_NSYM_TYPE)
	{
	    case NSYM_UNDEFINED:     goto SAS_UNDEFINED;	// FORCE EXTERNAL
	    case NSYM_ASEG:          goto SAS_DEFINED;	// ASEG - DEFAULT DEFINED
	    case NSYM_RELOC:         goto SAS_DEFINED;	// RELOCATABLE - DEFAULT DEFINED
	    case NSYM_COMM_NEAR:     goto SAS_DEFINED;	// NEAR COMMUNAL - DEFAULT DEFINED
	    case NSYM_COMM_FAR:      goto SAS_DEFINED;	// FAR COMMUNAL
	    case NSYM_COMM_HUGE:     goto SAS_DEFINED;	// HUGE COMMUNAL - DEFAULT DEFINED
	    case NSYM_CONST:         goto SAS_DEFINED;	// CONST - DEFAULT DEFINED
	    case NSYM_LIBRARY:       goto SAS_LIB;	// LIBRARY - FORCE EXTERNAL
	    case NSYM_IMPORT:        goto SAS_DEFINED;	// IMPORT - DEFAULT DEFINED
	    case NSYM_PROMISED:      goto SAS_DEFINED;	// PROMISED - DEFAULT DEFINED
	    case NSYM_EXTERNAL:      goto SAS_DEFINED;	// EXTERNAL -
	    case NSYM_WEAK_EXTRN:    goto SAS_DEFINED;	// WEAK_EXTRN - FORCE EXTERNAL
	    case NSYM_POS_WEAK:      goto SAS_DEFINED;	// POSWEAK - CANNOT HAPPEN
	    case NSYM_LIBRARY_LIST:  goto SAS_DEFINED;	// LIBLIST - CANNOT HAPPEN
	    case NSYM__IMP__UNREF:   goto SAS_IMP;	// __imp_UNREF, REFERENCE IT
	    case NSYM_ALIASED:       goto SAS_DEFINED;	// ALIAS
	    case NSYM_COMDAT:        goto SAS_COMDAT;	// COMDAT
	    case NSYM_WEAK_DEFINED:  goto SAS_DEFINED;	// WEAK_DEFINED
	    case NSYM_WEAK_UNREF:    goto SAS_WEAK_UNREF;	// UNREFERENCED WEAK, REFERENCE IT
	    case NSYM_ALIASED_UNREF: goto SAS_ALIAS_UNREF;	// UNREFERENCED ALIAS, REFERENCE IT
	    case NSYM_POS_LAZY:      goto SAS_DEFINED;	// POS_LAZY, CANNOT HAPPEN
	    case NSYM_LAZY:          goto SAS_DEFINED;	// REFERENCED LAZY, DEFAULT DEFINED
	    case NSYM_LAZY_UNREF:    goto SAS_LAZY_UNREF;	// UNREFERENCED LAZY, REFERENCE IT
	    case NSYM_ALIAS_DEFINED: goto SAS_DEFINED;	// ALIAS-DEFINED, DEFAULT DEFINED
	    case NSYM_LAZY_DEFINED:  goto SAS_DEFINED;	// LAZY-DEFINED, DEFAULT DEFINED
	    case NSYM_NCOMM_UNREF:   goto SAS_NCOMM_UNREF;	// UNREFERENCED COMMUNAL, REFERENCE IT
	    case NSYM_FCOMM_UNREF:   goto SAS_FCOMM_UNREF;	// UNREFERENCED COMMUNAL, REFERENCE IT
	    case NSYM_HCOMM_UNREF:   goto SAS_HCOMM_UNREF;	// UNREFERENCED COMMUNAL, REFERENCE IT
	    case NSYM__IMP__:        goto SAS_DEFINED;	// __imp__
	    case NSYM_UNDECORATED:   goto SAS_DEFINED;	// UNDECORATED, CANNOT HAPPEN
//	    default:
//		assert(0);
	}

L9:
	CURNMOD_GINDEX = 0;
	return 0;

SAS_IMP:
	// ES:DI (DX) IS LIBRARY SYMBOL TO BE 'REFERENCED'
	EDI->_S_REF_FLAGS |= S_REFERENCED;
	_reference_impsym(EDX, EDI);
	goto SAS_FINI_1;

SAS_LIB:
	// ES:DI (DX) IS LIBRARY SYMBOL TO BE 'REFERENCED'
	EDI->_S_REF_FLAGS |= S_REFERENCED;
	_reference_libsym(EDX, EDI);
SAS_FINI_1:
	_remove_from_aliased_list(ECX, ESI);
	_add_to_alias_defined_list(ECX, ESI);
	CURNMOD_GINDEX = 0;
	return 1;

SAS_COMDAT:
	_reference_comdat(EDX, EDI);
	goto SAS_FINI_1;
}

// return 1 means file not found

int _find_library_file(FILE_LIST_STRUCT *EAX)
{
	/* Current directory first
	 *  paths supplied by command line
	 *  paths supplied by LIB= environment variable
	 * 
	 * (LIB= already scanned at lnkinit time, paths appended to
	 * 	those supplied by command line)
	 */

	CURN_FILE_LIST_GINDEX = EAX;

	EAX->FILE_LIST_FLAGS |= FLF_RANDOM;

#if fgh_inthreads
	if (!(_HOST_THREADED & 0xFF))
	    goto NO_THREADS;

	MYI_STRUCT *EDX = &MYI_STUFF[EAX->FILE_LIST_THREAD];
	MYI2_STRUCT *m2s = EDX->MYI_LOCALS;

	// Get MYNAMS structure
	OBJ_DEVICE = EDX;
	unsigned i = m2s->MYI2_NEXT_OPEN_FILE;
	i = (i + 1) & 3;

	m2s->MYI2_NEXT_OPEN_FILE = i;
	NFN_STRUCT *ESI = &m2s->MYI2_NAMS[i];

	OPEN_STRUCT *EDI = &m2s->MYI2_OPEN_STRUC[i];

	EDX->MYI_FILE_LIST_GINDEX = CURN_FILE_LIST_GINDEX;
	m2s->MYI2_LAST_OPENFILE = EDI;

	_capture_eax(&m2s->OPENFILE_OPEN_SEM);		// wait till file is open

	EDX->MYI_HANDLE = EDI->OPENFILE_HANDLE;

	_move_nfn(&FILNAM, ESI);

	// Save filename for LIBREAD_THREADS
	if (!EDI->OPENFILE_FLAGS)
	    goto OPEN_FAIL;			// didn't find it

	FILE_LIST_STRUCT *fls = CURN_FILE_LIST_GINDEX;
	fls->FILE_LIST_PATH_GINDEX = EDI->OPENFILE_PATH_GINDEX;

	EDX->MYI_BYTE_OFFSET = -PAGE_SIZE;
	EDX->MYI_PHYS_ADDR = 0;
	EDX->MYI_COUNT = 0;	// buffer empty
	EDX->MYI_BUFCNT = 0;
	EDX->MYI_FILLBUF = &_readlib_16k;

	return 0;

OPEN_FAIL:
	// Let him open another
	_release_eax_bump(&m2s->OPENFILE_AVAIL_SEM);

	EDX->MYI_FILLBUF = &_readlib_16k;

	_delete_phantom_path(&FILNAM);
	goto FNF;

#endif

NO_THREADS:
	_move_nfn(&FILNAM, &EAX->FILE_LIST_NFN);	// copy to FILNAM

	MYI_STRUCT *ms =_fancy_open_input(&FILNAM, &LIBPATH_LIST);
	if (!ms)
	    goto FNF;

	ms->MYI_FILLBUF = &_readlib_16k;
	FILE_LIST_STRUCT *fls2 = CURN_FILE_LIST_GINDEX;	// now update flags, time, date, file-size

	NFN_STRUCT *ns = &fls2->FILE_LIST_NFN;
	ns->NFN_FLAGS = FILNAM.NFN_FLAGS;
	ns->NFN_TYPE = FILNAM.NFN_TYPE;
	ns->reserved1 = FILNAM.reserved1;
	ns->NFN_TIME = FILNAM.NFN_TIME;
	ns->NFN_DATE = FILNAM.NFN_DATE;
	ns->NFN_FILE_LENGTH = FILNAM.NFN_FILE_LENGTH;
	return 0;

FNF:
	if (LIB_NOT_FOUND_FATAL & 0xFF)
	    _err_nfn_abort(FILE_NOT_FOUND_ERR, &FILNAM);
	else
	    _warn_nfn_ret(FILE_NOT_FOUND_ERR, &FILNAM);
	return 1;
}

void _lib_fatal()
{
    _err_abort(LIB_ERR);	// not a valid library
}


//void *_define_lib_symbolsX(LIBRARY_STRUCT *xEBX, unsigned xEDX)
void _define_lib_symbols()
{
	// This creates library entry, defining sizes, blocks, and symbols
	// First, define header module

	LIB_UNUSED_SYMBOLS_GINDEX = 0;
	CURN_LIB_MODULE = -1;
	unsigned char *p;
	p = GET_FIRST_SECTOR();		// Returns EAX pointing actually to DGROUP 16-bytes

	if (*p != 0xF0)			// must be library header
	    _lib_fatal();		// someday allow .obj files

	unsigned EDX = *(unsigned short *)(p + 1);
	if (EDX >= 512)
	{
	    if (EDX > 4096)
		_lib_fatal();
	    _warn_ret(LIB_PAGESIZE_ERR);
	}
	EDX += 3;
	unsigned offset = *(unsigned *)(p + 3);
	LIB_PAGE_SIZE = EDX;	// that is page size

	EDX = *(unsigned short *)(p + 7);		// # of dict blocks
	FILE_LIST_STRUCT *fls = CURN_FILE_LIST_GINDEX;

	DICT_BLOCKS = EDX;
	if (!EDX)
	    _lib_fatal();

	// Define offset to directory, and directory size
	DICT_OFFSET = offset;

	fls->FILE_LIST_NFN.NFN_FILE_LENGTH = offset;
	if (!offset)
	    _lib_fatal();

#if fgh_inthreads
	if (_HOST_THREADED & 0xFF)
	    goto L1;
#endif
	_dosposition_i(OBJ_DEVICE, offset);
    L1:
	void *EAX = (void *)_text_pool_alloc(sizeof(LIBRARY_STRUCT));

	LIBRARY_STRUCT *EBX = EAX;
	CURNLIB_GINDEX = EAX;

	FILNAM.NFN_FILE_LENGTH = DICT_OFFSET;

	EDX = DICT_OFFSET + PAGE_SIZE - 1;

	memset(EBX, 0, sizeof(LIBRARY_STRUCT));

	EDX >>= PAGE_BITS;		// EDX IS # OF BLOCK LEFT
	MYI_STRUCT *ECX = OBJ_DEVICE;

	EBX->_LS_BLOCKS_LEFT = EDX;

	EBX->_LS_BLOCKS = EDX;		// AND # OF BLOCKS
	EBX->_LS_HANDLE = ECX->MYI_HANDLE;

#if fgh_inthreads
	if (_HOST_THREADED & 0xFF)
	{
	    ECX->MYI_HANDLE = 0;

	    // PLACE TO BUILD LIST OF REQUESTED BLOCKS
	    void *p = (void *)_p1only_pool_get(EDX * 4);

	    EBX->_LS_NEXT_TRLIB_BLOCK = p;
	    EBX->_LS_NEXT_REQUEST_BLOCK = p;
	}
#endif
	unsigned n = EBX->_LS_BLOCKS;		// # OF BLOCKS

	// WE WILL SUPPORT UP TO 64MB FILES... OR UP TO 4K BLOCKS

	n <<= 3;
	if (n < PAGE_SIZE / 2)			// bigger than half a block?
	    goto L38;

	void **pEDI = (void **)&EBX->_LS_BLOCK_TABLE;
L31:
	void *p1 = _get_new_phys_blk();
	*pEDI = p1;
	pEDI++;
	memset(p1, 0, PAGE_SIZE);

	if (n >= PAGE_SIZE)
	{
	    n -= PAGE_SIZE;
	    goto L31;
	}
	goto L39;

L38:
	EBX->_LS_BLOCK_TABLE[0] = _p1only_pool_get(n);
	memset(EBX->_LS_BLOCK_TABLE[0], 0, n);			// ZERO IT OUT, NO BLOCKS YET
L39:
	//return EBX;

#if 1
    {
	EBX->_LS_FILE_POSITION = -1;
	EBX->_LS_FILE_LIST_GINDEX = CURN_FILE_LIST_GINDEX;

	LIBRARY_STRUCT *ECX = CURNLIB_GINDEX;
	LIBRARY_STRUCT *ls2 = LAST_CV_LIB_GINDEX;

	LAST_CV_LIB_GINDEX = ECX;
	unsigned EDX = CV_LIB_COUNT;

	if (!ls2)
	    goto AFL_CV;

	EDX++;
	ls2->_LS_NEXT_CV_GINDEX = ECX;

AFL_CV_RET:
	EBX->_LS_LIBNUM = EDX;
	CV_LIB_COUNT = EDX;

	_init_install_module();	// SET UP FOR BIT # SETTING

	// BUILD LIST OF DESIRED MODULES FROM THIS LIBRARY.
	_add_extrns_to_list();

	MYI_STRUCT *EAX = OBJ_DEVICE;

#if fgh_inthreads
	if (_HOST_THREADED & 0xFF)
	{
	    MYI2_STRUCT *ms2 = EAX->MYI_LOCALS;

	    OPEN_STRUCT *EDX = ms2->MYI2_LAST_OPENFILE;
	    EDX->OPENFILE_HANDLE = 0;	// SO IT DOESN'T GET CLOSED

	    _release_eax(&ms2->OPENFILE_AVAIL_SEM);		// LET HIM OPEN ANOTHER FILE
	}
	else
#endif
	{
	    EAX->MYI_HANDLE = 0;
	}

	// SCAN BITMAP STORING PAGES FROM 0 TO MAX_PAGE
	// USE EXETABLE AS PTR STORAGE FOR TEMPORARY INFO
	_scan_bits();

#if 0
	// Don't understand this, doesn't seem to be a real problem
	if (LIB_SYMBOLS < LIB_MODULES - 1)
	    // See Bugzilla 2378 and 4645
	    _warn_ret(FIX_LIB_ERR);		// Library probably needs FIXLIB
#endif
	goto L6;

AFL_CV:
	EDX++;
	FIRST_CV_LIB_GINDEX = ECX;
	goto AFL_CV_RET;

ADD_FIRST_LIB:
	FIRST_LIB_GINDEX = ECX;
	goto AFL_1;

L6:
	// COUNT MODULES/BLOCK
	_count_mods_per_block();
	//extern void COUNT_MODS_PER_BLOCK();
	//COUNT_MODS_PER_BLOCK();

	// ADD THIS LIBRARY TO LINKED LIST OF AVAILABLE LIBRARIES
	LIBRARY_STRUCT *EAX2 = LAST_LIB_GINDEX;
	LIBRARY_STRUCT *ESI = CURNLIB_GINDEX;

	LIBRARY_STRUCT *EDX2 = EAX2;
	ECX = ESI;

	if (!EAX2)
	    goto ADD_FIRST_LIB;

	EAX2->_LS_NEXT_LIB_GINDEX = ECX;
	ESI->_LS_PREV_LIB_GINDEX = EDX2;
AFL_1:
	LAST_LIB_GINDEX = ECX;

	// DEFINE LS_MODULE_PTRS
	_do_module_ptrs();

	// DEFINE LS_MODULE_PTRS PLEASE FOR BINSER'S USE
	LIBRARY_STRUCT *EAX3 = CURNLIB_GINDEX;

	unsigned ECX2 = LIB_MODULES;
	void **EAX4 = EAX3->_LS_MODULE_PTRS;

	if (ECX2 < PAGE_SIZE/4/2 || ECX2 > PAGE_SIZE/4)
	    goto L8;

	EAX4 = *EAX4;		// DEREFERENCE ONE BLOCK
L8:
	LS_MODULE_PTR = EAX4;

	// SCAN LIST OF EXTERNALS UPDATING INFO ABOUT SYMBOLS FROM
	// THIS LIBRARY
	REFERENCING_LIBS = 0xFF;
	_update_lib_symbols(FIRST_EXTERNAL_GINDEX);		// THESE GET REFERENCED NOW
	REFERENCING_LIBS = 0;
	_update_lib_symbols(FIRST_WEAK_GINDEX);		// THESE JUST GET LIB_MODULE UPDATED
	SYMBOL_STRUCT *EBX = LIB_UNUSED_SYMBOLS_GINDEX;
	goto TEST_SYM;

SYM_LOOP:
	// CONVERT BEGINNING PAGE # TO MODULE #
	EBX->_S_LIB_MODULE = _binser_module(EBX->_S_LIB_MODULE);
	EBX = EBX->_S_PREV_SYM_GINDEX;
TEST_SYM:
	if (EBX)
	    goto SYM_LOOP;

	// RELEASE SPACE ALLOCATED TO MODULE LIST
	CURNLIB_GINDEX = 0;
L9:
	// IN CASE WE USED IT FOR MAXES
	memset(&EXETABLE, 0, 16 * 4);
    }
}

#endif

void _update_lib_symbols(SYMBOL_STRUCT *EBX)
{
	// DSSI IS HEAD OF LIST TO SCAN FOR MY SYMBOLS
	LIBRARY_STRUCT *EDI = CURNLIB_GINDEX;
	goto L6;

L1:
	EBX = EBX->_S_NEXT_SYM_GINDEX;
	if (!EBX)
	    goto L9;

L3:
	SYMBOL_STRUCT *ESI = EBX;
	if (EDI != (LIBRARY_STRUCT *)EBX->_S_LIB_GINDEX)
	    goto L1;

	// CONVERT PAGE # TO MODULE #
	// EBX->_S_LIB_MODULE is currently page
	EBX->_S_LIB_MODULE = _binser_module(EBX->_S_LIB_MODULE);

	// Officially request this symbol
	SYMBOL_STRUCT *ECX = EBX;
	EBX = EBX->_S_NEXT_SYM_GINDEX;
	if (!REFERENCING_LIBS)
	    goto L5;

	_remove_from_external_list(ESI, ECX);
	_reference_libsym(ESI, ECX);
L5:
L6:
	if (EBX)
	    goto L3;
L9:	;
}

void _count_mods_per_block()
{
	// Count modules/block

	LIBRARY_STRUCT *ls = CURNLIB_GINDEX;

	DID_DICT_OFFSET = 0;

	unsigned *EDI = (unsigned *)ls->_LS_BLOCK_TABLE[0];
	void **EAX = &ls->_LS_BLOCK_TABLE[1];

	EDI++;
	unsigned *CMP_NEXT_BLOCK = (unsigned *)EAX;

	unsigned ECX = LIB_MODULES;		// # OF MODULES
	unsigned *EDX = (unsigned *)&EXETABLE[0];

	--*EDI;				// DON'T COUNT END OF FIRST
	--ECX;				// DON'T COUNT DICTIONARY

	unsigned *EBP = (unsigned *)((char *)EDI + PAGE_SIZE);
	unsigned EBX = PAGE_SIZE;
L60:
	unsigned *ESI = (unsigned *)*EDX++;

	unsigned *EDXsave = EDX;			// SAVE PTR FOR NEXT SOURCE BLK

	// WE ARE ACTING ON END-OF-MODULES
	unsigned *CMP_ESI_LIMIT = (unsigned *)((char *)ESI + PAGE_SIZE);
	if (!ECX)
	    goto L69;
L61:
	unsigned EAX3 = *ESI++;			// PAGE #

	EAX3 *= LIB_PAGE_SIZE;		// OFFSET IN LIBRARY
L62:
	if (EBX > EAX3)
	    goto L65;			// INC COUNT, TRY NEXT MOD

	unsigned EDX2 = *EDI;
	if (EBX == EAX3)
	{
	    ++EDX2;
	    EBX += PAGE_SIZE;

	    *EDI = EDX2;
	    EDI += 2;

	    if (EDI == EBP)
	    {
		unsigned *EAX2 = CMP_NEXT_BLOCK;
		EDI = (unsigned *)*EAX2;
		EAX2 += 1;
		EDI += 1;
		CMP_NEXT_BLOCK = EAX2;
	    }
	    goto L66;
	}
	++EDX2;				// INC CNT, INC BLK, SAME MOD
	EBX += PAGE_SIZE;

	*EDI = EDX2;
	EDI += 2;

	if (EDI == EBP)
	{
	    unsigned *EAX2 = CMP_NEXT_BLOCK;
	    EDI = (unsigned *)*EAX2;
	    EAX2 += 1;
	    EDI += 1;
	    CMP_NEXT_BLOCK = EAX2;
	}
	goto L62;

L65:
	++*EDI;					// INC COUNT
L66:
	unsigned *EDX3 = CMP_ESI_LIMIT;
	if (!--ECX)			// TRY NEXT MODULE
	    goto L69;

	if (ESI != EDX3)
	    goto L61;

	EDX = EDXsave;
	goto L60;

	// MAY NEED TO ADJUST FOR LAST ONE...
L69:
	if (DID_DICT_OFFSET & 0xFF)
	    goto L699;
	DID_DICT_OFFSET = 0xFF;
	++ECX;
	EAX3 = DICT_OFFSET;
	goto L62;

L699:
	;
}

void *_get_new_dssi();
void _release_fsymbeg();

void _scan_bits()
{
	// SCAN BIT TABLE, BUILDING LIST OF PAGES FOR BINARY SEARCHING LATER

	SCAN_EXETABLE_PTR = (void **)&EXETABLE;

	unsigned char *EBP = FSYMBEG;		//SCAN 64K BITS (8K BYTES)
	unsigned ESI;
	unsigned EDX;

	unsigned EDI = 0;

	ESI = PAGE_SIZE;

	unsigned ECX = (MAX_PAGEDWORD + 1) * 4;
	MAX_PAGEDWORD = ECX;
L2:
	ECX -= EDI;
	if (ECX == 0)
	    goto L9;
L21:
	unsigned EAX = *(unsigned *)(EBP + EDI);	// FIND FIRST NON-ZERO BYTE
	EDI += 4;
	ECX -= 4;
	if (!ECX)
	    goto L23;
	if (!EAX)
	    goto L21;
L24:
	ECX = 0;
L25:
	if (EAX & 0xFF)
	    goto L3;
L26:
	EAX >>= 8;
	ECX += 8;
	if (!(EAX & 0xFF))
	    goto L26;
L3:
	unsigned ECXsave = ECX;
L31:
	ECX++;
	if (!(EAX & 1))
	{
	    EAX = (EAX & 0xFFFFFF00) | ((EAX & 0xFF) >> 1);
	    goto L31;
	}
	EAX = (EAX & 0xFFFFFF00) | ((EAX & 0xFF) >> 1);

	// PAGE_NUMBER IS (EDI-4)*8+ECX-1
	if (ESI == PAGE_SIZE)
	    goto L5;
L4:
	unsigned EBX = EDI*8+ECX-33;

	*(unsigned *)(ESI+EDX) = EBX;
	ESI += 4;

	if (EAX & 0xFF)
	    goto L31;

	ECX = ECXsave;
	if (EAX)
	    goto L26;
L45:
	ECX = MAX_PAGEDWORD;
	goto L2;

L5:
	EDX = (unsigned)_get_new_dssi();
	ESI = 0;
	goto L4;

L23:
	if (EAX)
	    goto L24;
L9:
	// FAKE SPACE FOR DICT_OFFSET
	if (ESI != PAGE_SIZE)
	    goto L91;

	EDX = (unsigned)_get_new_dssi();
	ESI = 0;
L91:
	ESI += 4;

	// SET # OF MODULES

	FSYMEND = ESI;
	EBX = (char *)SCAN_EXETABLE_PTR - ((char *)&EXETABLE+4);	// # OF BLOCKS * 4

	ESI >>= 2;
	EAX = PAGE_SIZE/4/4;	// MODULES PER BLOCK / 4
	EAX *= EBX;
	EAX += ESI;
	LIB_MODULES = EAX;		// MODULE COUNT (INCLUDING DICTIONARY...)
	_release_fsymbeg();
}

void _release_fsymbeg()
{
	void *s;
	__asm
	{
	    xor	    EAX,EAX
	    xchg    EAX,FSYMBEG
	    mov     s,EAX
	}

	_release_segment(s);
}

void *_get_new_dssi()
{
	void **EBX = SCAN_EXETABLE_PTR;
	void *EDX = _get_new_phys_blk();
	*EBX = EDX;
	EBX++;
	SCAN_EXETABLE_PTR = EBX;
	return EDX;
}


void _do_module_ptrs()
{
	// COPY MODULE-PAGE# TO 4-BYTE OFFSET IN LIBRARY-FILE

	LIBRARY_STRUCT *ls = CURNLIB_GINDEX;
	unsigned EAX = LIB_MODULES;		// INCLUDING DICTIONARY
	ls->_LS_MODULES_LEFT = EAX - 1;		// -1 means don't count dictionary
	ls->_LS_MODULES = EAX;

	// ALLOCATE STORAGE FOR MODULE_PTRS
	if (EAX < PAGE_SIZE/4/2)	// use standard alloc if <.5 of a block
	{
	    // TYPICALLY <1K OR 2K MODULES
	    unsigned ECX = EAX;
	    unsigned *EDI = (unsigned *)_p1only_pool_get(EAX * 4);
	    ls->_LS_MODULE_PTRS = (void **)EDI;

	    unsigned EBX = LIB_PAGE_SIZE;
	    unsigned *ESI = (unsigned *)EXETABLE[0];

	    if (--ECX)
	    {
		do
		{
		    *EDI++ = *ESI++ * EBX;
		} while (--ECX);
	    }
	    void *pe = EXETABLE[0];

	    *EDI++ = DICT_OFFSET;

	    EXETABLE[0] = (void *)ECX;
	    LIB_MODULE_PTR_LAST_OFFSET = (unsigned)EDI;

	    // Release storage
	    _release_segment(pe);
	    return;
	}

	// TRICKY WITH MORE THAN 2K MODULES
	// EAX IS # OF MODULES	(INCLUDING DICTIONARY)
	EAX--;
	void **EBX = &EXETABLE[0];

	EAX >>= PAGE_BITS-2;		// # OF MODULES PER BLOCK
	unsigned ECX = LIB_MODULES;

	EAX++;			// # OF BLOCKS WE WILL NEED

	LIB_MODULE_PTR_BLOCKS = EAX;
	void **EDI = (void **)_p1only_pool_get(EAX * 4);	// allocate for block pointers
	ls->_LS_MODULE_PTRS = EDI;
L560:
	unsigned *ESI = *EBX++;
L56:
	// LEAVE THIS IN FAST MEMORY
	void **pf = (void **)_get_new_phys_blk();

	*EDI++ = pf;

	void **EDIsave = EDI;			// PTR TO STORE BLOCKS
	EDI = pf;

	EAX = ECX;
	ECX = PAGE_SIZE / 4;

	if (EAX <= ECX)
	    ECX = EAX;
	unsigned ECXsave = ECX;

	do
	{
	    *EDI++ = (void *)(*ESI++ * LIB_PAGE_SIZE);
	} while (--ECX);

	ECX = EAX;
	EAX = ECXsave;			// COUNTERS

	LIB_TEMP_WORD = (unsigned)EDI;
	EDI = EDIsave;			// MODULE_PTRS STUFF

	ECX -= EAX;
	if (!ECX)
	    goto L591;

	void *pv = EBX[-1];
	EBX[-1] = 0;
	_release_segment(pv);

	goto L560;

L591:
	EDI = (void **)LIB_TEMP_WORD;	// MAKE DICT_OFFSET LAST ENTRY
	ECX = DICT_OFFSET;

	pv = EBX[-1];

	EDI[-1] = (void *)ECX;
	LIB_MODULE_PTR_LAST_OFFSET = (unsigned)EDI;

	EBX[-1] = 0;
	_release_segment(pv);

	// IF MORE THAN ONE BLOCK, BUILD LIST OF MAXES
	ECX = LIB_MODULES;
	LIBRARY_STRUCT *ls2 = CURNLIB_GINDEX;
	if (ECX > PAGE_SIZE / 4)
	{
	    void **EDI = &EXETABLE[0];
	    void **ESI = ls2->_LS_MODULE_PTRS;

    L592:
	    void **EDX = *ESI++;		// GET LAST GUY IN THIS BLOCK

	    *EDI++ = EDX[PAGE_SIZE/4 - 1];	// store in EXETABLE

	    if (ECX > PAGE_SIZE / 4)
	    {
		ECX -= PAGE_SIZE / 4;
		goto L592;
	    }

	    EDI[-1] = (void *)DICT_OFFSET;	// DICT_OFFSET IS LAST ONE FROM PARTIAL BLOCK
	    LIB_MODULE_PTR_LAST_OFFSET -= (unsigned)EDX;
	}
}


#define HASHMOD     0x25
#define BUCKETPAGE  512
#define BUCKETSIZE  (BUCKETPAGE - HASHMOD - 1)

void _add_extrns_to_list()
{
	// Read directory name table, install symbols
	LIB_SYMBOLS = 0;
L00:
	unsigned char *pblk = READ_NEXT_DIRECTORY_BLOCK();	// pblk points to blk
	unsigned DICT_BLOCKS_THIS_PAGE = PAGE_SIZE/BUCKETPAGE;
	unsigned char *c = pblk;
L0:
	unsigned char *s = c;

	for (int i = 0; i < HASHMOD; i++)
	{
	    unsigned d = *s++;
	    if (d)			// usually something there
	    {
		unsigned char *pd = c + d * 2;
		unsigned a = *pd;		// name length
		if (a == 0xFF && !pd[1])	// if long name
		{
		    a = *(unsigned short *)(pd + 2) + 3;	// long name length + overhead
		}

		if (pd[a] == '!')		// ! denotes a module name
		{
		    // a = paragraph where module is stored
		    a = *(unsigned short *)(pd + a + 1);	// need this for detecting FIXLIB needs?
		}
		else				// else a symbol
		{
		    LIB_SYMBOLS++;		// count for help detecting FIXLIB needs
		    a = _lib_symbol(pd);
		}
		_install_module_page(a);
	    }
	}
	c += BUCKETPAGE;
	if (--DICT_BLOCKS)
	{
	    if (--DICT_BLOCKS_THIS_PAGE)
		goto L0;
	    CURN_DICT_LOG = NULL;		// for cancel
	    _release_io_segment(pblk);
	    goto L00;				// try next 512-byte chunk
	}

	// No more blocks
	CURN_DICT_LOG = NULL;			// for cancel
	_release_io_segment(pblk);
}

unsigned _opti_move(void *xEDI, void *xESI, unsigned xEAX)
{
    __asm
    {
	mov	EDI,xEDI
	mov	ESI,xESI
	mov	EAX,xEAX
	call	dword ptr OPTI_MOVE
	mov	EAX,EDX
    }
}

unsigned _lib_symbol(unsigned char *ESI)
{
	// ESI IS SYMBOL, RETURN MODULE IN EAX

	TPTR_STRUCT *EDI = &SYMBOL_TPTR;

	unsigned EAX = _get_omf_name_length_routine(&ESI);
	EDI->_TP_LENGTH = EAX;			// store symbol length
	unsigned EDX = _opti_move(&EDI->_TP_TEXT[0], ESI, EAX);

	unsigned pgnum = *(unsigned short *)(ESI + EAX);

	SYMBOL_STRUCT *ECX;
	EAX = _far_install(EDX, &ECX);	// EAX IS INDEX, ECX IS PHYS

	pgnum &= 0xFFFF;

	switch (ECX->_S_NSYM_TYPE & NSYM_ANDER)
	{
	    case NSYM_UNDEFINED:     goto L1;		// UNDEFINED, ITS MINE
	    case NSYM_ASEG:          goto L9;		// ASEG, IGNORE
	    case NSYM_RELOC:         goto L9;		// RELOC, IGNORE
	    case NSYM_COMM_NEAR:     goto L3;		// NEAR COMMUNAL, UNLIST, MAKE IT MINE
	    case NSYM_COMM_FAR:      goto L3;		// FAR COMMUNAL, UNLIST, MAKE IT MINE
	    case NSYM_COMM_HUGE:     goto L3;		// HUGE COMMUNAL, UNLIST, MAKE IT MINE
	    case NSYM_CONST:         goto L9;		// CONST, IGNORE
	    case NSYM_LIBRARY:       goto L9;		// LIBRARY, IGNORE
	    case NSYM_IMPORT:        goto L9;		// IMPORTED, IGNORE
	    case NSYM_PROMISED:      goto L9;		// PROMISED, IGNORE
	    case NSYM_EXTERNAL:      goto L2;		// EXTERNAL, MINE
	    case NSYM_WEAK_EXTRN:    goto L4;		// WEAK EXTERNAL, MINE
	    case NSYM_POS_WEAK:      goto L9;		// POS-WEAK, CANNOT
	    case NSYM_LIBRARY_LIST:  goto L9;		// LIBRARY_LIST, IGNORE
	    case NSYM__IMP__UNREF:   goto L1;		// __imp__UNREF, ITS MINE
	    case NSYM_ALIASED:       goto L9;		// ALIASED, IGNORE
	    case NSYM_COMDAT:        goto L9;		// COMDAT, IGNORE
	    case NSYM_WEAK_DEFINED:  goto L4;		// WEAK_DEFINED, MINE
	    case NSYM_WEAK_UNREF:    goto L4;		// WEAK-UNREF, MINE
	    case NSYM_ALIASED_UNREF: goto L9;		// ALIASED-UNREF, IGNORE
	    case NSYM_POS_LAZY:      goto L9;		// POS-LAZY, CANNOT
	    case NSYM_LAZY:          goto LS_LAZY;	// LAZY, UNLIST, PUT IN EXTRN LIST, MINE
	    case NSYM_LAZY_UNREF:    goto L1;		// LAZY-UNREF, MINE
	    case NSYM_ALIAS_DEFINED: goto L9;		// ALIASED-DEFINED, IGNORE
	    case NSYM_LAZY_DEFINED:  goto LS_LAZY_DEF;	// LAZY-DEFINED, UNLIST, EXTRN MINE
	    case NSYM_NCOMM_UNREF:   goto L11;		// NCOMM-UNREF, MINE
	    case NSYM_FCOMM_UNREF:   goto L11;		// FCOMM-UNREF, MINE
	    case NSYM_HCOMM_UNREF:   goto L11;		// HCOMM-UNREF, MINE
	    case NSYM__IMP__:        goto L5;		// __imp__, MINE
	    case NSYM_UNDECORATED:   goto L9;		// UNDECORATED, CAN'T HAPPEN
//	    default:
//		assert(0);
	}
L4:
	// Weak extrn, can still be linked to a library in case of hard extrn later
	if (ECX->_S_LIB_GINDEX == 0)	// already declared another lib
	    goto L2;

	return pgnum;

L3:
	if (!(COMDEF_SEARCH & 0xFF))
	    goto L99;

	// Communal, change to external
	_remove_from_communal_list((void *)EAX, ECX);
L31:
	_add_to_external_list((void *)EAX, ECX);
	goto L2;

L5:
	_remove_from__imp__list((void *)EAX, ECX);
	goto L31;

L11:
	if (!(COMDEF_SEARCH & 0xFF))
	    goto L99;
L1:
	ECX->_S_PREV_SYM_GINDEX = LIB_UNUSED_SYMBOLS_GINDEX;
	LIB_UNUSED_SYMBOLS_GINDEX = (void *)EAX;
	ECX->_S_NSYM_TYPE = NSYM_LIBRARY;
L2:
	// Store some things like library ptr, module page
	ECX->_S_LIB_MODULE = pgnum;
	ECX->_S_LIB_GINDEX = (int)CURNLIB_GINDEX;	// LIBRARY_STRUCT*, should use union
L99:
	return pgnum;

LS_LAZY:
	// Lazy, change to external
	_remove_from_lazy_list((void *)EAX, ECX);
	goto L31;

LS_LAZY_DEF:
	// Lazy-defined, change to external
	_remove_from_lazy_defined_list((void *)EAX, ECX);
	goto L31;

L9:
	if (WARN_LIB_DUPS & 0xFF)
	{
	    _warn_symbol_text_ret(LIB_DUP_ERR);
	}
	return pgnum;
}


void _reference_libsym(void *EAX, SYMBOL_STRUCT *ECX)
{
	// EAX is GINDEX, ECX is phys - not currently in any list
	// Add this to end of EXTRN_LIST for this library, and
	// request its blocks

	LIBRARY_STRUCT *ESI = (LIBRARY_STRUCT *)ECX->_S_LIB_GINDEX;

	SYMBOL_STRUCT *EDI = (SYMBOL_STRUCT *)ESI->_LS_LAST_EXTRN_GINDEX;
	SYMBOL_STRUCT *EDX = EDI;
	if (EDI)
	{
	    EDI->_S_NEXT_SYM_GINDEX = EAX;
	}
	else
	{
	    ESI->_LS_FIRST_EXTRN_GINDEX = (SYMBOL_STRUCT *)EAX;
	}
	ECX->_S_PREV_SYM_GINDEX = EDX;
	ESI->_LS_LAST_EXTRN_GINDEX = (int)EAX;

	ECX->_S_NEXT_SYM_GINDEX = 0;
	ECX->_S_NSYM_TYPE = NSYM_LIBRARY_LIST;

	_lib_sym_request((int)EAX, ECX);		// DS:BX IS PHYS, DX IS INDEX
}


void **_conv_axesdicx_to_module_ptr_dssi(int EAX, LIBRARY_STRUCT *ECX)
{
	/* EAX	MODULE #
	 * ECX	LIB_GINDEX
	 *
	 * Returns
	 *	EAX	MODULE_PTR
	 *	ECX	LIBRARY_STRUCT
	 */

	unsigned EBX = ECX->_LS_MODULES;

	void **EDX = ECX->_LS_MODULE_PTRS;

	if (EBX < PAGE_SIZE/4/2)
	    return EDX + EAX;

	EBX = EAX;
	EAX &= (PAGE_SIZE-1) >> 2;
	EBX >>= PAGE_BITS - 2;

	return (void **)EDX[EBX] + EAX;
}

//__declspec(naked)
void _lib_sym_request(int notused, SYMBOL_STRUCT *ss)
{
#if 0
    __asm
    {
	mov ECX,8[ESP]	;
		push	EDI	;
		push	ESI	;
		mov	EDI,0x10[ECX]	;
		mov	ESI,0x14[ECX]	;
		mov	EAX,EDI	;
		mov	ECX,ESI	;
push ECX	;
push ECX	;
push EAX	;
call	_conv_axesdicx_to_module_ptr_dssi	;
add ESP,8	;
pop ECX	;
		mov	DL,3[EAX]	;
		or	DL,DL	;
		jns	LBD7	;
		pop	ESI	;
		pop	EDI	;
		ret	;
LBD7:		push	EBX	;
		mov	EBX,03Ch[ECX]	;
		or	DL,0x80	;
		inc	EBX	;
		mov	3[EAX],DL	;
		mov	0x3C[ECX],EBX	;
		dec	EBX	;
		jne	LBF3	;
		push	ECX	;
		push	EAX	;
		mov	EAX,ESI	;
push EAX	;
call	_add_to_request_list	;
add ESP,4	;
		pop	EAX	;
		pop	ECX	;
LBF3:		test	byte ptr _HOST_THREADED,0xFF	;
		je	LC34	;
		mov	EDX,[EAX]	;
		add	EAX,4	;
		test	EAX,0xFFF	;
		je	LC38	;
LC08:		mov	EDI,ECX	;
		mov	ECX,[EAX]	;
		and	EDX,0x7FFFFFFF	;
		and	ECX,0x7FFFFFFF	;
		shr	EDX,0xE	;
		dec	ECX	;
		shr	ECX,0xE	;
		dec	EDX	;
		sub	ECX,EDX	;
		inc	EDX	;
		mov	ESI,EDX	;
LC25:		push	ECX	;
		mov	EAX,ESI	;
		mov	ECX,EDI	;
push ECX	;
push EAX	;
call	_request_block	;
add ESP,8	;
		pop	ECX	;
		inc	ESI	;
		dec	ECX	;
		jne	LC25	;
LC34:		pop	EBX	;
		pop	ESI	;
		pop	EDI	;
		ret	;
LC38:		lea	EAX,1[EDI]	;
		mov	ECX,ESI	;
		push	EDX	;
push ECX	;
push ECX	;
push EAX	;
call	_conv_axesdicx_to_module_ptr_dssi	;
add ESP,8	;
pop ECX	;
		pop	EDX	;
		jmp short	LC08	;
    }
#else
	//printf("lib_sym_request(ss = %p)\n", ss);
	// This symbol is to be officially requested. It's already linked to the correct library
	// ss IS SYMBOL, notused IS INDEX

	unsigned EDI = ss->_S_LIB_MODULE;	// library module #
	LIBRARY_STRUCT *ESI = (LIBRARY_STRUCT *)ss->_S_LIB_GINDEX;

	// Get pointer to addresses
	LIBRARY_STRUCT *ECX = ESI;

	unsigned *EAX = (unsigned *)_conv_axesdicx_to_module_ptr_dssi(EDI, ECX);	// MODULE_PTR IN EDI

	// Fails on next line with EXTDEF for ModuleInfo, EAX has only low 16 bits set to a value
	if (((signed char *)EAX)[3] < 0)
	    return;				// module already requested

	// First request for this module

	((signed char *)EAX)[3] |= 0x80;	// select it
	int EBX = ECX->_LS_SELECTED_MODULES + 1;
	ECX->_LS_SELECTED_MODULES = EBX;	// Count selected modules this library
	// If this is first requested module, add this library to tail of
	// request list, lowest priority
	if (--EBX == 0)
	    _add_to_request_list(ESI);	// ESI must be library index

#if fgh_inthreads
	// ESI IS LIB_GINDEX,EDI IS MODULE #
	// EAX IS MODULE PTR,ECX IS LIB_PTR
	if (!(_HOST_THREADED & 0xFF))
	    goto L39;

	unsigned EDX = *EAX;
	EAX++;
	if (!((unsigned)EAX & (4*1024-1)))
	{
	    ECX = ESI;
	    EAX = (unsigned *)_conv_axesdicx_to_module_ptr_dssi((int)EDI + 1, ECX);	// LIBRARY_STRUCT IN ECX, MODULE_PTR IN EAX
	}
	unsigned n = *EAX;

	EDX &= 0x7FFFFFFF;
	EDX >>= PAGE_BITS;
	--EDX;

	n = ((n & 0x7FFFFFFF) - 1) >> PAGE_BITS;	// ending block #
	n -= EDX;			// # of blocks to mark

	unsigned b = EDX + 1;
L3:
	//printf("_request_block(b = %u, ECX = %p)\n", b, ECX);
	_request_block(b, ECX);
	b++;
	if (--n)
	    goto L3;
L39:	;
#endif
#endif
}

#if	fgh_inthreads

void _request_block(unsigned EAX, LIBRARY_STRUCT *ECX)
{
	// EAX is block #, ECX is LIB_PTR

	unsigned EDX = ECX->_LS_BLOCKS;
	void *ESI = ECX->_LS_BLOCK_TABLE[0];

	if (EDX >= PAGE_SIZE/8/2)
	    goto L4;

	unsigned EDI = EAX;
L49:
	unsigned char DL = ((unsigned char *)ESI)[EAX*8+7];
	DL ^= 0x80;
	if (DL & 0x80)			// requested yet?
	    goto L6;
	return;

L4:
	// There are PAGE_SIZE/8 blocks per page
	EDX = EAX;
	EDI = EAX;

	EDX >>= PAGE_BITS-3;
	EAX &= (PAGE_SIZE-1) >> 3;

	ESI = ECX->_LS_BLOCK_TABLE[EDX];
	goto L49;

L6:
	((unsigned char *)ESI)[EAX*8+7] = DL;		// request it
	unsigned *pu = ECX->_LS_NEXT_REQUEST_BLOCK;

	ECX->_LS_OS2_BLOCKS_REQUESTED++;	// must be done in one instruction - multithreading
	MYL2_STRUCT *m2 = ECX->_LS_THREAD_LOCALS;
	*pu = EDI;
	ECX->_LS_NEXT_REQUEST_BLOCK = pu + 1;
	_release_eax_bump(&m2->MYL2_LIB_BLOCK_SEM);
}

#endif

void _process_libs()
{
	// Go all the way through list once
	for (int EAX = SELECTED_LIB_COUNT; EAX; EAX--)
	{
		_process_lib();
	}
}

void _obj_mod();

void _process_lib()
{
	// Here is where it all happens
	_select_library(FIRST_REQUEST_LIB_GINDEX);	// becomes FIRST_LIBRARY & LIB_PTR
L1:
	if (!_find_earliest_selected_module())
	    goto L5;		// no modules, jump

	_verify_block_loaded();	// destroys regs

	DURING_OBJ = 0xFF;

	// Process module

	_obj_mod();

	MYI_STRUCT *ms = OBJ_DEVICE;

	DURING_OBJ = 0;

	ms->MYI_PTRA = 0;
	ms->MYI_BLOCK = 0;

	// Now, decrement counter for that block
	_dec_lib_block();
L5:
	// Clear module-selected bit, count

	LIBRARY_STRUCT *ls = CURNLIB_GINDEX;
	if (--ls->_LS_SELECTED_MODULES)
	    goto L3;

	// Last selected module, remove lib from requested list
	_remove_from_request_list(ls);	// deselect
L3:
	if (CURNLIB_GINDEX)
	    goto L1;
}


int _find_earliest_selected_module()
{
	// NULL if none selected
	LIBRARY_STRUCT *EAX = CURNLIB_GINDEX;

	SYMBOL_STRUCT *ss = EAX->_LS_FIRST_EXTRN_GINDEX;
	if (ss)
	{
		CURN_LIB_MODULE = ss->_S_LIB_MODULE;
		return 1;
	}
	return 0;
}

void _verify_block_loaded()
{
	// Set up stuff for reading this module
	LIBRARY_STRUCT *ECX = CURNLIB_GINDEX;

	void *EAX2 = *_conv_axesdicx_to_module_ptr_dssi(CURN_LIB_MODULE, ECX);	// library struct in ECX, MODULE_PTR in EAX2

	unsigned EDX = (unsigned)EAX2;
	EAX2 = (void *)(EDX & PAGE_SIZE-1);
	EDX &= 0x7FFFFFFF;
	EDX >>= PAGE_BITS;
	OBJ_DEVICE->MYI_PTRA = EAX2;
	unsigned EDI = ECX->_LS_BLOCKS;
	void *ESI = ECX->_LS_BLOCK_TABLE[0];
	if (EDI >= PAGE_SIZE/8/2)
	    goto L4;

	ECX = ((LIBRARY_STRUCT **)ESI)[EDX*2];
	void **EAX = (void **)((char *)ESI + EDX*8);
	void **EBX = (void **)((char *)ESI + PAGE_SIZE);
L49:
	CURN_LIB_BLOCK = EDX;	// see if it is available

	CURN_LIB_BLOCK_PTR = EAX;
	CURN_LIB_BLOCK_PTR_LIMIT = EBX;
	if (ECX)
	    goto L6;
L5:
	// Not there, read a block
	_read_library_block(EDX);
	_verify_block_loaded();
	return;

L4:
	// There are PAGE_SIZE/8 blocks per page
	unsigned EBX3 = EDX >> (PAGE_BITS - 3);
	unsigned ESI2 = EDX & ((PAGE_SIZE - 1) >> 3);

	CURN_LS_BLOCK_TABLE_PTR = &ECX->_LS_BLOCK_TABLE[EBX3];

	EBX = (void **)((char *)ECX->_LS_BLOCK_TABLE[EBX3] + PAGE_SIZE);
	ECX = ((LIBRARY_STRUCT **)EAX)[ESI2*2];
	EAX = (void**)((char *)EAX + ESI2*8);
	goto L49;

L6:
	// Looks like the block has already been read
	EDX++;
	MYI_STRUCT *EBX2 = OBJ_DEVICE;

	/* NEED:
	 *	MYI_PTR.SEGM
	 *	MYI_LOG
	 *	MYI_PBLK
	 *	MYI_COUNT
	 *	CURN_LIB_BLOCK
	 */
	unsigned EAX3;
	if (EDI == EDX)			// last block handled specially
	    goto L75;
L7:
	EAX3 = PAGE_SIZE;
L71:
	EDX = (unsigned)EBX2->MYI_PTRA;
	EBX2->MYI_BLOCK = ECX;

	EAX3 -= EDX;
	ECX = (LIBRARY_STRUCT *)((char *)ECX + EDX);

	EBX2->MYI_COUNT = EAX3;
	EBX2->MYI_PTRA = ECX;
	return;

L75:
	EAX3 = FILNAM.NFN_FILE_LENGTH;
	EAX3 &= PAGE_SIZE-1;
	if (EAX3)
	    goto L71;
	goto L7;
}


void _read_library_block(int ECX)
{

	// ECX is block # to read from CURNLIB_GINDEX
	void **EDX = CURN_LIB_BLOCK_PTR;
#if fgh_inthreads
	if (!(_HOST_THREADED & 0xFF))
	    goto L19;

	MYL2_STRUCT *m2 = CURNLIB_GINDEX->_LS_THREAD_LOCALS;
L1:
	// Wait till thread reads a block
	_capture_eax(&m2->MYL2_BLOCK_READ_SEM);	// 15 SECOND DELAY FOR DEBUG

	if (!*EDX)		// has it been read?
	    goto L1;		// loop till this block read
	return;
#endif

L19:
	// Allocate logical block
	void *ESI = _get_new_phys_blk();

	MYI_STRUCT *EBX = OBJ_DEVICE;

	*EDX = ESI;
	if (EBX->MYI_HANDLE)
	    // Library open file
	    _lib_reopen();

	_dosposition_i(EBX, ECX << PAGE_BITS);		// seek if necessary

	ECX++;
	if (CURNLIB_GINDEX->_LS_BLOCKS == ECX)
	    goto L22;
L25:
	ECX = PAGE_SIZE;		// even block size
L3:
	_dosread(EBX, ECX, ESI);
	return;

L22:
	ECX = FILNAM.NFN_FILE_LENGTH;
	ECX &= PAGE_SIZE-1;
	if (ECX)
	    goto L3;
	goto L25;
}


void _dec_lib_block()
{
	// Another module completed from this block
	void **EDX = CURN_LIB_BLOCK_PTR;
	void *EAX = (void *)((char *)EDX[1] - 1);
	EDX[1] = EAX;
	if ((int)EAX & 0x7FFFFFFF)
	    return;

	// Last module in that block, kill it
	EDX[1] = 0;
	EAX = *EDX;
	*EDX = 0;
	_release_io_block(EAX);		// release memory used

	LIBRARY_STRUCT *ls = CURNLIB_GINDEX;
	if (--ls->_LS_BLOCKS_LEFT)
	    return;

	// Totally finished with this library
	void *h;
	asm
	{
		mov	ECX,ls
		xor	EAX,EAX
		xchg	[ECX]._LS_HANDLE,EAX
#if	fgh_inthreads
		test	byte ptr _HOST_THREADED,0FFh
		jnz	L56
#endif
		mov	EDX,OBJ_DEVICE
		xor	EAX,EAX
		xchg	[EDX].MYI_HANDLE,EAX
L56:		mov	h,EAX
	}
	if (h)
	    _close_handle(h);
	_release_misc_blocks(ls);

	// Remove him completely from library list
	_remove_from_library_list(CURNLIB_GINDEX);
}


void _release_misc_blocks(LIBRARY_STRUCT *EAX)
{
	// If more than 2k modules, release MODULE_PTRS

	LIBRARY_STRUCT *EDX = EAX;
	unsigned ECX = EAX->_LS_MODULES;

	if (ECX < PAGE_SIZE/4/2)
	    goto L6;

	void **ESI = EDX->_LS_MODULE_PTRS;
L60:
	_release_block(*ESI++);

	if (ECX > PAGE_SIZE/4)
	{
	    ECX -= PAGE_SIZE/4;
	    goto L60;
	}
L6:
	// If more than PAGE_SIZE/4/2 blocks, release that storage

	ECX = EDX->_LS_BLOCKS;
	void **p = &EDX->_LS_BLOCK_TABLE[0];

	if (ECX < PAGE_SIZE/8/2)
	    goto L7;
L65:
	_release_block(*p++);

	if (ECX > PAGE_SIZE/8)
	{
	    ECX -= PAGE_SIZE/8;
	    goto L65;
	}
L7:
	;
}


void _select_library(LIBRARY_STRUCT *EAX)
{
	// EAX is library index to be selected as current
	// Now do select
	// 1. store LIB_PTR
	// 2. store OBJNAM structure
	// 3. store MYI_STUFF

	LIBRARY_STRUCT *ESI = EAX;

	CURNLIB_GINDEX = EAX;

#if fgh_inthreads
	if (_HOST_THREADED & 0xFF)
	    goto L1;
#endif
	MYI_STRUCT *EDI = OBJ_DEVICE;

	EDI->MYI_HANDLE = ESI->_LS_HANDLE;
	EDI->MYI_PHYS_ADDR = ESI->_LS_FILE_POSITION;

L1:

	FILE_LIST_STRUCT *ECX = ESI->_LS_FILE_LIST_GINDEX;
	CURN_FILE_LIST_GINDEX = ECX;
	CURNLIB_NUMBER = ESI->_LS_LIBNUM;

	_move_file_list_gindex_nfn(&FILNAM, ECX);
	_make_lib_mru();
}


void _make_lib_mru()
{
	// Make CURN_LIBNUM head of list

	if (CURNLIB_GINDEX == FIRST_LIB_GINDEX)
	    return;	// already mru
	_remove_from_library_list(CURNLIB_GINDEX);

	LIBRARY_STRUCT *EDX = FIRST_LIB_GINDEX;
	LIBRARY_STRUCT *EAX = CURNLIB_GINDEX;
	LIBRARY_STRUCT *EBX = EDX;
	LIBRARY_STRUCT *ECX = EAX;

	if (!EDX)
	    goto ADD_FIRST;

	EBX->_LS_PREV_LIB_GINDEX = EAX;
AF_RET:
	FIRST_LIB_GINDEX = EAX;
	ECX->_LS_NEXT_LIB_GINDEX = EDX;
	return;

ADD_FIRST:
	LAST_LIB_GINDEX = EAX;
	goto AF_RET;
}


void _add_to_request_list(LIBRARY_STRUCT *EDI)
{
	// Add this library to end of 'request' list
	// EDI is lib index

	_capture_eax(&LIB_REQUEST_ORDER_SEM);

	LIBRARY_STRUCT *ECX = LAST_REQUEST_LIB_GINDEX;
	int EDX = SELECTED_LIB_COUNT;
	if (!ECX)
	    goto L5;

	ECX->_LS_NEXT_RLIB_GINDEX = EDI;
L59:
	EDX++;
	LAST_REQUEST_LIB_GINDEX = EDI;
	SELECTED_LIB_COUNT = EDX;

#if fgh_inthreads
	// Add to a thread's linked list
	if (!(_HOST_THREADED & 0xFF))
	    goto L9;

	MYL2_STRUCT *EAX = _get_next_readlib_thread();	// returns locals
	MYL2_STRUCT *ESI = EAX;

	ECX = EAX->MYL2_LAST_REQUEST_LIB_GINDEX;
	if (!ECX)
	    goto L6;

	ECX->_LS_NEXT_TRLIB_GINDEX = EDI;
L69:
	ESI->MYL2_LAST_REQUEST_LIB_GINDEX = EDI;

	EDI->_LS_THREAD_LOCALS = ESI;
	int n = EDI->_LS_OS2_BLOCKS_REQUESTED;	// how can there be blocks requested???
	if (!n)
	    goto L79;
L7:
	_release_eax_bump(&ESI->MYL2_LIB_BLOCK_SEM);

	if (--n)
	    goto L7;
L79:
	// Free up lib_list
	_release_eax_bump(&LIB_REQUEST_ORDER_SEM);
#endif
L9:
	return;

L5:
	FIRST_REQUEST_LIB_GINDEX = EDI;
	goto L59;

#if fgh_inthreads

L6:
	EAX->MYL2_FIRST_REQUEST_LIB_GINDEX = EDI;
	goto L69;

#endif
}

#if	fgh_inthreads

MYL2_STRUCT *_get_next_readlib_thread()
{
    int ECX = NEXT_READLIB_THREAD;		// NEXT THREAD #
    ECX++;
    ECX &= N_R_THREADS - 1;		// WRAP AROUND 0-3

    NEXT_READLIB_THREAD = ECX;

    MYL2_STRUCT *EDI = LIB_THREAD_LOCALS[ECX];	// EXISTS YET?
    if (!EDI)
    {
	// MUST INITIATE A NEW THREAD
	EDI = _get_4k_segment();

	LIB_THREAD_LOCALS[ECX] = EDI;

	memset(EDI, 0, sizeof(MYL2_STRUCT));

	// OK, SET UP SEMAPHORES AND VARIABLES

	_init_eax(&EDI->MYL2_LIB_BLOCK_SEM);
	_capture_eax(&EDI->MYL2_LIB_BLOCK_SEM);	// NO BLOCKS FOR LIBTHREAD TO READ YET

	_init_eax(&EDI->MYL2_BLOCK_READ_SEM);
	_capture_eax(&EDI->MYL2_BLOCK_READ_SEM);	// NO BLOCKS FOR THREAD 1 TO READ YET

	EDI->MYL2_OPEN_FLAGS = 0+0x80+0x20+0x200;	// R/O, NO INHERIT,DENY WRITE, RANDOM ACCESS

	// NOW START THREAD
	HANDLE h = CreateThread(0, 4*1024, (LPTHREAD_START_ROUTINE)&LIBREADT, EDI, 0, &EDI->MYL2_LIBREAD_THREAD_ID);
	if (!h)
	    _err_abort(THREAD_FAIL_ERR);

	EDI->MYL2_LIBREAD_THREAD_HANDLE = h;
	_do_dossleep_0();			// LET THREAD START
	_do_dossleep_0();			// AND LIVE THRU PRIORITY CHANGE
    }
    return EDI;
}

#endif

void _remove_from_request_list(LIBRARY_STRUCT *EAX)
{
	--SELECTED_LIB_COUNT;
#if fgh_inthreads
	if (_HOST_THREADED & 0xFF)
	    goto L1;
#endif
	// Need to store current handle and file position
	LIBRARY_STRUCT *ESI = EAX;
	ESI->_LS_HANDLE = OBJ_DEVICE->MYI_HANDLE;
	OBJ_DEVICE->MYI_HANDLE = 0;
	ESI->_LS_FILE_POSITION = OBJ_DEVICE->MYI_PHYS_ADDR;
L1:
	// Remove from linked list
	_capture_eax(&LIB_REQUEST_ORDER_SEM);

	ESI = CURNLIB_GINDEX;

	CURNLIB_GINDEX = 0;
	LIBRARY_STRUCT *EDX = ESI->_LS_NEXT_RLIB_GINDEX;

	ESI->_LS_NEXT_RLIB_GINDEX = 0;
	FIRST_REQUEST_LIB_GINDEX = EDX;

	if (!EDX)
	    LAST_REQUEST_LIB_GINDEX = 0;
#if fgh_inthreads
	// Remove from thread's linked list
	if (!(_HOST_THREADED & 0xFF))
	    goto L7;

	MYL2_STRUCT *myl2 = ESI->_LS_THREAD_LOCALS;
	ESI->_LS_THREAD_LOCALS = 0;
	LIBRARY_STRUCT *EBX = ESI->_LS_NEXT_TRLIB_GINDEX;
	ESI->_LS_NEXT_TRLIB_GINDEX = 0;
	myl2->MYL2_FIRST_REQUEST_LIB_GINDEX = EBX;

	if (!EBX)
	    myl2->MYL2_LAST_REQUEST_LIB_GINDEX = EBX;
#endif
L7:
	_release_eax_bump(&LIB_REQUEST_ORDER_SEM);

	CURN_FILE_LIST_GINDEX = 0;
}


void _remove_from_library_list(LIBRARY_STRUCT *EAX)
{
	// EAX	IS LIBRARY INDEX TO REMOVE FROM LIST
	LIBRARY_STRUCT *ECX = EAX;			// save index

	LIBRARY_STRUCT *EDI = ECX->_LS_NEXT_LIB_GINDEX;
	LIBRARY_STRUCT *EBX = ECX->_LS_PREV_LIB_GINDEX;
	ECX->_LS_NEXT_LIB_GINDEX = NULL;
	ECX->_LS_PREV_LIB_GINDEX = NULL;

	ECX = EBX;
	LIBRARY_STRUCT *EDX = EDI;
	if (!EBX)
	    goto PREV_NUL;

	ECX->_LS_NEXT_LIB_GINDEX = EDI;
DO_NEXT:
	if (!EDI)
	    goto NEXT_NUL;

	EDX->_LS_PREV_LIB_GINDEX = EBX;
RFEL1:
	return;

PREV_NUL:
	FIRST_LIB_GINDEX = EDI;
	goto DO_NEXT;

NEXT_NUL:
	LAST_LIB_GINDEX = EBX;
	goto RFEL1;
}


void _close_lib_files()
{
	// close any files, release any storage
	LIBRARY_STRUCT *r;

	LAST_LIB_GINDEX = 0;
	asm
	{	xor	EAX,EAX			;
		xchg	EAX,FIRST_LIB_GINDEX	;
		mov	r,EAX			;
	}
	goto CHECK_LIB;

LIB_LOOP:
	LIBRARY_STRUCT *ESI = r;
	CURNLIB_GINDEX = r;
	LIBRARY_STRUCT *nlg = ESI->_LS_NEXT_LIB_GINDEX;
	void *h = ESI->_LS_HANDLE;
	ESI->_LS_HANDLE = NULL;
	if (h)
	{
	    _close_handle(h);
	}
LIB_CLOSED:
	if (CANCEL_REQUESTED & 0xFF)	// don't worry about releasing memory, just close handles
	    goto NEXT_LIB;

	LIBRARY_STRUCT *ls = ESI;

	// release all blocks back to storage

	unsigned ECX = ESI->_LS_BLOCKS;
	void **EDI = (void **)ESI->_LS_BLOCK_TABLE[0];
	unsigned EBX = PAGE_SIZE/8;

	if (ECX >= PAGE_SIZE/8/2)
	{
	    ESI = (LIBRARY_STRUCT *)&ESI->_LS_BLOCK_TABLE[1];
	}

NEXT_BLOCK:
	void *p = *EDI;
	if (p)
	{
	    *EDI = NULL;
	    _release_block(p);
	}
	if (--EBX == 0)
	    goto L5;
L41:
	if (--ECX)
	    goto NEXT_BLOCK;
L45:
	_release_misc_blocks(ls);		// if bigbig lib
NEXT_LIB:
	r = nlg;
CHECK_LIB:
	if (r)
	    goto LIB_LOOP;
	return;

L5:
	EDI = *(void***)ESI;
	ESI = (LIBRARY_STRUCT *)((void***)ESI + 1);
	EBX = PAGE_SIZE/8;
	if (EDI)
	    goto L41;
	goto L45;
}

void _lib_reopen()
{
    _move_file_list_gindex_nfn(&FILNAM, CURN_FILE_LIST_GINDEX);
    MYI_STRUCT *EBX;
    _open_input(&FILNAM, &EBX);
    EBX->MYI_FILLBUF = &_readlib_16k;
}

#if 0

		.DATA?

		ALIGN	4


#endif
