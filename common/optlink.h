
void _capture_eax(struct GLOBALSEM_STRUCT *s);
void _release_eax(struct GLOBALSEM_STRUCT *s);
void _release_eax_bump(struct GLOBALSEM_STRUCT *s);
void _DELETE_EXE();
int _err_ret(int errcode);

void _err_abort(int errcode);
#pragma ZTC noreturn(_err_abort)

void _cerr_abort(int errcode);
void _critical_error();
void _FLUSH_DISABLE_MAPOUT();
void _do_dossleep_0();
int _mesout(char *p);
int _loutall_con(unsigned nbytes, char *ptr);
void _release_io_segment(void *p);
void _release_minidata(struct ALLOCS_STRUCT *p);
void _release_local_storage();
void _release_large_segment(void *p);
void _seg_released();
void _init_pool();
void _disable_mapout();
void _oom_error();
void *_get_large_segment(int EAX);
void _close_handle(void *);

void *memset(void *dst, int value, unsigned n);
void *memcpy(void *dst, const void *src, unsigned n);
void *memmove(void *dst, const void *src, unsigned n);

/*
fg_td   equ     00h
fg_implib       equ     00h
fg_cv   equ     0ffffh
fg_ov   equ     00h
fg_slrpack      equ     0ffffh
fg_ai   equ     00h
fgh_dpmi        equ     00h
fg_virt equ     00h
fg_plink        equ     00h
fg_rom  equ     00h
fg_def  equ     0ffffh
fg_cfg  equ     0ffffh
fg_rat  equ     00h
fg_segm equ     0ffffh
fg_mscmd        equ     0ffffh
fg_sym_virt     equ     00h
*/

#define ENTRIES_EQU     0x12
#define DOS_NEWSIZE_ERR 0x3f
#define DUP_GROUP_ERR   0x16

#define SEG_STACK_ERR   0x4a
#define DOS_ERR		0x1d
#define QFH_FAILED_ERR	0x3a
/*
DUP_OLD_ERR     equ     066h
DLL_NO_LIB_ERR  equ     05ch
DEF_STRING_ERR  equ     068h
INDEX_RANGE_ERR equ     010h
START_ERR       equ     035h
GRP_COD_DAT_ERR equ     073h
LOD_MISSING_ERR equ     0abh
DEF_EXPORTS_ERR equ     065h
ALIGN_SMALL_ERR equ     057h
DUP_HEAP_ERR    equ     061h
MUST_HAVE_SEGMENT_ERR   equ     019h

USE32_USE16_ERR equ     094h
EXP_TEXT_ERR    equ     06fh
BAD_RECORD_ERR  equ     0eh
LIBRARY_ERR     equ     06h
CVGTYPES_EQU    equ     01ah
NEAR_IMPORT_ERR equ     074h
BAD_PAGESIZE_ERR        equ     0b0h
CANNOT_PACK_ERR equ     044h
*/
#define LIB_PAGESIZE_ERR 0xb2
#define LOBYTE_ERR      0x26
#define CANNOT_LINK_ERR 0x7b
#define TOO_LONG_ERR    0x93
#define FIX_LIB_ERR     0x8c
#define DUP_MODEND_ERR  0x33
#define BAD_SEG_SIZE_ERR 0x15
#define DATAPACK_ERR    0x4e

