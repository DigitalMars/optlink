
#include "all.h"

void _do_far_install(CV_GLOBALSYM_STRUCT* EBX,char *ECX,unsigned hash,CV_SYMBOL_STRUCT* ESI);

void _install_globalsym(unsigned hash, char *ECX, CV_SYMBOL_STRUCT *ESI)
{
printf("_install_globalsym(hash = %x, ECX = %p, ESI = %p)\n", hash, ECX, ESI);
		// hash IS HASH VALUE, CONVERT IT
		// ESI IS SYMBOL, ECX IS TEXT
		// SYMBOL IS CONSTANT, GDATA32, GDATA16, UDT

for (int j = 0; j < 1024; j++)
    if (GSYM_HASH_LOG[j]) printf("GSYM_HASH_LOG[%d] = %p\n", j, GSYM_HASH_LOG[j]);

		unsigned i = hash % GSYM_HASH;

		CV_GLOBALSYM_STRUCT *EBX;
		CV_GLOBALSYM_STRUCT *EAX;

		EAX = GSYM_HASH_LOG[i];
		EBX = (CV_GLOBALSYM_STRUCT *)
		    &((unsigned char *)GSYM_HASH_LOG)[i*4 - 4]; // -CV_GLOBALSYM_STRUCT._NEXT_HASH_GINDEX];

		goto FAR_CONT;

FAR_NEXT1:
FAR_NEXT:
		EAX = EBX->_NEXT_HASH_GINDEX;
FAR_CONT:
		if (!EAX)
		{
		    _do_far_install(EBX, ECX, hash, ESI);
		    return;
		}
printf("i = %p EBX = %p EAX = %p\n", i, EBX, EAX);

		EBX = EAX;

		// Bugzilla http://d.puremagic.com/issues/show_bug.cgi?id=4009
		// The following dies with a segfault
		// No, EBX is not NULL, it's 6F7333231

		if (EBX->_HASH != hash)
		    goto FAR_NEXT;

		// PROBABLE MATCH, NEED COMPARE

		if (!ECX)
		    goto FAR_TEXT_NUL;

		unsigned toff = EBX->_TEXT_OFFSET;
		if (!toff)
		    goto FAR_NEXT1;

		unsigned char *EDI = (unsigned char *)EBX + toff + sizeof(CV_GLOBALSYM_STRUCT)-4;
		unsigned len1 = _get_omf_name_length_routine(&EDI);
		unsigned len2 = _get_omf_name_length_routine(&EDI);

		if (len1 != len2)
		    goto FAR_NEXT1;

		if (memcmp(ECX,EDI,len1))
		    goto FAR_NEXT1;

		// COMPARE SYMBOL TYPE (NOT LENGTH, AS DWORD ALIGNMENT MAY OR MAY NOT BE DONE)
		if (ESI->_ID != EBX->_ID)
		    goto FAR_MATCH_FAIL;

FAR_MATCH_DELETE:
		ESI->_ID = I_S_DELETE;
FAR_MATCH_FAIL:
		// FAIL MEANS ANOTHER SYMBOL SAME NAME, DIFFERENT KIND, DON'T
		// DELETE THIS ONE
		return;

FAR_TEXT_NUL:
		CV_IREF_STRUCT *ESI2 = (CV_IREF_STRUCT *)ESI;
		CVG_REF_STRUCT *EBX2 = (CVG_REF_STRUCT *)EBX;

		// NO NAME, ONE MUST BE A REFERENCE
		// TEST SEGMENT AND OFFSET FOR KICKS

		if (ESI2->_OFFSET != EBX2->_OFFSET)
		    goto FAR_NEXT;

		if (ESI2->_ID != EBX2->_ID)
		    goto FAR_NEXT;		// try again if only one is a reference

		if (ESI2->_SEGMENT != EBX2->_SEGMENT)
		    goto FAR_NEXT;		// try again if segments don't match

		ESI2->_ID = I_S_DELETE;
}


void _do_far_install(CV_GLOBALSYM_STRUCT* EBX,char *ECX,unsigned hash,CV_SYMBOL_STRUCT* ESI)
{
if (GSYM_HASH_LOG[0]) printf("test1 %s\n", GSYM_HASH_LOG);
	unsigned EAX;

	// EBX gets pointer
	CVG_REF_STRUCT *EDI = (CVG_REF_STRUCT *)
		_cv_gsym_pool_get(ESI->_LENGTH + sizeof(CV_GLOBALSYM_STRUCT) - 2);

	EBX->_NEXT_HASH_GINDEX = EDI;
	EBX = LAST_GSYM_GINDEX;

	EDI->_HASH = hash;
	LAST_GSYM_GINDEX = EDI;

	if (!EBX)
	    goto L3;

	EBX->_NEXT_GSYM_GINDEX = EDI;
L4:
	if (ECX)			// is there text?
	    EAX = ECX - (char *)ESI;	// offset to text
	else
	    EAX = 0;
	EDI->_TEXT_OFFSET = EAX;
	EDI->_NEXT_HASH_GINDEX = 0;
	unsigned ECX2 = (ESI->_LENGTH + 2) & 0xFFFF;
	EDI->_NEXT_GSYM_GINDEX = 0;
	memcpy(&EDI->_LENGTH, ESI, ECX2);
	ESI->_ID = I_S_DELETE;
if (GSYM_HASH_LOG[0]) printf("test2\n");
	return;

L3:
	FIRST_GSYM_GINDEX = EDI;
	goto L4;
}

