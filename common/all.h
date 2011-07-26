
#include "macros.h"
#include "optlink.h"
#include "errors.h"
#include "io_struc.h"
#include "exes.h"
#include "library.h"
#include "symbols.h"
#include "groups.h"
#include "segments.h"
#include "segmsyms.h"
#include "lnkdat.h"
#include "cvtypes.h"
#include "cvstuff.h"
#include "pe_struc.h"
#include "cddata.h"

// alloc\allocc
void _seg_released();
void _oom_error();
void *_sbrk();
void *_get_new_phys_blk();
void *_get_4k_segment();
void _release_4k_segment(void *p);
void _release_block(void *);
void _release_io_segment(void *);
void _release_io_block(void *);
void _release_segment(void *);
void *_get_new_phys_blk();
void *_get_new_log_blk();
void *_get_new_io_log_blk();

// alloc.allopool
void _ap_fix(unsigned EAX, ALLOCS_STRUCT *EBX, unsigned ECX);
void **_allo_pool_get1(unsigned EAX, ALLOCS_STRUCT *EBX);
void **_cv_ltype_pool_get(unsigned nbytes);
void **_cv_gtype_pool_get(unsigned nbytes);
void **_segment_pool_get(unsigned nbytes);
void **_text_pool_alloc(unsigned nbytes);
void **_p1only_pool_get(unsigned nbytes);
void **_cv_gsym_pool_get(unsigned nbytes);
void **_cv_ssym_pool_get(unsigned nbytes);
void **_cv_hashes_pool_get(unsigned nbytes);

// common.errors
extern char *ERR_TABLE[];
void _warn_nfn_ret(int errnum, NFN_STRUCT *n);
void _err_nfn_abort(int errnum, NFN_STRUCT *n);
void _warn_ret(int errnum);
void _warn_symbol_text_ret(int errnum);
void _library_error(SYMBOL_STRUCT *);
void _index_range();

// common.extdef
void _reference_impsym(void *EAX, SYMBOL_STRUCT *ECX);
void _reference_comdat(void *EAX, SYMBOL_STRUCT *ECX);

// common.lnkinit

// common.modpage
int _binser_module(int EAX, SYMBOL_STRUCT *EBX);

// common\newlib
void _dec_lib_block();
int _find_library_file(FILE_LIST_STRUCT *EAX);
unsigned _lib_symbol(unsigned char *);
void _reference_libsym(void *EAX, SYMBOL_STRUCT *ECX);
void _lib_sym_request(int notused, SYMBOL_STRUCT *ss);
void _process_libs();
void _process_lib();
int _find_earliest_selected_module();
void _request_block(unsigned EAX, LIBRARY_STRUCT *ECX);
void _verify_block_loaded();
void _read_library_block(int);
void _select_library(LIBRARY_STRUCT *EAX);
void _make_lib_mru();
void _add_to_request_list(LIBRARY_STRUCT *EDI);
void _remove_from_request_list(LIBRARY_STRUCT *EAX);
void _remove_from_library_list(LIBRARY_STRUCT *);
void _release_misc_blocks(LIBRARY_STRUCT *);
struct MYL2_STRUCT *_get_next_readlib_thread();
void _lib_reopen();

// common.subs
unsigned char *_cbta16(int n, unsigned char *dest);

// cv.cvhashesc
void _init_cv_symbol_hashes();
void _flush_cvg_temp();
unsigned _output_cv_symbol_align(struct CV_SYMBOL_STRUCT *ESI /* EAX */);
void _flush_cv_symbol_hashes(unsigned CV_INDEX);
void _do_symbol_hash();
void _do_address_hash();

// cv.cvindex
void _handle_cv_index(unsigned EAX, unsigned ECX);

// cv.cvsymbolc
unsigned _get_name_hash32(unsigned char *EAX);
unsigned _opti_hash32(unsigned EAX, unsigned char *ESI);
void _install_gsym_ref(struct CVSYMBOLS_VARS *cv, unsigned EAX, unsigned ECX, unsigned EDX, unsigned char *ESI);

// cv.instgsymc
void _install_globalsym(unsigned hash, char *ECX, CV_SYMBOL_STRUCT *ESI);

// cv.qsortadrc
unsigned _sort_hashes_garray();

