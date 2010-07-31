
struct GROUP_STRUCT
{
    uint _G_NEXT_HASH_GINDEX;
    uint _G_NEXT_GROUP_GINDEX;	//NEXT GROUP IN SOME LIST ORDER

    uint _G_FIRST_SEG_GINDEX;
    uint _G_LAST_SEG_GINDEX;
    uint _G_FRAME;	//OFFSET AND 0FFF0H IF REAL MODE, ELSE SEGMENT # & FLAGS
    uint _G_OFFSET;	//OFFSET FROM FRAME (0) - OR REAL ADDRESS FOR REAL MODE
    uint _G_LEN;	//LENGTH... AFTER ALIGN

    uint _G_NUMBER;	//GROUP NUMBER...  FOR INTERNAL FIXUPPS AND CV NUMBERING
    uint _G_NEXT_CV_GINDEX;

  version (fg_prot)
  {
    _G_OS2_NUMBER		=	(_G_FRAME)
    uint _G_OS2_FLAGS;
  }

  version (fg_plink)
  {
    uint _G_SECTION_INDEX;
    ubyte _G_PLTYPE;	//(PLTYPES RECORD)
    ubyte reserved;
  }
  else
  {
    ushort reserved;
  }

    ubyte _G_TYPE;	//UNDEF, ASEG, RELOC, ETC
    ubyte _G_TYPE1;	//CODE OR DATA	(GREC) 16/32, ETC

    uint _G_HASH;
    uint _G_TEXT;

} // ***** MUST BE EVEN WORDS *****

enum GREC
{
	GROUP_IS_USE16:1,
	GROUP_IS_USE32:1,
	GROUP_IS_FLAT:1,
	GROUP_IS_DGROUP:1,
	GROUP_IS_CODE:1,
	GROUP_IS_DATA:1
}
