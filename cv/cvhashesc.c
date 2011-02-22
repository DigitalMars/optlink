#include "all.h"

extern unsigned FINAL_HIGH_WATER;
extern unsigned char CV_TEMP_RECORD;
extern unsigned CV_PUB_TXT_OFFSET;
extern unsigned CV_PUB_SYMBOL_ID;
extern unsigned CVG_SEGMENT;
extern unsigned BYTES_SO_FAR;
extern unsigned CVG_SYMBOL_OFFSET;
extern unsigned CVG_SEGMENT_OFFSET;
extern unsigned CVG_SYMBOL_HASH;
extern unsigned LAST_CVH_SEGMENT;
extern struct STD_PTR_S CV_HASHES_GARRAY;
extern struct ALLOCS_STRUCT CV_HASHES_STUFF;
extern void (*CV_DWORD_ALIGN)();

unsigned CV_PAGE_BYTES;
unsigned CV_HASH_COUNT;
//unsigned CV_TEMP_JUNK;
unsigned *CVG_PUT_PTR;
unsigned *CVG_PUT_LIMIT;
unsigned *CVG_PUT_BLK;
unsigned CV_SECTION_OFFSET;
unsigned CV_SECTION_HDR_ADDRESS;
unsigned CV_SYMBOL_BASE_ADDR;
unsigned CVG_N_HASHES;
unsigned CVG_BUFFER_LOG;
unsigned CVG_BUFFER_LIMIT;
struct CV_HASHES_STRUCT *FIRST_CVH;
struct CV_HASHES_STRUCT *LAST_CVH;


unsigned short CV_PRIMES[] =
{
        1,3,7,0x0D,0x1F,0x2F,0x3D,0x4F,0x59,0x6D,0x7F,0x8B,0x9D,0x0AD,0x0BF,0x0C7,0x0DF,0x0EF,0x0FB,
        0x11B,0x13D,0x15D,0x17F,0x199,0x1BB,0x1DF,0x1FD,
        0x21D,0x23B,0x25F,0x277,0x295,0x2BD,0x2DD,0x2F9,
        0x31D,0x33D,0x35F,0x377,0x397,0x3B9,0x3DF,0x3FD,
        0x43F,0x47F,0x4BD,0x4FF,
        0x52F,0x577,0x5BF,0x5FB,
        0x63D,0x67F,0x6BB,0x6FD,
        0x737,0x779,0x7BB,0x7F7,
        0x83F,0x871,0x8BF,0x8F9,
//      0x935,0x977,0x9AD,0x9FD,
//      0x0A3D,0x0A7F,0x0ABD,0x0AF3,
//      0x0B3F,0x0B7B,0x0BB9,0x0BFB,
//      0x0C31,0x0C77,0x0CBB,0x0CFB,
//      0x0D3F,0x0D79,0x0DBD,0x0DFF,
//      0x0E3B,0x0E7D,0x0EB9,0x0EF9,
//      0x0F31,0x0F7F,0x0FBB,0x0FFD,
};

struct CV_HASH_HDR_STRUCT CV_HASH_HEADER = { 10,12,0,0,0 };


#if 0
                TITLE   CVHASHES - Copyright (c) SLR Systems 1994

                INCLUDE MACROS
if      fg_cvpack
                INCLUDE CVTYPES
                INCLUDE CVSTUFF

                PUBLIC  INIT_CV_SYMBOL_HASHES,STORE_CV_SYMBOL_INFO,FLUSH_CV_SYMBOL_HASHES,OUTPUT_CV_SYMBOL_ALIGN,FIRST_CVH


                .DATA



                .CODE   CVPACK_TEXT

                EXTERNDEF       MOVE_TEXT_TO_OMF:PROC,HANDLE_CV_INDEX:PROC,FLUSH_CV_TEMP:PROC,RELEASE_BLOCK:PROC
                EXTERNDEF       ALLOC_LOCKED:PROC,GET_NEW_LOG_BLK:PROC,MOVE_EAX_TO_FINAL_HIGH_WATER:PROC,RELEASE_LOCKED:PROC
                EXTERNDEF       MOVE_EAX_TO_EDX_FINAL:PROC,SORT_HASHES_GARRAY:PROC,GET_NAME_HASH32:PROC,CV_HASHES_POOL_GET:PROC
                EXTERNDEF       RELEASE_GARRAY:PROC,_release_minidata:proc
