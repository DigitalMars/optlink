
#define S_UNDEFINED		0
#define S_COMPILE		1
#define S_REGISTER		2
#define S_CONSTANT		3
#define S_UDT			4
#define S_SSEARCH		5
#define S_END			6
#define S_SKIP			7
#define S_DELETE		8
#define S_OBJNAME		9
#define S_ENDARG		0xA
#define S_COBOLUDT		0xB
#define S_MANYREG		0xC
#define S_RETURN		0xD
#define S_ENTRYTHIS		0xE
#define S_TDBNAME		0xF

#define S_BPREL16		0x100
#define S_LDATA16		0x101
#define S_GDATA16		0x102
#define S_PUB16			0x103
#define S_LPROC16		0x104
#define S_GPROC16		0x105
#define S_THUNK16		0x106
#define S_BLOCK16		0x107
#define S_WITH16		0x108
#define S_LABEL16		0x109
#define S_CEXMODEL16		0x10A
#define S_VFPATH16		0x10B
#define S_REGREL16		0x10C

#define S_BPREL32		0x200
#define S_LDATA32		0x201
#define S_GDATA32		0x202
#define S_PUB32			0x203
#define S_LPROC32		0x204
#define S_GPROC32		0x205
#define S_THUNK32		0x206
#define S_BLOCK32		0x207
#define S_WITH32		0x208
#define S_LABEL32		0x209
#define S_CEXMODEL32		0x20A
#define S_VFPATH32		0x20B
#define S_REGREL32		0x20C
#define S_LTHREAD32		0x20D
#define S_GTHREAD32		0x20E

#define S_LPROCMIPS		0x300
#define S_GPROCMIPS		0x301

#define S_PROCREF		0x400
#define S_DATAREF		0x401
#define S_ALIGN			0x402

enum
{
	I_S_UNDEFINED,
	I_S_COMPILE,
	I_S_REGISTER,
	I_S_CONSTANT,
	I_S_UDT,
	I_S_SSEARCH,
	I_S_END,
	I_S_SKIP,
	I_S_DELETE,
	I_S_OBJNAME,
	I_S_ENDARG,
	I_S_COBOLUDT,
	I_S_MANYREG,
	I_S_RETURN,
	I_S_ENTRYTHIS,
	I_S_TDBNAME,

	I_S_BPREL16,
	I_S_LDATA16,
	I_S_GDATA16,
	I_S_PUB16,
	I_S_LPROC16,
	I_S_GPROC16,
	I_S_THUNK16,
	I_S_BLOCK16,
	I_S_WITH16,
	I_S_LABEL16,
	I_S_CEXMODEL16,
	I_S_VFPATH16,
	I_S_REGREL16,

	I_S_BPREL32,
	I_S_LDATA32,
	I_S_GDATA32,
	I_S_PUB32,
	I_S_LPROC32,
	I_S_GPROC32,
	I_S_THUNK32,
	I_S_BLOCK32,
	I_S_WITH32,
	I_S_LABEL32,
	I_S_CEXMODEL32,
	I_S_VFPATH32,
	I_S_REGREL32,
	I_S_LTHREAD32,
	I_S_GTHREAD32,

	I_S_LPROCMIPS,
	I_S_GPROCMIPS,

	I_S_PROCREF,
	I_S_DATAREF,
	I_S_ALIGN,
};

#define LF_UNDEFINED		0
#define LF_MODIFIER		1
#define LF_POINTER		2
#define LF_ARRAY		3
#define LF_CLASS		4
#define LF_STRUCTURE		5
#define LF_UNION		6
#define LF_ENUM			7
#define LF_PROCEDURE		8
#define LF_MFUNCTION		9
#define LF_VTSHAPE		0x0A
#define LF_COBOL0		0x0B
#define LF_COBOL1		0x0C
#define LF_BARRAY		0x0D
#define LF_LABEL		0x0E
#define LF_NULL			0x0F
#define LF_NOTTRAN		0x10
#define LF_DIMARRAY		0x11
#define LF_VFTPATH		0x12
#define LF_PRECOMP		0x13
#define LF_ENDPRECOMP		0x14
#define LF_OEM			0x15
#define LF_RESERVED		0x16