// cv.recordsc
void _print_obj_device();
void _check_checksum(unsigned char *ESI);

// cv.xdebugc
void _flush_cv_temp(unsigned char *EDI);
void _big_xdebug_write(unsigned EAX, void* *ECX);
void _xdebug_normal(void *EAX, unsigned ECX);
void _xdebug_write(unsigned char *EDI);

// exe.movefina
void _move_eax_to_final_high_water(void *EAX, unsigned ECX);
void _move_eax_to_edx_final(void *EAX, unsigned ECX, unsigned EDX);

// install/addtoext
void _add_to_external_list(void *EAX, SYMBOL_STRUCT *ECX);
void _add_to_weak_list(void *EAX, SYMBOL_STRUCT *ECX);
void _add_to_aliased_list(void *EAX, SYMBOL_STRUCT *ECX);
void _add_to_lazy_list(void *EAX, SYMBOL_STRUCT *ECX);
void _add_to_communal_list(void *EAX, SYMBOL_STRUCT *ECX);
void _add_to_undecorated_list(void *EAX, SYMBOL_STRUCT *ECX);
void _add_to__imp__list(void *EAX, SYMBOL_STRUCT *ECX);
void _add_to_comdat_ordered_list(void *EAX, SYMBOL_STRUCT *ECX);
void _add_to_alias_defined_list(void *EAX, SYMBOL_STRUCT *ECX);
void _add_to_comdat_list(void *EAX, SYMBOL_STRUCT *ECX);
void _add_to_weak_defined_list(void *EAX, SYMBOL_STRUCT *ECX);
void _add_to_lazy_defined_list(void *EAX, SYMBOL_STRUCT *ECX);
void _remove_from_communal_list(void *EAX, SYMBOL_STRUCT *ECX);
void _remove_from__imp__list(void *EAX, SYMBOL_STRUCT *ECX);
void _remove_from_undecorated_list(void *EAX, SYMBOL_STRUCT *ECX);
void _remove_from_weak_list(void *EAX, SYMBOL_STRUCT *ECX);
void _remove_from_lazy_list(void *EAX, SYMBOL_STRUCT *ECX);
void _remove_from_lazy_defined_list(void *EAX, SYMBOL_STRUCT *ECX);
void _remove_from_aliased_list(void *EAX, SYMBOL_STRUCT *ECX);
void _remove_from_external_list(void *EAX, SYMBOL_STRUCT *ECX);

// install/farinst
unsigned _comdat_install(unsigned EAX, SYMBOL_STRUCT **pECX);
unsigned _far_install(unsigned EDX, SYMBOL_STRUCT **pECX);

// install/finsegat
SEGMENT_STRUCT *_find_segat_location();

// install/instcomm
void _common_inst_init(ALLOCS_STRUCT *EAX);

// install/modpage
void _init_install_module();
void _install_module_page(int EAX);

// ntio\capturec.c
void _init_semaphores();
void _init_eax(struct GLOBALSEM_STRUCT *EAX);
void _close_semaphore(GLOBALSEM_STRUCT *s);
void _capture_eax(struct GLOBALSEM_STRUCT *s);
void _release_eax(struct GLOBALSEM_STRUCT *s);
void _release_eax_bump(struct GLOBALSEM_STRUCT *s);
void _capture_stdio();
void _release_stdio();

// ntio\dotc
void DOT();

// ntio\openinc
int _open_input(NFN_STRUCT *nfn, MYI_STRUCT **pdevice);
MYI_STRUCT *_read_16k_input(MYI_STRUCT *myi);

// ntio\dosioc
MYO_STRUCT *_doswrite(MYO_STRUCT *s, unsigned bytecount, unsigned char *buf);
MYI_STRUCT *_dosread(MYI_STRUCT *s, unsigned bytecount, unsigned char *buf);
MYI_STRUCT *_dosposition_i(MYI_STRUCT *s, unsigned offset);
MYI_STRUCT *_read_16k_threaded(MYI_STRUCT *myi);
void _err_file_list_abort(unsigned char errnum, FILE_LIST_STRUCT *ECX);
void _dos_fail_move(void *p);
void _dos_fail_seteof(void *p);
void _abort();
void print(char *msg);
int hexToAscii(unsigned u);
void printbyte(unsigned b);
void halt();

