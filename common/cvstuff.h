
struct CVL_STRUCT
{
    unsigned _CVL_OFFSET;
    unsigned _CVL_LINNUM;
    unsigned _CVL_SRC_GINDEX;
    unsigned reserved;		// must be a power of 2 ?????
};

#define CVL_BITS	4


struct CVH_STRUCT
{
    unsigned _SYMBOL_OFFSET;
    unsigned _SYMBOL_HASH;
    unsigned _SEGMENT_OFFSET;
    unsigned _SEGMENT;
};


#define CVH_BITS	4


struct CV_HASHES_STRUCT
{
    unsigned _SYMBOL_OFFSET;
    unsigned _SYMBOL_HASH;
    unsigned _SEGMENT_OFFSET;
    unsigned _SEGMENT;
    unsigned _NEXT_HASH;
    struct CV_HASHES_STRUCT *_NEXT;
    struct CV_HASHES_STRUCT *_PREV;		// for 16-bit compatibility
};


struct CV_HASH_HDR_STRUCT
{
    unsigned short _CVHH_SYMHASH;
    unsigned short _CVHH_ADDRHASH;
    unsigned _CVHH_CBSYMBOL;
    unsigned _CVHH_CBSYMHASH;
    unsigned _CVHH_CBADDRHASH;
};


struct CV_NAMESP_STRUCT		// uses RELOC_STUFF and RELOC_GARRAY
{
    unsigned _CNS_NEXT_HASH_GINDEX;	// next same hash
    unsigned _CNS_NEXT_GINDEX;
    unsigned _CNS_OFFSET;		// offset in output namespace table
    unsigned _CNS_HASH;
    unsigned _CNS_TEXT;
};

