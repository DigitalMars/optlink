!INCLUDE ..\COMMON\MAKEMACS


FLAGS = $(MASMFLAGS)
SRC = $(BASE)\ALLOC
CFLAGS=-I..\common -r

HEADERS=..\common\all.h ..\common\optlink.h ..\common\errors.h ..\common\io_struc.h \
	..\common\exes.h ..\common\library.h ..\common\symbols.h ..\common\groups.h \
	..\common\segments.h ..\common\segmsyms.h ..\common\lnkdat.h ..\common\cvtypes.h \
	..\common\cvstuff.h ..\common\pe_struc.h

ALL : $(LIB)\ALLOC.LIB

$(LIB)\ALLOC.LIB : $(OBJ)\ALLOPOOL.OBJ $(OBJ)\allopoolc.obj $(OBJ)\ldatac.obj \
	$(OBJ)\ALLOC.OBJ $(OBJ)\allocc.obj \
	$(OBJ)\comprelec.obj $(OBJ)\minidatac.obj $(OBJ)\compalloc.obj
  $(BUILD_LIB)


$(OBJ)\MINIDATA.OBJ : MINIDATA.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\MINIDATA.ASM

$(OBJ)\ALLOPOOL.OBJ : ALLOPOOL.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\ALLOPOOL.ASM

$(OBJ)\COMPALLO.OBJ : COMPALLO.ASM ..\COMMON\MACROS ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\COMPALLO.ASM

$(OBJ)\COMPRELE.OBJ : COMPRELE.ASM ..\COMMON\MACROS ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\COMPRELE.ASM

$(OBJ)\ALLOC.OBJ : ALLOC.ASM ..\COMMON\MACROS ..\COMMON\WIN32DEF
  ML $(FLAGS) $(SRC)\ALLOC.ASM

$(OBJ)\INITPOOL.OBJ : INITPOOL.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\INITPOOL.ASM

$(OBJ)\LDATA.OBJ : LDATA.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS
  ML $(FLAGS) $(SRC)\LDATA.ASM


$(OBJ)\allocc.obj : allocc.c $(HEADERS)
	dmc -N_ -c allocc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\allocc.obj

$(OBJ)\allopoolc.obj : allopoolc.c $(HEADERS)
	dmc -N_ -c allopoolc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\allopoolc.obj

$(OBJ)\compalloc.obj : compalloc.c $(HEADERS)
	dmc -N_ -c compalloc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\compalloc.obj

$(OBJ)\comprelec.obj : comprelec.c $(HEADERS)
	dmc -N_ -c comprelec -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\comprelec.obj

$(OBJ)\initpoolc.obj : initpoolc.c $(HEADERS)
	dmc -N_ -c initpoolc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\initpoolc.obj

$(OBJ)\ldatac.obj : ldatac.c $(HEADERS)
	dmc -N_ -c ldatac -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\ldatac.obj

$(OBJ)\minidatac.obj : minidatac.c $(HEADERS)
	dmc -N_ -c minidatac -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\minidatac.obj


