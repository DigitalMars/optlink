!INCLUDE ..\COMMON\MAKEMACS


SRC = $(BASE)\NTIO
FLAGS = $(MASMFLAGS)
CFLAGS=-I..\common -r -N_

HEADERS=..\common\all.h ..\common\optlink.h ..\common\errors.h ..\common\io_struc.h \
	..\common\exes.h ..\common\library.h ..\common\symbols.h ..\common\groups.h \
	..\common\segments.h ..\common\segmsyms.h ..\common\lnkdat.h ..\common\cvtypes.h \
	..\common\cvstuff.h ..\common\pe_struc.h

OBJS= \
	$(OBJ)\fancyc.obj \
	$(OBJ)\findfirsc.obj \
	$(OBJ)\libstuffc.obj \
	$(OBJ)\reopenc.obj \
	$(OBJ)\openoutc.obj \
	$(OBJ)\flushesc.obj \
	$(OBJ)\libreadtc.obj \
	$(OBJ)\printf.obj \
	$(OBJ)\doswritec.obj \
	$(OBJ)\dotc.obj \
	$(OBJ)\openreadc.obj \
	$(OBJ)\gettimec.obj \
	$(OBJ)\DOSIO.OBJ $(OBJ)\xcnotifyc.obj $(OBJ)\CAPTURE.OBJ \
	$(OBJ)\opreadtc.obj \
	$(OBJ)\terrorsc.obj \
	$(OBJ)\openinc.obj \
	$(OBJ)\promptc.obj \
	$(OBJ)\dosposac.obj $(OBJ)\dosioc.obj $(OBJ)\capturec.obj $(OBJ)\recohndlc.obj

ALL : $(LIB)\NTIO.LIB 
# $(LIB)\LOADX.EXE

$(LIB)\NTIO.LIB : $(OBJS)
  del $(LIB)\ntio.lib
  $(BUILD_LIB)

$(LIB)\LOADX.EXE : $(OBJ)\LOADX.OBJ
  OPTLINK $(OBJ)\LOADX,$(LIB)\LOADX;

$(OBJ)\LOADX.OBJ : LOADX.ASM
  OPTASM LOADX /DDEBUG=0 ,$(OBJ)\LOADX;

$(OBJ)\OPENIN.OBJ : OPENIN.ASM ..\COMMON\IO_STRUC ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\OPENIN.ASM

$(OBJ)\DOSPOSA.OBJ : DOSPOSA.ASM ..\COMMON\IO_STRUC ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\DOSPOSA.ASM

$(OBJ)\PROMPT.OBJ : PROMPT.ASM ..\COMMON\IO_STRUC ..\COMMON\MACROS ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\PROMPT.ASM

$(OBJ)\FINDFIRS.OBJ : FINDFIRS.ASM ..\COMMON\IO_STRUC ..\COMMON\MACROS ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\FINDFIRS.ASM

$(OBJ)\LIBSTUFF.OBJ : LIBSTUFF.ASM ..\COMMON\IO_STRUC ..\COMMON\MACROS ..\COMMON\LIBRARY
  ML $(FLAGS) $(SRC)\LIBSTUFF.ASM

$(OBJ)\REOPEN.OBJ : REOPEN.ASM ..\COMMON\IO_STRUC ..\COMMON\MACROS ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\REOPEN.ASM

$(OBJ)\OPENOUT.OBJ : OPENOUT.ASM ..\COMMON\IO_STRUC ..\COMMON\MACROS ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\OPENOUT.ASM

$(OBJ)\FLUSHES.OBJ : FLUSHES.ASM ..\COMMON\IO_STRUC ..\COMMON\MACROS ..\COMMON\WINMACS
  ML $(FLAGS) $(SRC)\FLUSHES.ASM

$(OBJ)\DOSWRITE.OBJ : DOSWRITE.ASM ..\COMMON\IO_STRUC ..\COMMON\MACROS ..\COMMON\WIN32DEF ..\COMMON\WINMACS
  ML $(FLAGS) $(SRC)\DOSWRITE.ASM

$(OBJ)\FANCY.OBJ : FANCY.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\FANCY.ASM

$(OBJ)\OPENREAD.OBJ : OPENREAD.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\OPENREAD.ASM

$(OBJ)\CVPACKRN.OBJ : CVPACKRN.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\CVPACKRN.ASM

$(OBJ)\RECOHNDL.OBJ : RECOHNDL.ASM ..\COMMON\MACROS ..\COMMON\LIBRARY ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\RECOHNDL.ASM

