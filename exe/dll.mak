BASE = ..
#D:\P386

LST = .\LSTDLL
OBJ = .\OBJDLL
MASMFLAGS = /DHOS=W32D
LIB = $(BASE)\LIBDLL

MASMFLAGS = $(MASMFLAGS) /c /Cp /Fl$(LST)\ /Sn /Zi /Fo$(OBJ)\ /I$(BASE)\COMMON


SRC = $(BASE)\EXE
FLAGS = $(MASMFLAGS)

ALL : $(LIB)\EXE.LIB

$(LIB)\EXE.LIB : $(OBJ)\NEWSECT.OBJ $(OBJ)\EXEINIT.OBJ $(OBJ)\INITRANG.OBJ $(OBJ)\MOVEFINA.OBJ $(OBJ)\FLUSFINA.OBJ $(OBJ)\RNEWSEG.OBJ \
			$(OBJ)\OUTLDATA.OBJ $(OBJ)\RFLUSH.OBJ $(OBJ)\ZERO.OBJ $(OBJ)\REALRELO.OBJ $(OBJ)\EOSEGS.OBJ \
			$(OBJ)\ENDSECT.OBJ $(OBJ)\CALCEXE.OBJ $(OBJ)\EXESTR.OBJ $(OBJ)\SEGMINIT.OBJ $(OBJ)\PE_CHANG.OBJ \
			$(OBJ)\PFLUSH.OBJ $(OBJ)\PNEWSEG.OBJ $(OBJ)\PACK0.OBJ $(OBJ)\PE_RELOC.OBJ $(OBJ)\ICODE.OBJ $(OBJ)\IDATA.OBJ \
			$(OBJ)\PEENDSEC.OBJ $(OBJ)\PENDSECT.OBJ $(OBJ)\FLUSHEXE.OBJ $(OBJ)\SEGMRELO.OBJ $(OBJ)\FIXDS.OBJ \
			$(OBJ)\WINPRELO.OBJ
  OPTLIB /OKMULTI $(LIB)\EXE ~+ $(OBJ)\*;

$(OBJ)\NEWSECT.OBJ : NEWSECT.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS ..\COMMON\IO_STRUC ..\COMMON\SECTS ..\COMMON\SECTIONS ..\COMMON\EXES \
			..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\NEWSECT.ASM

$(OBJ)\EXEINIT.OBJ : EXEINIT.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\EXEINIT.ASM

$(OBJ)\INITRANG.OBJ : INITRANG.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\INITRANG.ASM

$(OBJ)\MOVEFINA.OBJ : MOVEFINA.ASM ..\COMMON\MACROS ..\COMMON\WINMACS
  ML $(FLAGS) $(SRC)\MOVEFINA.ASM

$(OBJ)\FLUSFINA.OBJ : FLUSFINA.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC ..\COMMON\WINMACS
  ML $(FLAGS) $(SRC)\FLUSFINA.ASM

$(OBJ)\RNEWSEG.OBJ : RNEWSEG.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS ..\COMMON\MODULES
  ML $(FLAGS) $(SRC)\RNEWSEG.ASM

$(OBJ)\OUTLDATA.OBJ : OUTLDATA.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS
  ML $(FLAGS) $(SRC)\OUTLDATA.ASM

$(OBJ)\RFLUSH.OBJ : RFLUSH.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS ..\COMMON\IO_STRUC ..\COMMON\SECTS
  ML $(FLAGS) $(SRC)\RFLUSH.ASM

$(OBJ)\ZERO.OBJ : ZERO.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\ZERO.ASM

$(OBJ)\REALRELO.OBJ : REALRELO.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS ..\COMMON\RELOCSS ..\COMMON\EXES
  ML $(FLAGS) $(SRC)\REALRELO.ASM

$(OBJ)\EOSEGS.OBJ : EOSEGS.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\EOSEGS.ASM

$(OBJ)\ENDSECT.OBJ : ENDSECT.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC ..\COMMON\SECTS ..\COMMON\SECTIONS ..\COMMON\EXES
  ML $(FLAGS) $(SRC)\ENDSECT.ASM

$(OBJ)\CALCEXE.OBJ : CALCEXE.ASM ..\COMMON\MACROS ..\COMMON\EXES
  ML $(FLAGS) $(SRC)\CALCEXE.ASM

$(OBJ)\EXESTR.OBJ : EXESTR.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\EXESTR.ASM

$(OBJ)\SEGMINIT.OBJ : SEGMINIT.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC ..\COMMON\SEGMSYMS ..\COMMON\EXES
  ML $(FLAGS) $(SRC)\SEGMINIT.ASM

$(OBJ)\PE_CHANG.OBJ : PE_CHANG.ASM ..\COMMON\MACROS ..\COMMON\PE_STRUC
  ML $(FLAGS) $(SRC)\PE_CHANG.ASM

$(OBJ)\PFLUSH.OBJ : PFLUSH.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS ..\COMMON\SEGMSYMS
  ML $(FLAGS) $(SRC)\PFLUSH.ASM

$(OBJ)\PNEWSEG.OBJ : PNEWSEG.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS ..\COMMON\PE_STRUC ..\COMMON\SEGMSYMS
  ML $(FLAGS) $(SRC)\PNEWSEG.ASM

$(OBJ)\PACK0.OBJ : PACK0.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\PACK0.ASM

$(OBJ)\PE_RELOC.OBJ : PE_RELOC.ASM ..\COMMON\MACROS ..\COMMON\PE_STRUC ..\COMMON\EXES
  ML $(FLAGS) $(SRC)\PE_RELOC.ASM

$(OBJ)\ICODE.OBJ : ICODE.ASM ..\COMMON\MACROS ..\COMMON\PE_STRUC ..\COMMON\SEGMSYMS
  ML $(FLAGS) $(SRC)\ICODE.ASM

$(OBJ)\IDATA.OBJ : IDATA.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS ..\COMMON\SYMBOLS
  ML $(FLAGS) $(SRC)\IDATA.ASM

$(OBJ)\PEENDSEC.OBJ : PEENDSEC.ASM ..\COMMON\MACROS ..\COMMON\PE_STRUC ..\COMMON\EXES ..\COMMON\IO_STRUC ..\COMMON\SYMBOLS
  ML $(FLAGS) $(SRC)\PEENDSEC.ASM

$(OBJ)\PENDSECT.OBJ : PENDSECT.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS ..\COMMON\GROUPS ..\COMMON\EXES ..\COMMON\IO_STRUC ..\COMMON\SECTS ..\COMMON\RESSTRUC ..\COMMON\SEGMSYMS
  ML $(FLAGS) $(SRC)\PENDSECT.ASM

$(OBJ)\FLUSHEXE.OBJ : FLUSHEXE.ASM ..\COMMON\MACROS ..\COMMON\EXES
  ML $(FLAGS) $(SRC)\FLUSHEXE.ASM

$(OBJ)\FIXDS.OBJ : FIXDS.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\FIXDS.ASM

$(OBJ)\SEGMRELO.OBJ : SEGMRELO.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS ..\COMMON\RELOCSS ..\COMMON\SEGMSYMS
  ML $(FLAGS) $(SRC)\SEGMRELO.ASM

$(OBJ)\WINPRELO.OBJ : WINPRELO.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS ..\COMMON\RELOCSS ..\COMMON\SEGMSYMS
  ML $(FLAGS) $(SRC)\WINPRELO.ASM


