
#ifndef MACROS_H
#define MACROS_H 1

#define debug	0
#define V5	-1
#define V6	-1

#define fg_cfg		-1
#define fg_pe		-1
#define fg_dosx		-1
#define fg_cvpack	V6

#define _PRODUCT	12

#define fg_xref		-1
#define fg_cv		-1
#define fg_segm		-1
#define fg_ai		0
#define fg_rat		0
#define fg_ov		0		// SINGLE-LEVEL OVERLAYS
#define fg_rom		0		// ROM-LINK
#define fg_def		-1		// HAS A DEF FILE
#define fg_slrpack	-1
#define fg_plink	0
#define fg_mscmd	-1
#define fg_implib	0
#define fg_phar		-1
#define fg_winpack	-1
#define fg_dospack	0

#define fgh_prot	-1
#define fgh_dpmi	0
#define fgh_win		0
#define fgh_win32	-1
#define fgh_win32dll	0
#define fgh_dosx	0
#define fgh_inthreads	-1
#define fgh_outhreads	0		// OUTPUT THREADS
#define fgh_mapthread	-1
#define fg_virt		0
#define fg_sym_virt	0
#define page_size	16K
#define PAGE_SIZE	(16*1024)
#define PAGE_SHIFT	2
#define PAGE_BITS	14
#define fg_td		0

#define fgh_anythreads	(fgh_inthreads || fgh_outhreads || fgh_mapthread)

#define INBUF_LEN		1024
#define SYMBOL_TEXT_SIZE	(8 * 1024)
#define NFN_TEXT_SIZE		320
#define LONG_OMF_DELTA		3

#define APPTYPE		0x8000	// 0=PROGRAM, 1=DLL
//#define APP??		0x4000	// UNKNOWN
#define APPERRORS	0x2000	// ERRORS DURING LINK
//#define APP???		0x1C00	// UNKNOWN
//#define APPWINFLAGS	0x300 // WINDOWAPI, WINDOWCOMPAT, NOTWINDOWCOMPAT
#define APPFLOATS	0x80	// FLOATS USED?
#define APP386		0x40	// 386 INSTRUCTIONS
#define APP286		0x20	// 286 INSTRUCTIONS
#define APP86		0x10	// 8086-ONLY
#define APPPROT		8	// PROTMODE APP
#define APPINSTANCE	4 	// INSTANCE DATA
#define APPMULTI	2	// MULTIPLE DATA (NONSHARED)
#define APPSOLO		1	// SOLO DATA (SHARED)

#define CASE_SENSITIVE	0
#define MAX_RECORD_LEN	(1024+64+6*1024)	// FOR BORLAND
#define SLRIB		0		// NOT OPTLIB
#define OPTLIB		0

#define MAX_LEDATA_LEN	(2*1024)

#define BLOCK_BASE	8		// STARTING OFFSET FOR FAR BLOCKS (FIRST DWORD IS USAGE COUNT, NEXT IS NEXT_PTR)
					// **** FARCALLTRANSLATE MAY NOT HANDLE CHANGE CORECTLY ****

#define EMS_XMS_DELTA	(1024/(PAGE_SIZE/(1024))*2)

#define N_R_THREADS	4	// # of open-read threads
#define N_W_THREADS	4

typedef struct GLOBALSEM_STRUCT
{
    void *_SEM_ITSELF;
    int _SEM_COUNTER;
} GLOBALSEM_STRUCT;

typedef struct ALLOCS_STRUCT
{
    void **ALLO_LAST_LIST;		// LAST STORED LIST OF BLOCKS
    void *ALLO_PTR;		// LOGICAL PTR TO NEXT AVAILABLE BYTE
    int ALLO_CNT;		// BYTES LEFT THIS BLOCK
    int ALLO_SUB_CNT;
    void *ALLO_SUB_PTR;
    void **ALLO_NEXT_BLK;	// PTR TO STORE NEXT BLOCK LOCALLY
    int ALLO_HASH_TABLE_PTR;	// HASH TABLE IF IT EXISTS
    int ALLO_HASH;		// HASH IF IT EXISTS
    int ALLO_BLK_CNT;		// ALLOCATED BLOCK COUNT
    void *ALLO_BLK_LIST[8];	// LIST OF UP TO 8 ALLOCATED BLOCKS
} ALLOCS_STRUCT;

typedef struct SEQ_STRUCT
{

    unsigned _SEQ_PTR;		// NEXT ADDRESS TO WRITE TO
    unsigned _SEQ_TABLE[(16*32*1024)/PAGE_SIZE*4]; // 256K MAX DATA THIS TYPE
    unsigned _SEQ_TARGET;		// PTR TO NEXEHEADER OFFSET OF OFFSET...
    unsigned _SEQ_NEXT_TARGET;
} SEQ_STRUCT;


typedef struct DTA_STRUCT
{
    unsigned short FDATECREATION;
    unsigned short FTIMECREATION;
    unsigned short FDATELASTACCESS;
    unsigned short FTIMELASTACCESS;
    unsigned short FDATELASTWRITE;
    unsigned short FTIMELASTWRITE;
    unsigned CBFILE;
    unsigned CBFILEALLOC;
    unsigned short ATTRFILE;
    unsigned char CCHNAME;
    unsigned char ACHNAME[13];
} DTA_STRUCT;

