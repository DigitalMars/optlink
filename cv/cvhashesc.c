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
unsigned *CVG_BUFFER_LOG;
unsigned *CVG_BUFFER_LIMIT;
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

    //printf("\nESI = %p, length = %x\n", ESI, ESI->_LENGTH);

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
	/* Insert S_ALIGN symbol to ensure that the next symbol will not cross
	 * a page boundary.
	 * The format is:
	 *	word length
	 *	word S_ALIGN
	 *	... pad bytes ...
	 */ 

	unsigned nbytes = 0x1000 - 2;	    // 4K-2
	nbytes -= CV_PAGE_BYTES;            // # OF BYTES TO FILL
	EDX = S_ALIGN * 0x10000;	    // S_ALIGN*64K
	EDX |= nbytes;
	nbytes -= 2;

	unsigned *EDI = CVG_PUT_PTR;

	// Fix for Bugzilla 2436 where it would seg fault on the memset()
	if ((char *)EDI - (char *)CVG_PUT_BLK + nbytes + 2 > 0x4000)
	{
	    _flush_cvg_temp();
	    EDI = CVG_PUT_PTR;
	}

	*EDI++ = EDX;			    // write length, S_ALIGN

	memset(EDI, 0, nbytes);		    // pad bytes
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

extern void DO_SYMBOL_HASH();

void _flush_cv_symbol_hashes(unsigned EAX /* CV_INDEX */)
{
    // CLEAN UP THE MESS YOU STARTED
    _flush_cvg_temp();

    unsigned n = FINAL_HIGH_WATER - CV_SYMBOL_BASE_ADDR;
    CV_HASH_HEADER._CVHH_CBSYMBOL = n;
    BYTES_SO_FAR += n;

    _do_symbol_hash();
    _do_address_hash();

    // WRITE OUT CV_HASH_HEADER

    _move_eax_to_edx_final(&CV_HASH_HEADER, sizeof(struct CV_HASH_HDR_STRUCT), CV_SECTION_HDR_ADDRESS);

    _release_block(CVG_PUT_BLK);
    _release_garray(&CV_HASHES_GARRAY);

    _release_minidata(&CV_HASHES_STUFF);

    _handle_cv_index(CV_SECTION_OFFSET, EAX);	// backwards
}

void _do_symbol_hash()
{
    if (!CV_HASH_COUNT)
    {   CV_HASH_HEADER._CVHH_CBSYMHASH = 0;
	return;
    }

    unsigned final_high_water_save = FINAL_HIGH_WATER;

    // CALCULATE # OF HASH BUCKETS
    unsigned EBX = 17 + 16;
    unsigned EAX = CV_HASH_COUNT;

    while (1)
    {
	--EBX;
	if ((int)EAX < 0)
	    break;
	EAX *= 2;
    }

    if (EBX < 10)
	EBX = 10;
    EAX = CV_HASH_COUNT;
    EBX -= 7;

    unsigned short *pprim = &CV_PRIMES[0];

    EAX /= EBX;

    // HIGH LIMIT IS PAGE_SIZE/8

    if (EAX > PAGE_SIZE_8_HASH)
	EAX = PAGE_SIZE_8_HASH;

    // Convert to next highest prime #
    while (*pprim++ < EAX)
	;

    CVG_N_HASHES = pprim[-1];
    struct CV_HASHES_STRUCT *esi2 = LAST_CVH;

    // SCAN CV_HASHES, LINK-LIST STUFF
    memset(CVG_PUT_BLK, 0, CVG_N_HASHES * 8);

    unsigned *EDI = CVG_PUT_BLK;

    do
    {
	unsigned EDX = esi2->_SYMBOL_HASH % CVG_N_HASHES;

	esi2->_NEXT_HASH = (struct CV_HASHES_STRUCT *)EDI[EDX*2]; // LINK-LIST TO THIS 'BUCKET'
	EDI[EDX*2] = (unsigned)esi2;
	++EDI[EDX*2 + 1];	// count guys in this bucket
	esi2 = esi2->_PREV;
    } while (esi2);

    // Start writing hash table out
    unsigned *pu = (unsigned *)_get_new_log_blk();

    unsigned nhashes = CVG_N_HASHES;

    CVG_BUFFER_LOG = pu;
    *pu = nhashes;

    unsigned EDX = 0;

    // WRITE OFFSET OF EACH CHAIN
    CVG_BUFFER_LIMIT = pu + PAGE_SIZE / sizeof(*pu);
    unsigned ECX = 0;

    do
    {
	unsigned EAX = EDI[ECX*2 + 1];       // BUCKET COUNT
	pu[ECX + 1] = EDX;

	++ECX;
	--nhashes;

	EDX += EAX * 8;
    } while (nhashes);

    _move_eax_to_final_high_water(pu, (ECX + 1) * 4);

    // Write bucket counts
    unsigned *psi = CVG_PUT_BLK;
    psi++;                   // LOOK AT COUNT
    EDI = CVG_BUFFER_LOG;
    unsigned nhashes2 = CVG_N_HASHES;
    do
    {
	*EDI++ = *psi;
	psi += 2;
    } while (--nhashes2);

    _move_eax_to_final_high_water(CVG_BUFFER_LOG, (char *)EDI - (char *)CVG_BUFFER_LOG);

    // Write chains

    struct CV_HASHES_STRUCT **ebx2 = (struct CV_HASHES_STRUCT **)CVG_PUT_BLK;
    EDI = CVG_BUFFER_LOG;
    EDX = CVG_N_HASHES;
    do
    {
	struct CV_HASHES_STRUCT *ESI = *ebx2; // first item in chain
	ebx2 += 2;

	while (ESI)
	{
	    EDI[0] = ESI->_SYMBOL_OFFSET;
	    EDI[1] = ESI->_SYMBOL_HASH;
	    EDI += 2;

	    if (EDI >= CVG_BUFFER_LIMIT)
	    {
		unsigned bufcount = (char *)EDI - (char *)CVG_BUFFER_LOG;
		EDI = CVG_BUFFER_LOG;
		_move_eax_to_final_high_water(EDI, bufcount);
	    }
	    ESI = ESI->_NEXT_HASH;
	}
    } while (--EDX);

    unsigned bufcount = (char *)EDI - (char *)CVG_BUFFER_LOG;
    if (bufcount)
	_move_eax_to_final_high_water(CVG_BUFFER_LOG, bufcount);

    // STORE BYTE-COUNT FOR SYMBOL HASH STUFF
    unsigned count = FINAL_HIGH_WATER - final_high_water_save;
    CV_HASH_HEADER._CVHH_CBSYMHASH = count;
    BYTES_SO_FAR += count;
    _release_block(CVG_BUFFER_LOG);
}


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


