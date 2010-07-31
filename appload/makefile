# Use MAKE, not NMAKE

#if not exist ..\LIB mkdir ..\LIB
#if not exist ..\LIBNT mkdir ..\LIBNT
#if not exist ..\LIBDLL mkdir ..\LIBDLL

targets: ..\lib\slrload.lib ..\libnt\slrload.lib ..\libdll\slrload.lib

bin2db.exe : bin2db.c
	sc bin2db

slrload.dat: bin2db.exe
	OPTASM NAPPLOAD /DCOMPRESS_APPLOADER\#-1 /DCOMPRESS_PASS1\#-1 /DCOMPRESS_PASS2\#0 /DC_LEN\#0 /DB_C_LEN\#0 /DDEBUG\#0 /L/N;
	OPTLINK NAPPLOAD/BI/MA;
	COMPRESS\OBJ\COMP NAPPLOAD.SYS NAPPLOAD.ZZZ 0X32 0X1F7
	BIN2DB NAPPLOAD.ZZZ COMPRESS.DAT 10
	OPTASM NAPPLOAD /DCOMPRESS_APPLOADER\#-1 /DCOMPRESS_PASS1\#0 /DCOMPRESS_PASS2\#-1 /DDEBUG\#0 /DC_LEN\#1817 /DB_C_LEN\#0B9CH,,n1/l/n;
	BIN2DB NAPPLOAD.OBJ SLRLOAD.DAT

slrloadb.dat: bin2db.exe
	OPTASM NAPPLOAD /DCOMPRESS_APPLOADER\#-1 /DCOMPRESS_PASS1\#-1 /DCOMPRESS_PASS2\#0 /DC_LEN\#0 /DB_C_LEN\#0 /DDEBUG\#-1 ,,N2/L/N;
	OPTLINK NAPPLOAD/BI/MA;
	COMPRESS\OBJ\COMP NAPPLOAD.SYS NAPPLOAD.ZZZ 0X36 0X228
	DIR NAPPLOAD.ZZZ
	BIN2DB NAPPLOAD.ZZZ COMPRESS.DAT 10
	OPTASM NAPPLOAD /DCOMPRESS_APPLOADER\#-1 /DCOMPRESS_PASS1\#0 /DCOMPRESS_PASS2\#-1 /DDEBUG\#-1 /DC_LEN\#2410 /DB_C_LEN\#1132H;
	BIN2DB NAPPLOAD.OBJ SLRLOADB.DAT

..\lib\slrload.lib: slrload.asm slrload.dat slrloadb.dat
	ML /DHOS=DOSX /c /Cp SLRLOAD.ASM
	OPTLIB ..\LIB\SLRLOAD ~+SLRLOAD;


..\libnt\slrload.lib: slrload.asm slrload.dat slrloadb.dat
	ML /DHOS=W32 /c /Cp SLRLOAD.ASM
	OPTLIB ..\LIBNT\SLRLOAD ~+SLRLOAD;

..\libdll\slrload.lib: slrload.asm slrload.dat slrloadb.dat
	ML /DHOS=W32D /c /Cp SLRLOAD.ASM
	OPTLIB ..\LIBDLL\SLRLOAD ~+SLRLOAD;

clean:
	del bin2db.obj
	del bin2db.exe
	del bin2db.map
	del compress.dat
	del slrload.dat
	del slrloadb.dat
	del nappload.obj
	del nappload.sys
	del nappload.zzz
	del nappload.map
	del slrload.obj
	del napploa2.lst
	del nappload.lst
	del n1.lst
	del n2.lst