#endif

void _init_cv_symbol_hashes()
{
	// INITIALIZE STUFF USED FOR GLOBAL SYMBOL HASH TABLES

	CV_HASH_COUNT = 0;
	CV_PAGE_BYTES = 0;

	FIRST_CVH = 0;
	LAST_CVH = 0;

	CVG_SYMBOL_OFFSET = 0;

	(*CV_DWORD_ALIGN)();          // make sure of DWORD alignment

	unsigned EAX = BYTES_SO_FAR;
	unsigned ECX = FINAL_HIGH_WATER;
	CV_SECTION_OFFSET = EAX;
	CV_SECTION_HDR_ADDRESS = ECX;
	EAX += sizeof(struct CV_HASH_HDR_STRUCT);
	ECX += sizeof(struct CV_HASH_HDR_STRUCT);

	BYTES_SO_FAR = EAX;
	FINAL_HIGH_WATER = ECX;

	CV_SYMBOL_BASE_ADDR = ECX;

	unsigned *p = _get_new_log_blk();
	CVG_PUT_BLK = p;
	CVG_PUT_PTR = p;
	CVG_PUT_LIMIT = p + (PAGE_SIZE - 512) / sizeof(*p);
}

void _store_cv_symbol_info()
{
    struct CV_HASHES_STRUCT *cvh = (struct CV_HASHES_STRUCT *)_cv_hashes_pool_get(sizeof(struct CV_HASHES_STRUCT));

    ++CV_HASH_COUNT;

    cvh->_SYMBOL_OFFSET = CVG_SYMBOL_OFFSET;
    cvh->_SYMBOL_HASH = CVG_SYMBOL_HASH;
    cvh->_SEGMENT_OFFSET = CVG_SEGMENT_OFFSET;
    cvh->_SEGMENT = CVG_SEGMENT;

    struct CV_HASHES_STRUCT *ECX = LAST_CVH;
    if (ECX)
	ECX->_NEXT = cvh;
    else
	FIRST_CVH = cvh;
    cvh->_PREV = ECX;
    LAST_CVH = cvh;
    cvh->_NEXT = 0;
    cvh->_NEXT_HASH = 0;
}

unsigned _output_cv_symbol_align(struct CV_SYMBOL_STRUCT *ESI /* EAX */)
{
    // EAX IS CV_TEMP_RECORD
    // 
    // ALIGN SYMBOL STORED IN CV_TEMP_RECORD
    // 
    // RETURN EAX IS OFFSET OF THIS SYMBOL
    // 

    unsigned EDX = ESI->_LENGTH;

    unsigned char *p = (unsigned char *)ESI + EDX;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;

    EDX += (2 - EDX) & 3;              // # OF ZEROS TO ADD AT THE END

    ESI->_LENGTH = EDX;
    unsigned ECX = 2 + EDX;

    // 
    // DO 4K ALIGNMENT CALCULATION
    // 
    if (DOING_4K_ALIGN)		// STATICSYM doesn't matter
    {
	EDX = 0x1000;			// 4K

	unsigned EAX = CV_PAGE_BYTES + ECX;
	if (EDX < EAX)
	    goto L2;
	EDX -= EAX;
	if (!EDX)
	    goto L28;
	// MUST LEAVE 0 OR AT LEAST 8 BYTES
	if (EDX > 8)
	    goto L29;
L2:
	// 
	// INSERT S_ALIGN SYMBOL FOR PAGE ALIGNMENT
	// 
	unsigned *EDI = CVG_PUT_PTR;

	unsigned nbytes = 0x1000 - 2;	    // 4K-2
	nbytes -= CV_PAGE_BYTES;            // # OF BYTES TO FILL
	EDX = S_ALIGN * 0x10000;	    // S_ALIGN*64K
	EDX |= nbytes;
	nbytes -= 2;

	*EDI++ = EDX;

	// BUG: seg faults here with long symbol,
	//  nbytes apparently went negative.
	//  Happens when record length is > 0x1000
	//  Bugzilla 2436
	memset(EDI, 0, nbytes);
	EDI = (unsigned *)((char *)EDI + nbytes);

	CVG_PUT_PTR = EDI;
	if (EDI >= CVG_PUT_LIMIT)
	    _flush_cvg_temp();

	EDX = ECX;
L28:
	EAX = EDX;
L29:
	CV_PAGE_BYTES = EAX;       // # OF BYTES IN PAGE AFTER THIS SYMBOL GOES OUT
    }

    // 
    // STORE IN BUFFER
    // 
    unsigned EAX = FINAL_HIGH_WATER - CV_SYMBOL_BASE_ADDR;
    EAX += (char *)CVG_PUT_PTR - (char *)CVG_PUT_BLK;

    memcpy(CVG_PUT_PTR, ESI, ECX);

    CVG_PUT_PTR = (unsigned *)((char *)CVG_PUT_PTR + ECX);

    if (CVG_PUT_PTR >= CVG_PUT_LIMIT)
	_flush_cvg_temp();
    return EAX;
}