typedef struct CMDLINE_STRUCT
{
    int (*CMD_SELECTED)();	// USE SRCNAM (NZ) OR NUL
    unsigned char *CMD_EXTENT;	// DEFAULT EXTENT
#if fg_mscmd
    int (*CMD_DESIRED)();	// WANT AT ALL?  LIKE OUTPUT LIBRARY NAME
    unsigned char *CMD_PMSG;	// PROMPT MESSAGE
#endif
} CMDLINE_STRUCT;


typedef struct STD_PTR_S	// USED BY LIBRARIES, SOON MASTER GROUPS, SEGMENTS, CLASSES, POSSIBLY EVEN SYMBOLS, OTHERS
{
    void *_STD_BASE_NEXT;	// NEXT PLACE TO STORE BLOCK POINTER
    void *_STD_NEXT;		// NEXT PLACE TO STORE PTR
    int _STD_LIMIT;		// # OF VALID ENTRIES IN TABLE
    int _STD_COUNT;		// # LEFT TO ALLOCATE THIS BLOCK
    void (*_STD_ALLOC_ROUTINE)();	// ALLOCATE MEMORY FOR PTRS
    int _STD_TYPE_TEXT;
    void (*_STD_INSTALL_ROUTINE)();	// INSTALL NEXT ENTRY INTO TABLE
    void (*_STD_INSTALL_RANDOM_ROUTINE)();
    void (*_STD_AX_TO_DSSI)();		// CONVERT AX TO PTR IN DS:SI PHYSICAL
    void (*_STD_AX_TO_ESDI)();		// CONVERT AX TO PTR IN ES:DI PHYSICAL
    int _STD_EAX_TO_EBX;
    void *_STD_PTRS[16];	// INITIALLY PTRS, THEN PTRS TO BLOCKS, THEN PTR TO MASTER BLOCK
} STD_PTR_S;

typedef struct FA_BASE_S
{
    unsigned usage_count;
    struct FA_BASE_S *next_ptr;
} FA_BASE_S;

typedef struct FA_S
{
    void *FA_PTR;
    unsigned FA_CNT;
    FA_BASE_S *FA_BASE;
} FA_S;

#define GMEM_FIXED		0x0000
#define GMEM_MOVEABLE		0x0002
#define GMEM_ZEROINIT		0x0040
#define GMEM_NOCOMPACT		0x0010
#define GMEM_NODISCARD		0x0020
#define GMEM_MODIFY		0x0080
#define GMEM_DISCARDABLE 	0x0100
#define GMEM_NOT_BANKED		0x1000
#define GMEM_SHARE		0x2000
#define GMEM_DDESHARE		0x2000
#define GMEM_NOTIFY		0x4000
#define GMEM_LOWER		GMEM_NOT_BANKED


#define INBUF_LEN			1024
#define SYMBOL_TEXT_SIZE		(8 * 1024)
#define NFN_TEXT_SIZE			320
#define LONG_OMF_DELTA			3

#define INDIRECT_NEST_LIMIT		10

#define NEW_FIXUPP_SIZE			1024

#define CV_TEMP_SIZE			(9 * 1024)
#define TEMP_SIZE			1024

#define FILENAME_HASH_SIZE		0xFB
#define THEADR_HASH_SIZE		61
#define VIRDEF_MCD_HASH_SIZE		0xFB

#define CASE_SENSITIVE		0
#define MAX_RECORD_LEN		(1024+64+6*1024)	// FOR BORLAND
#define SLRIB			0		// NOT OPTLIB
#define OPTLIB			0

#define MAX_LEDATA_LEN		(2 * 1024)

#define BLOCK_BASE		sizeof(FA_BASE_S)	// STARTING OFFSET FOR FAR BLOCKS (FIRST DWORD IS USAGE COUNT, NEXT IS NEXT_PTR)
					// **** FARCALLTRANSLATE MAY NOT HANDLE CHANGE CORECTLY ****

#define EMS_XMS_DELTA		(1024/(PAGE_SIZE/(1024))*2)

#define N_R_THREADS		4		// # OF OPEN-READ THREADS
#define N_W_THREADS		4

;#define PHYS_TABLE_SIZE 	4		// # OF 16K BLOCKS RELEASED LOCALLY OS/2

;#define MAX_VECTORS		((32*1024)/VECTOR_SIZE*2)

#define M1		-1

typedef struct FARCALL_HEADER_TYPE
{
    unsigned _FC_BLOCK_BASE;
    unsigned _FC_NEXT_FARCALL;
    unsigned _FC_LENGTH;
    unsigned _FC_SEGMOD_GINDEX;
} FARCALL_HEADER_TYPE;

typedef struct TPTR_STRUCT
{
    unsigned _TP_FLAGS;
    unsigned _TP_HASH;
    unsigned _TP_LENGTH;
    unsigned char _TP_TEXT[SYMBOL_TEXT_SIZE];
} TPTR_STRUCT;

#endif
