!INCLUDE ..\COMMON\MAKEMACS


FLAGS = $(MASMFLAGS)
SRC = $(BASE)\MOVES
DMD = \dmd2\windows\bin\dmd
CFLAGS=-I..\common -r


ALL : $(LIB)\MOVES.LIB


$(LIB)\MOVES.LIB : $(OBJ)\MVLNAME.OBJ $(OBJ)\PUBCHECK.OBJ $(OBJ)\DOPUBLIC.OBJ \
	$(OBJ)\PUTDATA.OBJ $(OBJ)\FAKELEDA.OBJ $(OBJ)\putdatac.obj
  del $(LIB)\moves.lib
  OPTLIB /OK $(LIB)\MOVES ~+ $(OBJ)\*;


$(OBJ)\PUTDATA.OBJ : PUTDATA.ASM ..\COMMON\IO_STRUC ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\PUTDATA.ASM

$(OBJ)\MVLNAME.OBJ : MVLNAME.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\MVLNAME.ASM

$(OBJ)\PUBCHECK.OBJ : PUBCHECK.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\PUBCHECK.ASM

$(OBJ)\DOPUBLIC.OBJ : DOPUBLIC.ASM ..\COMMON\MACROS ..\COMMON\SYMBOLS ..\COMMON\SEGMENTS ..\COMMON\CDDATA
  ML $(FLAGS) $(SRC)\DOPUBLIC.ASM

$(OBJ)\FAKELEDA.OBJ : FAKELEDA.ASM ..\COMMON\MACROS ..\COMMON\SEGMENTS
  ML $(FLAGS) $(SRC)\FAKELEDA.ASM


$(OBJ)\putdatac.obj : putdatac.c
	dmc -N_ -c putdatac -NTPASS2_TEXT $(CFLAGS) -o$(OBJ)\putdatac.obj