void _flush_cvg_temp()
{
    unsigned ECX = (char *)CVG_PUT_PTR - (char *)CVG_PUT_BLK;
    CVG_PUT_PTR = CVG_PUT_BLK;
    if (ECX)
        _move_eax_to_final_high_water(CVG_PUT_BLK, ECX);
}

#if 0
FLUSH_CV_SYMBOL_HASHES  PROC
                ;
                ;CLEAN UP THE MESS YOU STARTED...
                ;
                PUSH    EAX
                CALL    FLUSH_CVG_TEMP

                MOV     EAX,FINAL_HIGH_WATER
                MOV     EDX,CV_SYMBOL_BASE_ADDR

                SUB     EAX,EDX
                MOV     EDX,BYTES_SO_FAR

                MOV     CV_HASH_HEADER._CVHH_CBSYMBOL,EAX
                ADD     EDX,EAX

                MOV     BYTES_SO_FAR,EDX
                CALL    DO_SYMBOL_HASH

                CALL    DO_ADDRESS_HASH

                ;
                ;WRITE OUT CV_HASH_HEADER
                ;
                MOV     EAX,OFF CV_HASH_HEADER
                MOV     EDX,CV_SECTION_HDR_ADDRESS

                MOV     ECX,SIZE CV_HASH_HDR_STRUCT
                CALL    MOVE_EAX_TO_EDX_FINAL

                MOV     EAX,CVG_PUT_BLK
                CALL    RELEASE_BLOCK

                MOV     EAX,OFF CV_HASHES_GARRAY
                CALL    RELEASE_GARRAY

                MOV     EAX,OFF CV_HASHES_STUFF

                push    EAX
                call    _release_minidata
                add     ESP,4

                POP     ECX                     ;CV_INDEX
                MOV     EAX,CV_SECTION_OFFSET

                JMP     HANDLE_CV_INDEX         ;BACKWARDS

FLUSH_CV_SYMBOL_HASHES  ENDP


DO_SYMBOL_HASH  PROC    NEAR
                ;
                ;
                ;
                MOV     EAX,CV_HASH_COUNT

                TEST    EAX,EAX
                JNZ     L0$

                MOV     CV_HASH_HEADER._CVHH_CBSYMHASH,EAX

                RET

L0$:
                PUSHM   EDI,ESI

                PUSH    EBX
                MOV     EAX,FINAL_HIGH_WATER

                PUSH    EAX
                ;
                ;CALCULATE # OF HASH BUCKETS
                ;
                MOV     EBX,17+16
                MOV     EAX,CV_HASH_COUNT
L1$:
                DEC     EBX

                ADD     EAX,EAX
                JNC     L1$

                CMP     EBX,10
                JA      L12$

                MOV     EBX,10
L12$:
                MOV     EAX,CV_HASH_COUNT
                SUB     EBX,7

                XOR     EDX,EDX
                MOV     ESI,OFF CV_PRIMES

                ASSUME  ESI:PTR WORD

                DIV     EBX
                ;
                ;HIGH LIMIT IS PAGE_SIZE/8
                ;
                CMP     EAX,PAGE_SIZE_8_HASH
                JB      L14$

                MOV     EAX,PAGE_SIZE_8_HASH
