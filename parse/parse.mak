!INCLUDE ..\COMMON\MAKEMACS


FLAGS = $(MASMFLAGS)
SRC = $(BASE)\PARSE
CFLAGS=$(DMCFLAGS) -N_


ALL : $(LIB)\PARSE.LIB

$(LIB)\PARSE.LIB : $(OBJ)\mvsrcfilc.obj $(OBJ)\chknulc.obj \
	$(OBJ)\librtnc.obj \
	$(OBJ)\mvpathc.obj $(OBJ)\do_fnc.obj \
	$(OBJ)\hndllibsc.obj \
	$(OBJ)\HNDLLIBS.OBJ \
	$(OBJ)\mvfilnamc.obj \
	$(OBJ)\mvlistnfc.obj \
	$(OBJ)\mvfnc.obj $(OBJ)\parse_fnc.obj $(OBJ)\get_fnc.obj
  del $(LIB)\PARSE.LIB
  $(BUILD_LIB)

$(OBJ)\MVSRCFIL.OBJ : MVSRCFIL.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\MVSRCFIL.ASM

$(OBJ)\MVFILNAM.OBJ : MVFILNAM.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\MVFILNAM.ASM

$(OBJ)\PARSE_FN.OBJ : PARSE_FN.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\PARSE_FN.ASM

$(OBJ)\MVFN.OBJ : MVFN.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\MVFN.ASM

$(OBJ)\MVFNASC.OBJ : MVFNASC.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\MVFNASC.ASM

$(OBJ)\MVLISTNF.OBJ : MVLISTNF.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\MVLISTNF.ASM

$(OBJ)\GET_FN.OBJ : GET_FN.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\GET_FN.ASM

$(OBJ)\DO_FN.OBJ : DO_FN.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\DO_FN.ASM

$(OBJ)\CHKNUL.OBJ : CHKNUL.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\CHKNUL.ASM

$(OBJ)\HNDLLIBS.OBJ : HNDLLIBS.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC
  ML $(FLAGS) $(SRC)\HNDLLIBS.ASM

$(OBJ)\GTNXTU.OBJ : GTNXTU.ASM ..\COMMON\MACROS
  ML $(FLAGS) $(SRC)\GTNXTU.ASM

$(OBJ)\LIBRTN.OBJ : LIBRTN.ASM ..\COMMON\MACROS ..\COMMON\IO_STRUC ..\COMMON\EXES
  ML $(FLAGS) $(SRC)\LIBRTN.ASM

$(OBJ)\chknulc.obj : chknulc.c
	dmc -c chknulc -NTPHASE1_TEXT $(CFLAGS) -o$(OBJ)\chknulc.obj

$(OBJ)\do_fnc.obj : do_fnc.c
	dmc -c do_fnc -NTFILEPARSE_TEXT $(CFLAGS) -o$(OBJ)\do_fnc.obj

$(OBJ)\get_fnc.obj : get_fnc.c
	dmc -c get_fnc -NTFILEPARSE_TEXT $(CFLAGS) -o$(OBJ)\get_fnc.obj

$(OBJ)\hndllibsc.obj : hndllibsc.c
	dmc -c hndllibsc -NTPHASE1_TEXT $(CFLAGS) -o$(OBJ)\hndllibsc.obj

$(OBJ)\librtnc.obj : librtnc.c
	dmc -c librtnc -NTFILEPARSE_TEXT $(CFLAGS) -o$(OBJ)\librtnc.obj

$(OBJ)\mvfilnamc.obj : mvfilnamc.c
	dmc -c mvfilnamc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\mvfilnamc.obj

$(OBJ)\mvfnc.obj : mvfnc.c
	dmc -c mvfnc -NTPHASE1_TEXT $(CFLAGS) -o$(OBJ)\mvfnc.obj

$(OBJ)\mvfnascc.obj : mvfnascc.c
	dmc -c mvfnascc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\mvfnascc.obj

$(OBJ)\mvlistnfc.obj : mvlistnfc.c
	dmc -c mvlistnfc -NTROOT_TEXT $(CFLAGS) -o$(OBJ)\mvlistnfc.obj

$(OBJ)\mvpathc.obj : mvpathc.c
	dmc -c mvpathc -NTPHASE1_TEXT $(CFLAGS) -o$(OBJ)\mvpathc.obj

$(OBJ)\mvsrcfilc.obj : mvsrcfilc.c
	dmc -c mvsrcfilc -NTFILEPARSE_TEXT $(CFLAGS) -o$(OBJ)\mvsrcfilc.obj

$(OBJ)\parse_fnc.obj : parse_fnc.c
	dmc -c parse_fnc -NTFILEPARSE_TEXT $(CFLAGS) -o$(OBJ)\parse_fnc.obj


