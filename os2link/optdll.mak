
LIBS=         CV EXE COMMON SUBS MOVES ALLOC PARSE INSTALL OVERLAYS NTIO KERNEL32 USER32
DIRS= OS2LINK CV EXE COMMON SUBS MOVES ALLOC PARSE INSTALL OVERLAYS
LINKCMD = \dm\bin\linkold /SILENT /NOI /NOERROR

!IFNDEF DM
DM = \dm
!ENDIF

!IFDEF HOST_WIN32

DIRS = NTIO $(DIRS)

EXT = NT
MASMFLAGS = /DHOS=W32
MAKECMD = HOST_WIN32=
LINKCMD1 = $(LINKCMD)  @..\slrNT
LINKCMD2 = $(LINKCMD)  @.\slrnt3

!ENDIF

LST = .\LST$(EXT)
OBJ = .\OBJ$(EXT)
LIBd = ..\LIB$(EXT)
LIB = $(LIB);$(LIBd)


default : $(DIRS) OS2LNK

OPTLINK: $(DIRS)
	cd ..\OS2LINK
	$(DM)\bin\link $(OBJ)\* ..\appload\slrload,$(OBJ)\link.exe,$(OBJ)\link.map,..\LIBNT\ $(LIBS)/CO/MAP/DET/XREF/NOI/EXETYPE:NT;


$(DIRS) :
	@echo on
	cd ..\$@
	@if not exist $(LST) md $(LST)
	@if not exist $(OBJ) md $(OBJ)
	@if not exist $(LIBd) md $(LIBd)
	nmake -nologo -f $@.MAK $(MAKECMD)

OS2LNK	:
	@echo on
	cd ..\OS2LINK
	@if not exist .\OBJ$(EXT) mkdir .\OBJ$(EXT)
	cd OBJ$(EXT)
!IFDEF	HOST_WIN32
	copy ..\OBJ\LNKX.EXE>nul
!ENDIF
	$(LINKCMD1)
	cd ..
!IFDEF	HOST_WIN32
	copy .\OBJ\LNKX.EXE>nul
!ENDIF
	$(LINKCMD2)
	