L14$:
                ;
                ;NOW CONVERT TO NEXT HIGHEST PRIME #
                ;
                XOR     ECX,ECX
L16$:
                MOV     CX,[ESI]
                ADD     ESI,2

                CMP     ECX,EAX
                JB      L16$

                MOV     CX,[ESI-2]
                MOV     EDI,CVG_PUT_BLK

                MOV     CVG_N_HASHES,ECX
                MOV     ESI,LAST_CVH
                ;
                ;SCAN CV_HASHES, LINK-LIST STUFF PLEASE
                ;
                XOR     EAX,EAX
                ADD     ECX,ECX

                REP     STOSD

                MOV     EDI,CVG_PUT_BLK
                MOV     ECX,ESI

L2$:
                CONVERT ESI,ESI,CV_HASHES_GARRAY
                ASSUME  ESI:PTR CV_HASHES_STRUCT

                MOV     EAX,[ESI]._SYMBOL_HASH
                XOR     EDX,EDX

                DIV     CVG_N_HASHES

                MOV     EAX,[EDI+EDX*8]         ;LINK-LIST TO THIS 'BUCKET'
                MOV     [EDI+EDX*8],ECX

                MOV     EBX,[EDI+EDX*8+4]
                MOV     [ESI]._NEXT_HASH,EAX

                INC     EBX                     ;COUNT GUYS IN THIS 'BUCKET'
                MOV     ESI,[ESI]._PREV

                MOV     [EDI+EDX*8+4],EBX
                MOV     ECX,ESI

                TEST    ESI,ESI
                JNZ     L2$
L29$:
                ;
                ;NOW START WRITING HASH TABLE OUT
                ;
                CALL    GET_NEW_LOG_BLK

                MOV     ESI,EAX
                MOV     EBX,CVG_N_HASHES
                ASSUME  ESI:NOTHING

                MOV     CVG_BUFFER_LOG,EAX
                MOV     [EAX],EBX

                ADD     EAX,PAGE_SIZE
                XOR     EDX,EDX
                ;
                ;WRITE OFFSET OF EACH CHAIN
                ;
                MOV     CVG_BUFFER_LIMIT,EAX
                XOR     ECX,ECX
L3$:
                MOV     EAX,[EDI+ECX*8+4]       ;BUCKET COUNT
                MOV     [ESI+ECX*4+4],EDX

                INC     ECX
                DEC     EBX

                LEA     EDX,[EDX+EAX*8]
                JNZ     L3$

                INC     ECX
                MOV     EAX,ESI

                SHL     ECX,2
                CALL    MOVE_EAX_TO_FINAL_HIGH_WATER
                ;
                ;NOW WRITE BUCKET COUNTS
                ;
                MOV     ESI,CVG_PUT_BLK
                MOV     EDI,CVG_BUFFER_LOG

                ADD     ESI,4                   ;LOOK AT COUNT
                MOV     ECX,CVG_N_HASHES
L4$:
                MOV     EAX,[ESI]
                ADD     ESI,8

                MOV     [EDI],EAX
                ADD     EDI,4

                DEC     ECX
                JNZ     L4$

                MOV     ECX,EDI
                MOV     EAX,CVG_BUFFER_LOG

                SUB     ECX,EAX
                CALL    MOVE_EAX_TO_FINAL_HIGH_WATER
                ;
                ;NOW WRITE CHAINS
                ;
                MOV     EBX,CVG_PUT_BLK
                MOV     EDI,CVG_BUFFER_LOG

                MOV     EDX,CVG_N_HASHES
L5$:
                MOV     ESI,[EBX]               ;FIRST ITEM IN CHAIN
                ADD     EBX,8
