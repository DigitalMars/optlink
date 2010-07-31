
#include "macros.h"

#define OS2_SEGM_TYPE		1
#define WIN_SEGM_TYPE		2
#define DOS4_SEGM_TYPE		3
#define UNKNOWN_SEGM_TYPE	4
#define DOS_EXE_TYPE		5
#define PE_EXE_TYPE		6
#define DOSX_EXE_TYPE		7

#define MODEL_SMALL		1
#define MODEL_MEDIUM		2
#define MODEL_COMPACT		3
#define MODEL_LARGE		4
#define MODEL_HUGE		5
#define MODEL_DOSX		6
#define MODEL_FLATOS2		7
#define MODEL_FLATNT		8


typedef struct EXE
{
    unsigned short _EXE_SIGN;
    unsigned short _EXE_LEN_MOD_512;	//LENGTH OF EXE MOD 512
    unsigned short _EXE_LEN_PAGE_512;	//# OF 512 BYTE PAGES, INCLUDING HEADER
    unsigned short _EXE_N_RELOC;	//NUMBER OF RELOC ITEMS
    unsigned short _EXE_HDR_SIZE;	//# OF PARAGRAPHS IN HEADER
    unsigned short _EXE_MIN_ABOVE;	//MINIMUM ALLOCATION NEEDED ABOVE
    unsigned short _EXE_MAX_ABOVE;	//MAX ALLOCATION REQUEST
    unsigned short _EXE_REG_SS;		//STACK SEGMENT
    unsigned short _EXE_REG_SP;		//INITIAL SP VALUE
    unsigned short _EXE_CHKSUM;		//WORD CHECKSUM
    unsigned short _EXE_REG_IP;		//START ADDRESS OFFSET
    unsigned short _EXE_REG_CS;		//START ADDRESS SEGMENT
    unsigned short _EXE_RELOC_OFF;	//OFFSET TO RELOC ITEMS
    unsigned short _EXE_OVLY_NUM;	//OVERLAY NUMBER
#if any_overlays
    unsigned _EXE_VECSEC_ADDR;
    unsigned _EXE_FIRST_RELOC;
    unsigned char _EXE_FILLER[0x40 - (14 * 2 + 2 * 4)];
#else
    unsigned short _EXE_xx;		//SOMETHING FOR MS...
    unsigned _EXE_FIRST_RELOC;
    unsigned char _EXE_FILLER[0x40 - (14 * 2 + 2 + 4)];
#endif
} EXE;

typedef struct NEXE
{
    unsigned short _NEXE_SIGN;		//'NE'					00
    unsigned short _NEXE_LVERNUM;	//LINKER VERSION NUMBER			02
    unsigned short _NEXE_ENTRY_OFFSET;	//OFFSET TO ENTRY TABLE			04
    unsigned short _NEXE_ENTRY_LENGTH;	//BYTE-LENGTH OF ENTRY TABLE		06
    unsigned _NEXE_CHECKSUM;		//FILE CHECKSUM				08
    unsigned short _NEXE_FLAGS;		//					0C
    unsigned short _NEXE_DGROUP;	//SEG NUMBER OF DGROUP			0E
    unsigned short _NEXE_HEAPSIZE;	//DYNAMIC HEAP				10
    unsigned short _NEXE_STACKSIZE;	//					12
    unsigned _NEXE_CSIP;		//START ADDRESS				14
    unsigned _NEXE_SSSP;		//STACK					18
    unsigned short _NEXE_NSEGS;		//# OF SEGMENTS				1C
    unsigned short _NEXE_NMODS;		//# OF DLL'S REFERENCED			1E
    unsigned short _NEXE_NONRES_LENGTH;	//# OF BYTES IN NONRESIDENT NAME TABLE	20
    unsigned short _NEXE_SEGTBL_OFFSET;	//OFFSET TO SEGMENT TABLE		22
    unsigned short _NEXE_RSRCTBL_OFFSET; //OFFSET TO RESOURCE TABLE		24
    unsigned short _NEXE_RESNAM_OFFSET;	//OFFSET TO RESIDENT NAMES TABLE		26
    unsigned short _NEXE_MODREF_OFFSET;	//OFFSET TO DLL REFERENCE TABLE		28
    unsigned short _NEXE_IMPNAM_OFFSET;	//					2A
    unsigned _NEXE_NRESNAM_OFFSET;	//OFFSET (IN FILE) TO NONRESIDENT NAMES	2C
    unsigned short _NEXE_NMOVABLE_ENTS;	//# OF MOVABLE ENTRY POINTS		30
    unsigned short _NEXE_LSECTOR_SHIFT;	//					32
    unsigned short _NEXE_CRES;		//					34
    unsigned char _NEXE_EXETYPE;	//1 = OS/2, 2 = WINDOWS, 3 = DOS4	36
    unsigned char _NEXE_LONGNAMES_FLAG;	//1 = LONGNAMES		8 = gangload	37
    unsigned short _NEXE_GANGSTART;	//					38
    unsigned short _NEXE_GANGLENGTH;	//					3A
    unsigned short _NEXE_SWAPAREA;	//					3C
    unsigned char _NEXE_WINVER_FRAC;	//SUB-VERSION IN HUNDREDTHS (0AH == .1)	3E
    unsigned char _NEXE_WINVER_INT;	//REQUIRED WINDOWS VERSION		3F
} NEXE;