#define LF_SKIP			0x200
#define LF_ARGLIST		0x201
#define LF_DEFARG		0x202
#define LF_LIST			0x203
#define LF_FIELDLIST		0x204
#define LF_DERIVED		0x205
#define LF_BITFIELD		0x206
#define LF_METHODLIST		0x207
#define LF_DIMCONU		0x208
#define LF_DIMCONLU		0x209
#define LF_DIMVARU		0x20A
#define LF_DIMVARLU		0x20B
#define LF_REFSYM		0x20C

#define LF_BCLASS		0x400
#define LF_VBCLASS		0x401
#define LF_IVBCLASS		0x402
#define LF_ENUMERATE		0x403
#define LF_FRIENDFCN		0x404
#define LF_INDEX		0x405
#define LF_MEMBER		0x406
#define LF_STMEMBER		0x407
#define LF_METHOD		0x408
#define LF_NESTTYPE		0x409
#define LF_VFUNCTAB		0x40A
#define LF_FRIENDCLS		0x40B
#define LF_ONEMETHOD		0x40C
#define LF_VFUNCOFF		0x40D

enum
{
	I_LF_UNDEFINED,
	I_LF_MODIFIER,
	I_LF_POINTER,
	I_LF_ARRAY,
	I_LF_CLASS,
	I_LF_STRUCTURE,
	I_LF_UNION,
	I_LF_ENUM,
	I_LF_PROCEDURE,
	I_LF_MFUNCTION,
	I_LF_VTSHAPE,
	I_LF_COBOL0,
	I_LF_COBOL1,
	I_LF_BARRAY,
	I_LF_LABEL,
	I_LF_NULL,
	I_LF_NOTTRAN,
	I_LF_DIMARRAY,
	I_LF_VFTPATH,
	I_LF_PRECOMP,
	I_LF_ENDPRECOMP,
	I_LF_OEM,
	I_LF_RESERVED,
	I_LF_SKIP,
	I_LF_ARGLIST,
	I_LF_DEFARG,
	I_LF_LIST,
	I_LF_FIELDLIST,
	I_LF_DERIVED,
	I_LF_BITFIELD,
	I_LF_METHODLIST,
	I_LF_DIMCONU,
	I_LF_DIMCONLU,
	I_LF_DIMVARU,
	I_LF_DIMVARLU,
	I_LF_REFSYM,
	I_LF_BCLASS,
	I_LF_VBCLASS,
	I_LF_IVBCLASS,
	I_LF_ENUMERATE,
	I_LF_FRIENDFCN,
	I_LF_INDEX,
	I_LF_MEMBER,
	I_LF_STMEMBER,
	I_LF_METHOD,
	I_LF_NESTTYPE,
	I_LF_VFUNCTAB,
	I_LF_FRIENDCLS,
	I_LF_ONEMETHOD,
	I_LF_VFUNCOFF,
};

#define CV_MINI_CIRCS	4		// MUST BE AT LEAST 4