L51$:
                TEST    ESI,ESI
                JZ      L58$

                CONVERT ESI,ESI,CV_HASHES_GARRAY
                ASSUME  ESI:PTR CV_HASHES_STRUCT

                MOV     EAX,[ESI]._SYMBOL_OFFSET
                MOV     ECX,[ESI]._SYMBOL_HASH

                MOV     [EDI],EAX
                MOV     [EDI+4],ECX

                ADD     EDI,8
                MOV     EAX,CVG_BUFFER_LIMIT

                CMP     EDI,EAX
                JAE     L57$
L56$:
                MOV     ESI,[ESI]._NEXT_HASH
                JMP     L51$

L58$:
                DEC     EDX
                JNZ     L5$

                MOV     ECX,EDI
                MOV     EAX,CVG_BUFFER_LOG

                SUB     ECX,EAX
                JZ      L59$

                CALL    MOVE_EAX_TO_FINAL_HIGH_WATER
L59$:
                ;
                ;STORE BYTE-COUNT FOR SYMBOL HASH STUFF
                ;
                POP     ECX
                MOV     EAX,FINAL_HIGH_WATER

                SUB     EAX,ECX
                MOV     ECX,BYTES_SO_FAR

                MOV     CV_HASH_HEADER._CVHH_CBSYMHASH,EAX
                ADD     ECX,EAX

                MOV     EAX,CVG_BUFFER_LOG
                MOV     BYTES_SO_FAR,ECX

                POPM    EBX,ESI

                POP     EDI
                JMP     RELEASE_BLOCK

L57$:
                MOV     ECX,EDI
                MOV     EAX,CVG_BUFFER_LOG

                PUSH    EDX
                SUB     ECX,EAX

                MOV     EDI,CVG_BUFFER_LOG
                CALL    MOVE_EAX_TO_FINAL_HIGH_WATER

                POP     EDX
                JMP     L56$

DO_SYMBOL_HASH  ENDP

#endif