typedef struct PEXE
{
    unsigned short _PEXE_SIGN;		//SHOULD BE 'PE/0/0'
    unsigned short _PEXE_1;			//
    unsigned short _PEXE_CPU_REQUIRED;	//CPU COMPATIBILITY REQUIRED TO RUN
    unsigned short _PEXE_N_OBJECTS;	//# OF ENTRIES IN OBJECTS TABLE

    unsigned _PEXE_TIME_DATE;		//LINK TIME AND DATE
    unsigned _PEXE_RES1;		//PTR TO SYMBOL TABLE

    unsigned _PEXE_RES2;		//# OF SYMBOLS
    unsigned short _PEXE_HDR_XTRA;	//# OF BYTES FOLLOWING FLAGS FIELD IN HEADER
    unsigned short _PEXE_FLAGS;		//PROGRAM, LIBRARY, LOADABLE, FIXED, ETC

    unsigned short _PEXE_MAJIC;		//MAGIC NUMBER
    unsigned char _PEXE_LMAJOR;		//LINKER MAJOR VERSION #
    unsigned char _PEXE_LMINOR;		//LINKER MINOR VERSION #
    unsigned _PEXE_CODE_SIZE;		//SIZE OF FIRST CODE SECTION

    unsigned _PEXE_IDATA_SIZE;		//INITIALIZED DATA SIZE
    unsigned _PEXE_UDATA_SIZE;		//UNINITIALIZED DATA SIZE

    unsigned _PEXE_ENTRY_RVA;		//START ADDRESS
    unsigned _PEXE_CODE_BASE;		//ADDRESS OF BEGINNING OF CODE SECTION IN MEMORY

    unsigned _PEXE_DATA_BASE;		//ADDRESS OF DATA SECTION IN MEMORY

    unsigned _PEXE_IMAGE_BASE;		//VIRTUAL ADDRESS OF FIRST BYTE OF FILE, 64K MULTIPLE

    unsigned _PEXE_OBJECT_ALIGN;	//POWER OF 2 BETWEEN 512 AND 256M.
    unsigned _PEXE_FILE_ALIGN;		//POWER OF 2 BETWEEN 512 AND 64K.

    unsigned short _PEXE_OS_MAJOR /*= 1*/;	//
    unsigned short _PEXE_OS_MINOR /*= 0*/;	//
    unsigned short _PEXE_USER_MAJOR;	//
    unsigned short _PEXE_USER_MINOR;	//

    unsigned short _PEXE_SUBSYS_MAJOR;	//
    unsigned short _PEXE_SUBSYS_MINOR;	//
    unsigned _PEXE_RES9;		//

    unsigned _PEXE_IMAGE_SIZE;		//INCLUDING HEADERS, MULTIPLE OF OBJECT_ALIGN
    unsigned _PEXE_HEADER_SIZE;		//DOS HEADER, PE HEADER, OBJECT TABLE

    unsigned _PEXE_CHECKSUM;		//
    unsigned short _PEXE_SUBSYSTEM;	//NT, OS2, POSIX, ETC
    unsigned short _PEXE_DLL_FLAGS;	//INIT, TERM FLAGS

    unsigned _PEXE_STACK_RESERVE;	//
    unsigned _PEXE_STACK_COMMIT;	//

    unsigned _PEXE_HEAP_RESERVE;	//
    unsigned _PEXE_HEAP_COMMIT;		//

    unsigned _PEXE_LOADER_FLAGS;	//
    unsigned _PEXE_N_INTERESTING /*= 16*/; //# OF VA/SIZE ENTRIES THAT FOLLOW

    unsigned _PEXE_EXPORT_RVA;		//
    unsigned _PEXE_EXPORT_SIZE;		//

    unsigned _PEXE_IMPORT_RVA;		//
    unsigned _PEXE_IMPORT_SIZE;		//

    unsigned _PEXE_RESOURCE_RVA;	//
    unsigned _PEXE_RESOURCE_SIZE;	//

    unsigned _PEXE_EXCEPTION_RVA;	//
    unsigned _PEXE_EXCEPTION_SIZE;	//

    unsigned _PEXE_SECURITY_RVA;	//
    unsigned _PEXE_SECURITY_SIZE;	//

    unsigned _PEXE_FIXUP_RVA;		//
    unsigned _PEXE_FIXUP_SIZE;		//

    unsigned _PEXE_DEBUG_RVA;		//
    unsigned _PEXE_DEBUG_SIZE;		//

    unsigned _PEXE_DESCRIPTION_RVA;	//
    unsigned _PEXE_DESCRIPTION_SIZE;	//

    unsigned _PEXE_MACHINE_RVA;		//
    unsigned _PEXE_MACHINE_SIZE;	//

    unsigned _PEXE_THREAD_LOCAL_RVA;	//
    unsigned _PEXE_THREAD_LOCAL_SIZE;	//

    unsigned _PEXE_LOAD_CONFIG_RVA;	//
    unsigned _PEXE_LOAD_CONFIG_SIZE;	//

    unsigned _PEXE_2;	//
    unsigned _PEXE_3;	//

    unsigned _PEXE_4;	//
    unsigned _PEXE_5;	//

    unsigned _PEXE_6;	//
    unsigned _PEXE_7;	//

    unsigned _PEXE_8;	//
    unsigned _PEXE_9;	//

    unsigned _PEXE_10;	//
    unsigned _PEXE_11;	//
} PEXE;


#define PEXE_UNKNOWN		0
#define PEXE_80386		0x14C
#define PEXE_80486		0x14D
#define PEXE_PENTIUM		0x14E
#define PEXE_MARK1		0x162
#define PEXE_MARK2		0x163
#define PEXE_MARK3		0x166	// R4000 MIPS LITTLE-ENDIAN
#define PEXE_ALPHA		0x184	// ALPHA AXP

#define PEXE_PROGRAM		0
#define PEXE_EXECUTABLE		2
#define PEXE_FIXED		0x200
#define PEXE_LIBRARY		0x2000

#define PSUB_UNKNOWN		0
#define PSUB_NATIVE		1
#define PSUB_WIN_GUI		2
#define PSUB_WIN_CHAR		3
#define PSUB_OS2		5
#define PSUB_POSIX		7

#define PDLL_PPROC_INIT		1
#define PDLL_PPROC_TERM		2
#define PDLL_PTHRD_INIT		4
#define PDLL_PTHRD_TERM		8

#define PE_EXE_BASE		0x400000
#define PE_DLL_BASE		0x10000000