// ntio\doswritec
MYO_STRUCT *_do_doswrite(MYO_STRUCT *EAX, unsigned ECX);
MYO_STRUCT *_do_doswrite_release(MYO_STRUCT* EAX, unsigned ECX, void* EDX);
MYO_STRUCT *_do_doswrite2(MYO_STRUCT* ESI, unsigned ECX, void* EDX);

// ntio.terrorsc
void _oerr_abort(int EAX, unsigned char *ECX);
void _terr_abort(int EAX, unsigned char *ECX);

// ntio.fancyc
MYI_STRUCT *_fancy_open_input(NFN_STRUCT *ESI, FILE_LISTS *ECX);
void _delete_phantom_path(NFN_STRUCT *ESI);

// ntio\findfirsc
NFN_STRUCT *_do_findfirst(NFN_STRUCT* EAX);
NFN_STRUCT *_do_findnext(NFN_STRUCT *EAX);
void _close_findnext();

// ntio\flushesc.c
struct MYO_STRUCT *_flush_trunc_close(struct MYO_STRUCT *);

// ntio\libreadtc
void LIBREADT(MYL2_STRUCT *EBX);

// ntio\libstuffc
MYI_STRUCT *_readlib_16k(MYI_STRUCT *myi);
ubyte *_get_first_sector();
unsigned char *READ_NEXT_DIRECTORY_BLOCK();
ubyte *GET_FIRST_SECTOR();

// ntio\openoutc
MYO_STRUCT* GET_OUTPUT_DEVICE();
MYO_STRUCT *_open_output(FILE_LIST_STRUCT *EDI);

// ntio.openreadc
MYI_STRUCT *OPEN_READING(FILE_LIST_STRUCT *EAX);

// ntio\printf
int printf(const char *format,...);
int sprintf(char *s,const char *format,...);
//int vsprintf(char *s,const char *format,va_list args);


// ntio\promptc
void _proper_prompt();
void *_get_stdin();
void _issue_prompt();
void _do_print_defaults();

// ntio\recohndlc
int _recover_handle();

// ntio.terrors
void _terr_abort(int, unsigned char *);

// os2link\src\first
extern void FIRST_LABEL();

// subs/getname
int _get_omf_name_length_routine(unsigned char **pp);
void xTOO_LONG();

//////// parse

// do_fnc
MYO_STRUCT* DOSPOSITION_A(MYO_STRUCT *, int);
struct NFN_STRUCT *_parse_filename(struct NFN_STRUCT *EDI);
NFN_STRUCT* _do_filename(NFN_STRUCT* ECX, CMDLINE_STRUCT* EAX);
NFN_STRUCT* _do_defaults(NFN_STRUCT* ESI, CMDLINE_STRUCT* EDI);

// get_fnc
unsigned char *_get_filename(CMDLINE_STRUCT *EAX, NFN_STRUCT* ECX, unsigned char *EDX);

// hndllibc
void _handle_libs(NFN_STRUCT *EAX);
void _handle_lod(NFN_STRUCT *EAX);

// librtnc
unsigned char *_lib_routine_final(unsigned char *EAX);

// mvfilnamc
void *_move_file_list_gindex_path_prim_ext(void *EAX, FILE_LIST_STRUCT *ECX);
void *_move_path_prim_ext(void* EAX, NFN_STRUCT* ECX);

// mvfnascc
void _move_fn_to_asciz(NFN_STRUCT* EAX);

// mvlistnf
void _move_file_list_gindex_nfn(NFN_STRUCT* EAX, FILE_LIST_STRUCT *ECX);

// mvpathc.c
int toupper(int c);
unsigned strlen(const char *p);
void *memset(void *p, int value, unsigned n);
void *memcpy(void *dst, const void *src, unsigned n);
void *memmove(void *dst, const void *src, unsigned n);
int memcmp(const void *,const void *,unsigned);
NFN_STRUCT *_move_ecxpath_eax(NFN_STRUCT* EAX, NFN_STRUCT* ECX);

// mvsrcfilc
void _move_srcprim_to_eax(NFN_STRUCT* EAX);
void _move_srcprim_to_eax_clean(NFN_STRUCT* EAX);