void _do_address_hash()
{
    // Build and output address hash

    //printf("_do_address_hash()\n");
    unsigned *CV_SEG_TBL[128/(PAGE_SIZE/1024)];
    unsigned CVG_CSEG;
    unsigned **CV_SEGTBL_PTR;
    unsigned *CV_SEGTBL_PTR_LIMIT;
    struct CVH_STRUCT* *CV_EXETABLE_PTR;
    unsigned **CV_NEXT_SEG_TBL;

    if (!CV_HASH_COUNT)
    {
        CV_HASH_HEADER._CVHH_CBADDRHASH = 0;
        return;
    }
    _sort_hashes_garray();              // sort in address order

    unsigned final_high_water_save = FINAL_HIGH_WATER;

    CV_SEGTBL_PTR = &CV_SEG_TBL[0];

    // now build count table

    CVG_CSEG = LAST_CVH_SEGMENT;        // == # of segments I allow for

    unsigned *EDI = (unsigned *)_get_new_log_blk();
    *CV_SEGTBL_PTR++ = EDI;
    CV_SEGTBL_PTR_LIMIT = EDI + PAGE_SIZE / sizeof(*EDI);

    CV_EXETABLE_PTR = (struct CVH_STRUCT **)&EXETABLE[0];

                                    // ALSO START WITH SYMBOL # 1
    unsigned segnum = 1;                // CURRENTLY LOOKING FOR SEG 1

    *EDI = 0;                   // COUNT IS ZERO FOR SEGMENT #1
    unsigned hcount = CV_HASH_COUNT;
    while (1)
    {
        struct CVH_STRUCT *ESI = *CV_EXETABLE_PTR++;

        struct CVH_STRUCT *EBX = ESI + PAGE_SIZE / sizeof(*ESI);
L11:
        unsigned segment = ESI->_SEGMENT;
        ++ESI;

        while (segnum != segment)
        {
            if (!segment)                       // CONSTANT?
                goto L17;

            ++segnum;
            ++EDI;

            if (EDI == CV_SEGTBL_PTR_LIMIT)
            {
                EDI = _get_new_log_blk();
                *CV_SEGTBL_PTR++ = EDI;
                CV_SEGTBL_PTR_LIMIT = EDI + PAGE_SIZE / sizeof(*EDI);
            }

            *EDI = 0;
        }

        (*EDI)++;                       // COUNT SYMBOLS IN THAT SEGMENT
L17:
        --hcount;
        if (!hcount)
            break;

        if (ESI < EBX)
            goto L11;
    }

    // OUTPUT STUFF

    unsigned *ESI = CVG_PUT_PTR;
    unsigned *EBX = ESI + PAGE_SIZE / sizeof(*ESI);
    *ESI++ = CVG_CSEG;
    CV_NEXT_SEG_TBL = &CV_SEG_TBL[1];

    if (CVG_CSEG)
    {
        unsigned *EDI = CV_SEG_TBL[0];
        unsigned *CV_SEGTBL_PTR_LIMIT = EDI + PAGE_SIZE / sizeof(*EDI);
        unsigned EAX = 0;
        unsigned ECX = CVG_CSEG;
        while (1)
        {
            unsigned EDX = *EDI++;              // # OF ENTRIES
            *ESI++ = EAX;

            EAX += EDX * 8;                     // 8 bytes per entry

            ECX--;
            if (!ECX)
                break;
            if (ESI == EBX)
            {
                ESI = CVG_PUT_BLK;
                _move_eax_to_final_high_water(ESI, PAGE_SIZE);
            }
            if (EDI == CV_SEGTBL_PTR_LIMIT)
            {
                EDI = *CV_NEXT_SEG_TBL++;
                CV_SEGTBL_PTR_LIMIT = EDI + PAGE_SIZE / sizeof(*EDI);
            }
        }
    }

    {
    unsigned ECX = (char *)ESI - (char *)CVG_PUT_BLK;
    if (ECX)
        _move_eax_to_final_high_water(CVG_PUT_BLK, ECX);
    }

    // NOW OUTPUT OFFSET COUNTS
    unsigned **EBX2 = &CV_SEG_TBL[0];           // table pointing to counts
    do
    {
        unsigned ECX = CVG_CSEG;

        if (ECX >= PAGE_SIZE/4)
            ECX = PAGE_SIZE/4;

        CVG_CSEG -= ECX;
        if (ECX)
            _move_eax_to_final_high_water(*EBX2, ECX * 4);
        _release_block(*EBX2);
        ++EBX2;
    } while (CVG_CSEG);

    // OUTPUT ACTUAL SYMBOL_OFFSETS AND SEGMENT_OFFSETS

    unsigned count = CV_HASH_COUNT;

    CV_EXETABLE_PTR = (struct CVH_STRUCT **)&EXETABLE[0];
    struct CVH_STRUCT *cvhp = *CV_EXETABLE_PTR++;

    struct CVH_STRUCT *cvhp_top = cvhp + PAGE_SIZE / sizeof(*cvhp);

    unsigned *EDI2 = CVG_PUT_BLK;
    unsigned *CV_PUT_LIMIT = EDI2 + PAGE_SIZE / sizeof(*EDI2);

    while (1)
    {
        if (cvhp->_SEGMENT)
        {
            EDI2[0] = cvhp->_SYMBOL_OFFSET;
            EDI2[1] = cvhp->_SEGMENT_OFFSET;

            EDI2 += 2;

            if (EDI2 == CV_PUT_LIMIT)
            {
                EDI2 -= PAGE_SIZE / sizeof(*EDI2);
                _move_eax_to_final_high_water(EDI2, PAGE_SIZE);
            }
        }
        --count;
        if (!count)
            break;

        cvhp += 1;

        if (cvhp == cvhp_top)
        {
            _release_block(CV_EXETABLE_PTR[-1]);
            CV_EXETABLE_PTR[-1] = 0;
            cvhp = *CV_EXETABLE_PTR;
            ++CV_EXETABLE_PTR;
            cvhp_top = cvhp + PAGE_SIZE / sizeof(*cvhp);
        }
    }

    unsigned ECX = (char *)EDI2 - (char *)CVG_PUT_BLK;
    if (ECX)
        _move_eax_to_final_high_water(CVG_PUT_BLK, ECX);

    // STORE BYTE-COUNT FOR ADDRESS HASH STUFF
    unsigned EAX = FINAL_HIGH_WATER - final_high_water_save;
    BYTES_SO_FAR += EAX;
    CV_HASH_HEADER._CVHH_CBADDRHASH = EAX;
}


