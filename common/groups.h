
typedef struct GROUP_STRUCT
{
    unsigned _G_NEXT_HASH_GINDEX;
    unsigned _G_NEXT_GROUP_GINDEX;	// NEXT GROUP IN SOME LIST ORDER

    unsigned _G_FIRST_SEG_GINDEX;
    unsigned _G_LAST_SEG_GINDEX;
    unsigned _G_FRAME;			// OFFSET AND 0FFF0H IF REAL MODE, ELSE SEGMENT # & FLAGS
    unsigned _G_OFFSET;			// OFFSET FROM FRAME (0) - OR REAL ADDRESS FOR REAL MODE
    unsigned _G_LEN;			// LENGTH... AFTER ALIGN

    unsigned _G_NUMBER;			// GROUP NUMBER...  FOR INTERNAL FIXUPPS AND CV NUMBERING
    unsigned _G_NEXT_CV_GINDEX;

#if	fg_prot
    #define _G_OS2_NUMBER	_G_FRAME
    unsigned _G_OS2_FLAGS;
#endif

#if	fg_plink
    unsigned _G_SECTION_INDEX;
    unsigned char _G_PLTYPE;		// (PLTYPES RECORD)
    unsigned char reserved1;
#else
    unsigned short reserved2;
#endif

    unsigned char _G_TYPE;		// UNDEF, ASEG, RELOC, ETC
    unsigned char _G_TYPE1;		// CODE OR DATA	(GREC) 16/32, ETC

    unsigned _G_HASH;
    unsigned _G_TEXT;

} GROUP_STRUCT;	// ***** MUST BE EVEN WORDS *****

#define	GROUP_IS_USE16		0x20
#define	GROUP_IS_USE32		0x10
#define	GROUP_IS_FLAT		8
#define	GROUP_IS_DGROUP		4
#define	GROUP_IS_CODE		2
#define	GROUP_IS_DATA		1