$(OBJ)\DOSIO.OBJ : DOSIO.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC ..\COMMON\EXES ..\COMMON\WINMACS ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\DOSIO.ASM

$(OBJ)\XCNOTIFY.OBJ : XCNOTIFY.ASM ..\COMMON\MACROS ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\XCNOTIFY.ASM

$(OBJ)\CAPTURE.OBJ : CAPTURE.ASM ..\COMMON\MACROS ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\CAPTURE.ASM

$(OBJ)\OPREADT.OBJ : OPREADT.ASM ..\COMMON\MACROS ..\COMMON\WIN32DEF ..\COMMON\IO_STRUC ..\COMMON\EXES
  ML $(FLAGS) $(SRC)\OPREADT.ASM

$(OBJ)\LIBREADT.OBJ : LIBREADT.ASM ..\COMMON\MACROS ..\COMMON\WIN32DEF ..\COMMON\IO_STRUC ..\COMMON\LIBRARY
  ML $(FLAGS) $(SRC)\LIBREADT.ASM

$(OBJ)\TERRORS.OBJ : TERRORS.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC ..\COMMON\SYMCMACS ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\TERRORS.ASM

$(OBJ)\capturec.obj : capturec.c $(HEADERS)
	dmc -c capturec -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\capturec.obj

$(OBJ)\dosioc.obj : dosioc.c $(HEADERS)
	dmc -c dosioc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\dosioc.obj

$(OBJ)\dosposac.obj : dosposac.c $(HEADERS)
	dmc -c dosposac -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\dosposac.obj

$(OBJ)\doswritec.obj : doswritec.c $(HEADERS)
	dmc -c doswritec -NTPASS2_TEXT $(CFLAGS) -o$(OBJ)\doswritec.obj

$(OBJ)\dotc.obj : dotc.c $(HEADERS)
	dmc -c dotc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\dotc.obj

$(OBJ)\fancyc.obj : fancyc.c $(HEADERS)
	dmc -c fancyc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\fancyc.obj

$(OBJ)\findfirsc.obj : findfirsc.c $(HEADERS)
	dmc -c findfirsc -NTPHASE1_TEXT $(CFLAGS) -o$(OBJ)\findfirsc.obj

$(OBJ)\flushesc.obj : flushesc.c $(HEADERS)
	dmc -c flushesc -NTPASS2_TEXT $(CFLAGS) -o$(OBJ)\flushesc.obj

$(OBJ)\gettimec.obj : gettimec.c $(HEADERS)
	dmc -c gettimec -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\gettimec.obj

$(OBJ)\libreadtc.obj : libreadtc.c $(HEADERS)
	dmc -o -c libreadtc -NTPASS1_TEXT $(CFLAGS) -o$(OBJ)\libreadtc.obj

$(OBJ)\libstuffc.obj : libstuffc.c $(HEADERS)
	dmc -c libstuffc -NTPASS1_TEXT $(CFLAGS) -o$(OBJ)\libstuffc.obj

$(OBJ)\openinc.obj : openinc.c $(HEADERS)
	dmc -c openinc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\openinc.obj

$(OBJ)\openoutc.obj : openoutc.c $(HEADERS)
	dmc -c openoutc -NTPASS2_TEXT $(CFLAGS) -o$(OBJ)\openoutc.obj

$(OBJ)\openreadc.obj : openreadc.c $(HEADERS)
	dmc -c openreadc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\openreadc.obj

$(OBJ)\opreadtc.obj : opreadtc.c $(HEADERS)
	dmc -c opreadtc -NTPASS1_TEXT $(CFLAGS) -o$(OBJ)\opreadtc.obj

$(OBJ)\printf.obj : printf.c $(HEADERS)
	dmc -c printf -NTPASS2_TEXT $(CFLAGS) -I$(DM)\src\include -o$(OBJ)\printf.obj

$(OBJ)\promptc.obj : promptc.c $(HEADERS)
	dmc -c promptc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\promptc.obj

$(OBJ)\recohndlc.obj : recohndlc.c $(HEADERS)
	dmc -c recohndlc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\recohndlc.obj

$(OBJ)\reopenc.obj : reopenc.c $(HEADERS)
	dmc -c reopenc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\reopenc.obj

$(OBJ)\terrorsc.obj : terrorsc.c $(HEADERS)
	dmc -c terrorsc -NTPASS2_TEXT $(CFLAGS) -o$(OBJ)\terrorsc.obj

$(OBJ)\xcnotifyc.obj : xcnotifyc.c $(HEADERS)
	dmc -o -c xcnotifyc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\xcnotifyc.obj