/*
typedef struct CV_LTYPE_STRUCT	// LOCAL TYPE DEFINITION STRUCTURE
{
_CV_LTYPE_GTYPE		DD	?	// GLOBAL TYPE
_CV_LTYPE_NAMEOFF	DB	?	// OFFSET TO NAME TEXT FOR CLASS		// **
_CV_LTYPE_FLAGS		DB	?	// LOCAL TYPE FLAGS			// **
			DW	?
_CV_LTYPE_NEST_LEVEL	DD	?	// SOMETIMES COMPARE-TYPE-BASE
_CV_LTYPE_CIRC_CNT	DD	?	// # OF CIRCULAR REFERENCES
_CV_LTYPE_CIRC1		DD	?	// OFFSET TO FIRST CIRCULAR REFERENCE
			DD	CV_MINI_CIRCS-1 DUP(?);

_CV_LTYPE_HASH		DD	?	;
			;
			// UP TO HERE MUST BE DWORD ALIGNED
			;
_CV_LTYPE_LENGTH	DW	?	// TYPE LENGTH
_CV_LTYPE_ILEAF		DW	?	// INTERNAL LEAF NUMBER
_CV_LTYPE_WORD1		DW	?
_CV_LTYPE_WORD2		DW	?
_CV_LTYPE_WORD3		DW	?
_CV_LTYPE_WORD4		DW	?
_CV_LTYPE_WORD5		DW	?
_CV_LTYPE_WORD6		DW	?

} CV_LTYPE_STRUCT;

_CV_LTYPE_CIRC_PTR	EQU	<_CV_LTYPE_CIRC1>
_CV_LTYPE_MAX_CNT	EQU	<_CV_LTYPE_CIRC1+4>
_CV_LTYPE_COMP_BASE	EQU	<_CV_LTYPE_NEST_LEVEL>

_CV_LTYPE_INDEX_TARGET	EQU	<_CV_LTYPE_NEST_LEVEL>
_CV_LTYPE_NEXT_LFINDEX	EQU	<_CV_LTYPE_CIRC_CNT>


CV_LTYPE_FLAGS		RECORD	CV_LTYPE_INSTALLED:1, \		// THIS MEANS TYPE HAS BEEN INSTALLED IN GLOBAL TABLE, TID-SIZE==INDEX
				CV_LTYPE_COMPARING:1, \		// WE ARE CURRENTLY COMPARING THIS GUY
				CV_LTYPE_DEFINING:1, \		// DEFINING MY TYPES
				CV_LTYPE_CSUE:1, \		// CLASS, STRUCTURE, UNION, OR ENUM
				CV_LTYPE_FWDREF:1, \		// CSUE IS FORWARD REFERENCED
				CV_LTYPE_DEFINING2:1		// DEFINING, OTHER PASS

typedef struct CV_GTYPE_STRUCT		// LOCAL TYPE DEFINITION STRUCTURE
{
_CV_GTYPE_NEXT_HASH	DD	?	// NEXT SAME HASH

_CV_GTYPE_NAMEOFF	DB	?	// HIGH BYTE OF GLOBAL TYPE
_CV_GTYPE_FLAGS		DB	?	// LOCAL TYPE FLAGS
			DW	?

_CV_GTYPE_HASH		DD	?	// 
			// 
			// UP TO HERE MUST BE DWORD ALIGNED
			// 
_CV_GTYPE_LENGTH	DW	?	// TYPE LENGTH
_CV_GTYPE_ILEAF		DW	?	// INTERNAL LEAF NUMBER
_CV_GTYPE_WORD1		DW	?
_CV_GTYPE_WORD2		DW	?
_CV_GTYPE_WORD3		DW	?
_CV_GTYPE_WORD4		DW	?
_CV_GTYPE_WORD5		DW	?
_CV_GTYPE_WORD6		DW	?

} CV_GTYPE_STRUCT;


typedef struct CV_LFWD_STRUCT
{
_CV_LFWD_NEXT_HASH	DD	?
_CV_LFWD_GINDEX		DD	?
_CV_LFWD_HASH		DD	?
} CV_LFWD_STRUCT;
*/

typedef struct CV_GLOBALSYM_STRUCT
{
    unsigned _HASH;
    void *_NEXT_HASH_GINDEX;
    void *_NEXT_GSYM_GINDEX;
    unsigned _TEXT_OFFSET;
    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
} CV_GLOBALSYM_STRUCT;


typedef struct CV_SYMBOL_STRUCT
{
    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
} CV_SYMBOL_STRUCT;