#define BAD_CHKSUM_ERR  0x5
#define SEG_ALREADY_RELEASED_ERR        0x42
#define RES_CONV_ERR    0x97
#define MIC_NUM_ERR     0x2c
#define LIB_DUP_ERR     0x88
#define STACK_NZERO_ERR 0x96
#define INCERR_ERR      0x46
#define DISK_FULL_ERR   0xa
#define UNEXP_XEOF_ERR  0x41
/*
COM_PC_ERR      equ     02eh
LIN_NONCODE_ERR equ     04bh
SEGMENTS_EQU    equ     04h
LINSYM_BEFORE_COMDAT_ERR        equ     04ch
RTNLS_EQU       equ     017h
CV_TOO_MANY_ERR equ     02dh
CVP_BLOCK_WO_PARENT_ERR equ     09eh
MISSING_COLON_ERR       equ     0bh

ONE_DESC_ERR    equ     05dh
CVLTYPES_EQU    equ     019h
PUBSYMS_EQU     equ     03h
LIBRARY_EQU     equ     00h
CVP_IGNORE_ERR  equ     0a1h
CVP_SYMBOLS_64K_ERR     equ     0a6h
RELOCS_EQU      equ     0ch
DUP_RESOURCE_ERR        equ     06ah
EXPORT_CONFLICT_ERR     equ     056h
CVGSYMS_EQU     equ     01bh
RES_TYPENAMES_EQU       equ     016h
ALIAS_IGNORED_ERR       equ     07ah
CVSSYMS_EQU     equ     01ch
EXEHDR_ERR      equ     06dh
FILE_EXP_ERR    equ     076h
MODULES_EQU     equ     07h
IMPROBABLE_RES_ERR      equ     0b7h
MDBS_EQU        equ     0eh

CVP_CIRC_NO_CSUE_ERR    equ     0a3h
UNRECOGNIZED_FIXUPP_FRAME_ERR   equ     021h
EXESTR_ERR      equ     047h
DOS_WRITE_ERR   equ     03eh
CREATESEM_ERR   equ     038h
CANT_REACH_ERR  equ     023h
STD_MAXINDEX_ERR        equ     085h
LIDATA_ERR      equ     095h
FILES_EQU       equ     02h
PENTS_EQU       equ     011h
BASE_RELOC_ERR  equ     027h
CVP_UNSUPPORTED_TYPE_ERR        equ     09fh
DOS_READ_ERR    equ     03ch
CVHASHES_EQU    equ     01dh
CVP_SSEARCH_ERR equ     0a4h
BAD_CD_ALLOC_ERR        equ     080h

CMDLINE_ERR     equ     01ah
DOS_POS_ERR     equ     03dh
DUP_STUB_ERR    equ     067h
OOPS_ERR        equ     00h
OS2_FLAG_CONFLICT_ERR   equ     072h
HEAP_STACK_DGROUP_ERR   equ     058h
SYNTAX_ERR      equ     034h
IMPNAMES_EQU    equ     0fh
FORREF_ERR      equ     030h
RES3216_ERR     equ     0b3h
INDIRECT_DEEP_ERR       equ     078h
*/
#define DOSX_EXEPACK_ERR       0xad
/*
fg_cvpack       equ     0ffffh
DIR_TOO_BIG     equ     08fh
NOUNDECOMATCH_ERR       equ     0b4h
DEF_SEGMENTS_ERR        equ     060h
SEG_COMMON_ERR  equ     049h

OBJ_PHASE_ERR   equ     0dh
AREAS_EQU       equ     09h
*/
#define FATAL_RESTART_ERR	0x8b
/*
FIXUPP_OFFSET_ERR       equ     022h
GRP_TYP_ERR     equ     012h
CLOSE_ERR       equ     03bh
GRP_ERR equ     079h
ONE_NAME_ERR    equ     05ah
CVP_SCOPE_ERR   equ     09ch
SEGMODS_EQU     equ     05h
BAD_THREAD_ERR  equ     01bh
fg_xref equ     0ffffh
*/
#define DUP_LOD_ERR     0xaa
/*TOKEN_TOO_LONG_ERR      equ     075h
ENTNAMES_EQU    equ     010h
CVP_GTYPES_64K_ERR      equ     0a8h
PROT_REAL_ERR   equ     063h
MUL32_ERR       equ     01fh

CVP_LTYPES_64K_ERR      equ     0a7h
CIRCULAR_ALIAS_ERR      equ     048h
SHORT_ERR       equ     025h
DEF_WEAK_INT_ERR        equ     081h
REC_TOO_LONG_ERR        equ     04h
HEAP_NO_DGROUP_ERR      equ     089h
SEGM_COM_SYS_ERR        equ     05bh
START_CANT_REACH_ERR    equ     053h
*/
#define THREAD_FAIL_ERR 0x37
/*
PE_16_ERR       equ     0afh
WEAK_DEF_ERR    equ     077h
COMMUNAL_ERR    equ     082h
INVALID_OBJ_ERR equ     08ah
EXP_CONST_ERR   equ     051h
DUPDECOMATCH_ERR        equ     0b6h
RES_TOO_LONG_ERR        equ     092h
SEGCLASS_EQU    equ     01h

SRCS_EQU        equ     0dh
DEF_CODE_ERR    equ     05eh
RESNAMES_EQU    equ     014h
CSEGS_EQU       equ     0bh
RESTYPES_EQU    equ     015h
OUTFILES_EQU    equ     08h
SECTIONS_EQU    equ     0ah
CMDLIN_ERR      equ     09h
VERSION_BIG_ERR equ     098h
NO_START_ERR    equ     086h
GRP_OVERLAP_ERR equ     059h
TOO_ENTRIES_ERR equ     050h
DEF_EXETYPE_ERR equ     062h
CVP_PRECOMP_ERR equ     0a0h
DATA_OUTSIDE_SEGMOD_ERR equ     020h
TOO_MANY_BASES_ERR      equ     028h
NO_BASE_SEG_ERR equ     08dh
CEXTDEF_ERR     equ     083h

PREV_DEF_ERR    equ     01h
CODEPACK_ERR    equ     04dh
FILENAME_ERR    equ     08h
START_IMPORT_ERR        equ     054h
SUBSYSTEM_ERR   equ     070h
SECTION_ERR     equ     07dh
ID_EXP_ERR      equ     07ch
COMDAT_SYNTAX_ERR       equ     07eh
DEF_DATA_ERR    equ     05fh
MULTIPLE_RC_ERR equ     06bh
_TIMEOUT_ERR    equ     039h
PAGESIZE_ERR    equ     08eh
MULTIPLE_STACKS_ERR     equ     018h
DEF_IMPORTS_ERR equ     064h
SEGM_32_ERR     equ     0aeh
DUP_ENT_ORD_ERR equ     052h
RESOURCE_CORRUPT_ERR    equ     06eh
*/