/*
// _CVGS_ID		EQU	SIZE CV_GLOBALSYM_STRUCT._CVS_ID
// _CVGS_LENGTH		EQU	SIZE CV_GLOBALSYM_STRUCT._CVS_LENGTH

CV_REG_STRUCT		STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_TYPE			DW	?
_REG			DW	?
_NAME			DB	?

CV_REG_STRUCT		ENDS


CV_CONST_STRUCT		STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_TYPE			DW	?
_VALUE			DW	?

CV_CONST_STRUCT		ENDS


CV_UDT_STRUCT		STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_TYPE			DW	?
_NAME			DB	?

CV_UDT_STRUCT		ENDS


CV_SEARCH_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_OFFSET			DD	?	// OFFSET OF SYMBOL
_SEGMENT		DW	?	// LOGICAL SEGMENT
_FILLER			DW	?	// FOR DWORD ALIGN

CV_SEARCH_STRUCT		ENDS


CV_OBJ_STRUCT		STRUC	// 09

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_SIGNATURE		DD	?
_NAME			DB	?

CV_OBJ_STRUCT		ENDS


CV_LDATA16_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_OFFSET			DW	?
_SEGMENT		DW	?
_TYPE			DW	?
_NAME			DB	?

CV_LDATA16_STRUCT	ENDS


CV_GDATA16_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_OFFSET			DW	?
_SEGMENT		DW	?
_TYPE			DW	?
_NAME			DB	?

CV_GDATA16_STRUCT	ENDS


CV_LPROC16_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_PPARENT	DD	?
_PEND		DD	?
_PNEXT		DD	?

_PLENGTH	DW	?
_DBG_START	DW	?

_DBG_END	DW	?
_OFFSET		DW	?

_SEGMENT	DW	?
_TYPE		DW	?

_FLAGS		DB	?
_NAME		DB	?

CV_LPROC16_STRUCT	ENDS


CV_GPROC16_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_PPARENT	DD	?
_PEND		DD	?
_PNEXT		DD	?
_PLENGTH	DW	?
_DBG_START	DW	?
_DBG_END	DW	?
_OFFSET		DW	?
_SEGMENT	DW	?
_TYPE		DW	?
_FLAGS		DB	?
_NAME		DB	?

CV_GPROC16_STRUCT	ENDS


CV_THUNK16_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_PPARENT	DD	?
_PEND		DD	?
_PNEXT		DD	?

_OFFSET		DW	?
_SEGMENT	DW	?

_PLENGTH	DW	?
_ORDINAL	DB	?
_NAME		DB	?

CV_THUNK16_STRUCT	ENDS


CV_BLOCK16_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_PPARENT	DD	?
_PEND		DD	?

_PLENGTH	DW	?
_OFFSET		DW	?

_SEGMENT	DW	?
_NAME		DB	?

CV_BLOCK16_STRUCT	ENDS


CV_LABEL16_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_OFFSET		DW	?
_SEGMENT	DW	?
_FLAGS		DB	?
_NAME		DB	?

CV_LABEL16_STRUCT	ENDS


CV_CEXMODEL16_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_OFFSET		DW	?
_SEGMENT	DW	?
_MODEL		DW	?
_VARIANT	DB	?

CV_CEXMODEL16_STRUCT	ENDS


CV_VFTPATH16_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_OFFSET		DW	?
_SEGMENT	DW	?
_ROOT		DW	?
_PATH		DW	?

CV_VFTPATH16_STRUCT	ENDS


CV_LDATA32_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_OFFSET		DD	?
_SEGMENT	DW	?
_TYPE		DW	?
_NAME		DB	?

CV_LDATA32_STRUCT	ENDS


CV_GDATA32_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_OFFSET		DD	?
_SEGMENT	DW	?
_TYPE		DW	?
_NAME		DB	?

CV_GDATA32_STRUCT	ENDS


CV_LPROC32_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_PPARENT	DD	?
_PEND		DD	?
_PNEXT		DD	?
_PLENGTH	DD	?
_DBG_START	DD	?
_DBG_END	DD	?
_OFFSET		DD	?
_SEGMENT	DW	?
_TYPE		DW	?
_FLAGS		DB	?
_NAME		DB	?

CV_LPROC32_STRUCT	ENDS


CV_GPROC32_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_PPARENT	DD	?
_PEND		DD	?
_PNEXT		DD	?
_PLENGTH	DD	?
_DBG_START	DD	?
_DBG_END	DD	?
_OFFSET		DD	?
_SEGMENT	DW	?
_TYPE		DW	?
_FLAGS		DB	?
_NAME		DB	?

CV_GPROC32_STRUCT	ENDS


CV_THUNK32_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID

_PPARENT	DD	?
_PEND		DD	?
_PNEXT		DD	?
_OFFSET		DD	?

_SEGMENT	DW	?
_PLENGTH	DW	?

_ORDINAL	DB	?
_NAME		DB	?

CV_THUNK32_STRUCT	ENDS


CV_BLOCK32_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID

_PPARENT	DD	?
_PEND		DD	?
_PLENGTH	DD	?
_OFFSET		DD	?

_SEGMENT	DW	?
_NAME		DB	?

CV_BLOCK32_STRUCT	ENDS


CV_LABEL32_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_OFFSET		DD	?
_SEGMENT	DW	?
_FLAGS		DB	?
_NAME		DB	?

CV_LABEL32_STRUCT	ENDS


CV_CEXMODEL32_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_OFFSET		DD	?
_SEGMENT	DW	?
_MODEL		DW	?
_VARIANT	DB	?

CV_CEXMODEL32_STRUCT	ENDS


CV_VFTPATH32_STRUCT	STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_OFFSET		DD	?
_SEGMENT	DW	?
_ROOT		DW	?
_PATH		DW	?

CV_VFTPATH32_STRUCT	ENDS


CV_REF_STRUCT		STRUC

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
_CHECKSUM	DD	?
_OFFSET		DD	?
_MODULE		DW	?
_FILLER		DW	?

CV_REF_STRUCT		ENDS
*/