#define DOSX_NONDOSX_ERR   0xac
#define NO_IMPORTS_ERR	   0x71

#define CANT_CREATE_ERR 3
/*
RELOC_CONFLICT_ERR      equ     043h
CONST_FRAME_ERR equ     084h
BAD_OMF_EXTENT_ERR      equ     0fh
CVP_STILL_FWDREF_ERR    equ     0a5h
IMPMODS_EQU     equ     013h
DOS_SETFILEINFO_ERR     equ     040h
CVP_NEST_SEG_ERR        equ     09dh
LOC_FRAME_ERR   equ     024h
CVP_BAD_LEAF_ERR        equ     09bh
GROUPS_EQU      equ     06h
ILL_NUMBER_ERR  equ     0ch
MISSING_STACK_ERR       equ     017h

UNREC_IN_CFG_ERR        equ     087h
UNDEFINED_ERR   equ     02ah
MODEL_CONFLICT_ERR      equ     0a9h
LIB1_ERR        equ     036h
BAD_LOC_ERR     equ     01ch
DUPUNDECOMATCH_ERR      equ     0b5h
*/

#define DUP_GROUP_ERR	0x16
#define RC_64K_ERR	0x6C
#define EXPORT_TOO_LONG_ERR	0x91
#define COM_BELOW_100_ERR	0x2F
#define TOO_MANY_SEGS_ERR	0x4F
#define CVP_CORRUPT_ERR	0x99
#define LIB_ERR	0x2B
#define UNREC_FIXUPP_ERR	0x29
#define SEG_TOO_BIG_ERR	0x13
#define PAGE_RELOCS_EQU	0x18
#define NOTHING_ERR	0x45
#define SEG_4G_ERR	0x14
#define IMPORT_TOO_LONG_ERR	0x90
#define GRP_TOO_BIG_ERR	0x11
#define CVP_SYMDEL_ERR	0x9A
#define APPLOAD_ERR	0x69

#define FILE_NOT_FOUND_ERR	2
#define OOM_ERR		7
#define DOS_ERR		0x1D
#define UNEXP_EOF_ERR	0x1E
#define CTRLC_ERR	0x31
#define CANT_REOPEN_ERR	0x32
#define CLOSE_ERR	0x3B
#define DOS_READ_ERR	0x3C
#define DOS_POS_ERR	0x3D
#define DOS_WRITE_ERR	0x3E
#define DOS_NEWSIZE_ERR	0x3F
#define BAD_STUB_ERR	0x55
#define COMDAT_CONT_ERR	0x7F
#define BAD_TYPEINDEX_ERR	0xA2
#define ILLEGAL_READ_ADDRESS_ERR	0xB1

extern int _ERR_COUNT;
extern unsigned char _CRITI_IN_PROGRESS;
extern unsigned char _DELETE_EXE_ON_ERROR;
extern unsigned char _NO_CANCEL_FLAG;
extern unsigned char _FINAL_ABORTING;
extern unsigned char _HOST_THREADED;
extern struct GLOBALSEM_STRUCT _ABORTER_SEM;
extern struct GLOBALSEM_STRUCT _SAY_VERBOSE_SEM;
extern struct GLOBALSEM_STRUCT _NONRES_LENGTH_SEM;
extern unsigned char _TIMEOUT_ERR;
extern unsigned char HANDLES_EXPANDED;

/****** moves/putdatac *********/

void _imp_out(void *EAX, unsigned ECX);
void _lst_out(void *EAX, unsigned ECX);

/****** alloc/dosioc **********/

void __exit(int exitcode);
void _delete_exe();
void _disable_mapout();

/*********** common/lnkinitc **********/

void _init_semaphores();
void _init_eax(struct GLOBALSEM_STRUCT *EAX);

/*********** common/exe *************/

extern void *EXE_OUT_TABLE;
extern void *SEGM_OUT_TABLE;
extern void *PE_OUT_TABLE;

/************ subs/array ************/

void _release_garray(void *);