typedef struct CVG_REF_STRUCT
{
    unsigned _HASH;
    void *_NEXT_HASH_GINDEX;
    void *_NEXT_GSYM_GINDEX;
    unsigned _TEXT_OFFSET;

    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID
    unsigned _ALIGN_OFF;	// OFFSET IN ALIGNSYM SECTION
    unsigned short _MODULE;	// MODULE THIS BELONGS TO
    unsigned short _SEGMENT;	// SEGMENT I AM IN
    unsigned _OFFSET;		// OFFSET IN MY SEGMENT

} CVG_REF_STRUCT;

typedef struct CV_IREF_STRUCT
{
    unsigned short _LENGTH;	// LENGTH OF REST OF SYMBOL
    unsigned short _ID;		// SYMBOL ID

    unsigned _ALIGN_OFF;	// OFFSET IN ALIGNSYM SECTION
    unsigned short _MODULE;	// MODULE THIS BELONGS TO
    unsigned short _SEGMENT;	// SEGMENT I AM IN
    unsigned _OFFSET;		// OFFSET IN MY SEGMENT
} CV_IREF_STRUCT;

/*
// 
//  /CO:5 Symantec SYM1 format structures
// 

cvLTypeStruct	struc		// LOCAL TYPE DEFINITION STRUCTURE

Flags		db	?	// LOCAL TYPE FLAGS			// **
NameOffset	db	?
IntLeaf		dw	?						// ** Fix zeroing in cvtypes (3 places)
		// 
		// UP TO HERE MUST BE DWORD ALIGNED
		// 
Len		dw	?	// TYPE LENGTH
Leaf		dw	?	// 
Word0		dw	?
Word1		dw	?
Word2		dw	?
Word3		dw	?
Word4		dw	?
Word5		dw	?
Word6		dw	?
Word7		dw	?
Word8		dw	?
Word9		dw	?
Word10		dw	?

cvLTypeStruct	ends


cvLTypeFlags		RECORD	cvLTypeInstalled:1, \		// THIS MEANS TYPE HAS BEEN INSTALLED IN GLOBAL TABLE, TID-SIZE==INDEX
				cvLTypeComparing:1, \		// WE ARE CURRENTLY COMPARING THIS GUY
				cvLTypeDefining:1, \		// DEFINING MY TYPES
				cvLTypeCSUE:1, \		// CLASS, STRUCTURE, UNION, OR ENUM
				cvLTypeFWDREF:1, \		// CSUE IS FORWARD REFERENCED
				cvLTypeDefining2:1		// DEFINING, OTHER PASS


cvGTypeStruct	struc

Len		dw	?	// Type length
iLeaf		dw	?	// Internal LEAF number
Word0		dw	?
Word1		dw	?
Word2		dw	?
Word3		dw	?
Word4		dw	?
Word5		dw	?
Word6		dw	?
Word7		dw	?
Word8		dw	?
Word9		dw	?
Word10		dw	?

cvGTypeStruct	ends


cvSymbolStruct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID

cvSymbolStruct	ends


cvRegStruct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
@Type		dd	?
Reg		dw	?
Nam		db	?

cvRegStruct	ends


cvConstStruct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
@Type		dd	?
Value		dw	?

cvConstStruct	ends


cvUDTStruct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
@Type		dd	?
Nam		db	?

cvUDTStruct	ends


cvSearchStruct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
Offs		dd	?	// OFFSET OF SYMBOL
Segm		dw	?	// LOGICAL SEGMENT
Filler		dw	?	// FOR DWORD ALIGN

cvSearchStruct	ends


cvObjStruct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
Signature	dd	?
Nam		db	?

cvObjStruct	ends


cvLData16Struct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
@Type		dd	?
Offs		dw	?
Segm		dw	?
Nam		db	?

cvLData16Struct	ends


cvGData16Struct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
@Type		dd	?
Offs		dw	?
Segm		dw	?
Nam		db	?

cvGData16Struct	ends


cvLProc16Struct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
pParent		dd	?
pEnd		dd	?
pNext		dd	?
@Type		dd	?

PLength		dw	?
DbgStart	dw	?

DbgEnd		dw	?
Offs		dw	?

Segm		dw	?
Flags		db	?
Nam		db	?

cvLProc16Struct	ends


cvGProc16Struct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
pParent		dd	?
pEnd		dd	?
pNext		dd	?
@Type		dd	?

PLength		dw	?
DbgStart	dw	?

DbgEnd		dw	?
Offs		dw	?

Segm		dw	?
Flags		db	?
Nam		db	?

cvGProc16Struct	ends

cvVFTPath16Struct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
@Root		DD	?	// 
@Path		DD	?	// 
Offs		DW	?
Segm		DW	?

cvVFTPath16Struct	ends

cvLData32Struct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
@Type		dd	?
Offs		dd	?
Segm		dw	?
Nam		db	?

cvLData32Struct	ends


cvGData32Struct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
@Type		dd	?
Offs		dd	?
Segm		dw	?
Nam		db	?

cvGData32Struct	ends


cvLProc32Struct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
pParent		dd	?
pEnd		dd	?
pNext		dd	?
@Type		dd	?
PLength		dd	?
DbgStart	dd	?
DbgEnd		dd	?
Offs		dd	?
Segm		dw	?
Flags		db	?
Nam		db	?

cvLProc32Struct	ends


cvGProc32Struct	struc

    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
pParent		dd	?
pEnd		dd	?
pNext		dd	?
@Type		dd	?
PLength		dd	?
DbgStart	dd	?
DbgEnd		dd	?
Offs		dd	?
Segm		dw	?
Flags		db	?
Nam		db	?

cvGProc32Struct	ends


typedef struct cvVFTPath32Struct
{
    unsigned short Len;		// LENGTH OF REST OF SYMBOL
    unsigned short ID;		// SYMBOL ID
    int atRoot;
    int atPath;
    int Offs;
    short Segm;
} cvVFTPath32Struct;

*/
