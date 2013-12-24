
SCP=\putty\pscp -i c:\.ssh\colossus.ppk
SCPDIR=walter@mercury:dm/optlink

ZIPFILE=optlinkcsrc.zip

DOTSRC= build.bat dorel.bat parse.d revert.bat install.bat getlog.bat clean.bat \
	testall.bat gr.bat makefile

ALLOCCSRC= \
	alloc\malloc.c

ALLOCSRC = $(ALLOCCSRC) \
	alloc\go.bat \
	alloc\initpool.asm \
	alloc\alloinit.asm \
	alloc\comprele.asm \
	alloc\initpoolc.c \
	alloc\compallo.asm \
	alloc\minidata.asm \
	alloc\compalloc.c \
	alloc\minidatac.c \
	alloc\comprelec.c \
	alloc\ldata.asm \
	alloc\alloc.mak \
	alloc\ldatac.c \
	alloc\alloc.asm \
	alloc\allopool.asm \
	alloc\allopoolc.c \
	alloc\allocc.c

APPLOADSRC= \
	appload\appload.mak \
	appload\bin2db.c \
	appload\go.bat \
	appload\newexe.inc \
	appload\go1.bat \
	appload\macros \
	appload\slrload.asm \
	appload\db.bat \
	appload\doit.bat \
	appload\optlink.exe \
	appload\optasm.exe \
	appload\optlib.exe \
	appload\makefile \
	appload\nappload.asm \
	appload\slrloadb.dat \
	appload\slrload.dat \
	appload\compress.dat \
	appload\nappload.obj \
	appload\nappload.sys \
	appload\nappload.zzz \
	appload\bin2db.exe \
	appload\slrload.obj \
	appload\compress\back.bat \
	appload\compress\slr32 \
	appload\compress\data.asm \
	appload\compress\comp.asm \
	appload\compress\rexepack.asm \
	appload\compress\macros \
	appload\compress\crc32.asm \
	appload\compress\ctest \
	appload\compress\lzw \
	appload\compress\unslr.asm \
	appload\compress\db.bat \
	appload\compress\go.bat \
	appload\compress\l.bat \
	appload\compress\comp1.zip \
	appload\compress\comp.zip

BUGSSRC= bugs\test.c

COMMONSRCC= \
	common\all.h \
	common\pe_struc.h \
	common\fixupp2c.c \
	common\mscmdlinc.c \
	common\recordsc.c

COMMONSRC= $(COMMONSRCC) \
	common\opreadt.asm common\nbkpat.asm common\cextdef.asm \
	common\c32quik.asm common\c32moves.asm common\unexe2.asm \
	common\unexe.sys common\linsym.asm common\c32slr.asm common\cddata \
	common\unexe.dat common\unexe.asm common\theadr.c common\classes \
	common\theadr.asm common\tdtypes common\maptemp common\tdbg common\modend.asm \
	common\coff_lib common\modules common\symcmacs common\lnames.asm \
	common\symbols common\link common\alias.asm common\stack.asm common\mapsubs.asm \
	common\csubs.asm common\cvstuff common\cvtypes \
	common\cx.asm common\defpubs.asm common\demangle.asm common\writemap.asm \
	common\segmsyms common\winmacs common\exe.asm \
	common\exepack.asm common\exes common\segments common\lzw common\win32def \
	common\fix2temp common\fixtemps common\sects common\sectmap.asm \
	common\library common\sections common\rexepack.asm common\resstruc \
	common\relocss common\quiktemp common\quikrelo.asm common\go.bat \
	common\groups common\slr32 common\quick.asm common\grpdef.asm common\grpdefc.asm \
	common\grpdefc.c common\pubdef.asm common\pe_struc \
	common\ledata.asm common\japan.inc common\virdef.asm common\personal.asm \
	common\io_struc common\makemacs common\unpack32.asm \
	common\unquik.asm common\sects.d common\initcode.d common\optlnk.d \
	common\segments.d common\symbols.d common\groups.d common\cmdsubs.d \
	common\io_struc.d common\fixupp.d common\macros \
	common\pe_sect.asm common\strtmap.asm common\symmap.asm \
	common\lnkdat.asm common\fixupp2.d common\lidata.asm common\initcode \
	common\align.asm common\comdat.asm common\comdats.asm \
	common\comdef.asm common\communal.asm common\fixupp2.asm \
	common\forref2.asm common\initmap.asm common\packsegs.asm \
	common\records.asm common\segdef.asm common\xrfmap.asm \
	common\middle.asm common\c32.asm common\libase.asm common\order.asm \
	common\lnkinitc.c common\errors.h common\segmsyms.h common\exes.h \
	common\groups.h common\segments.h common\pass2c.c common\pass2.asm \
	common\cmdsubsc.c common\optlnkc.c common\subs.asm common\cfgproc.asm \
	common\iniproc.asm common\linmap.asm common\segmap.asm \
	common\optlnk.asm common\release common\coment.asm common\cmdsubs.asm \
	common\lnkinit.asm common\mscmdlin.asm common\linnum.asm \
	common\forref.asm common\fixupp.asm common\io_struc.h \
	common\macrosc.c common\errors.asm common\macros.h common\pass1.asm \
	common\obj_mod.asm common\extdef.asm common\symbols.h \
	common\common.mak common\library.h common\optlink.h \
	common\lnkdat.h common\newlib.asm common\newlibc.c \
	common\cvtypes.h common\cvstuff.h common\cddata.h

CVSRCC= \
	cv\instgsymc.c \
	cv\cvhashesc.c \
	cv\cvlin4c.c \
	cv\cvsymbolc.c \
	cv\xdebugc.c

CVSRC= $(CVSRCC) \
	cv\xdebug.asm \
	cv\txtomf.asm \
	cv\cvindex.asm \
	cv\cvpublic.asm \
	cv\cvpuball.asm \
	cv\cvlinnum.asm \
	cv\cvmodall.asm \
	cv\go.bat \
	cv\cvlin4.asm \
	cv\link \
	cv\cvstuff.asm \
	cv\cvpub4.asm \
	cv\ccnt.bat \
	cv\cv.mak \
	cv\nt.mak \
	cv\dll.mak \
	cv\cvmod3.asm \
	cv\cvmod4.asm \
	cv\cvsym1.asm \
	cv\cvsymbol.asm \
	cv\qsortadr.asm \
	cv\qsortlin.asm \
	cv\cvglball.asm \
	cv\cvsegtbl.asm \
	cv\cvstaall.asm \
	cv\cvtypall.asm \
	cv\derivate.asm \
	cv\cvfilall.asm \
	cv\cvtypcon.asm \
	cv\cvtypes.asm \
	cv\instgsym.asm \
	cv\cvlibrar.asm


DLLSTUFFSRC= \
	dllstuff\go.bat \
	dllstuff\link \
	dllstuff\libentry.asm \
	dllstuff\dllentry.asm \
	dllstuff\spwntool.c \
	dllstuff\dllstuff.mak

EXESRC= \
	exe\shldxdi.asm \
	exe\icode.asm \
	exe\flushexe.asm \
	exe\fixds.asm \
	exe\pack0.asm \
	exe\endsect.asm \
	exe\eosegs.asm \
	exe\exe.mak \
	exe\exeinit.asm \
	exe\pe_chang.asm \
	exe\fllinnum.asm \
	exe\outldata.asm \
	exe\compmsg.asm \
	exe\go.bat \
	exe\handcomp.asm \
	exe\calcexe.asm \
	exe\idata.asm \
	exe\initrang.asm \
	exe\ldata.asm \
	exe\link \
	exe\exestr.asm \
	exe\zero.asm \
	exe\pnewseg.asm \
	exe\pflush.asm \
	exe\nt.mak \
	exe\dll.mak \
	exe\peendsec.asm \
	exe\aistuff.asm \
	exe\ratstuff.asm \
	exe\omf.asm \
	exe\rflush.asm \
	exe\dosxinit.asm \
	exe\winprelo.asm \
	exe\realrelo.asm \
	exe\pendsect.asm \
	exe\pe_reloc.asm \
	exe\rnewseg.asm \
	exe\segmrelo.asm \
	exe\movefina.asm \
	exe\flusfina.asm \
	exe\segminit.asm \
	exe\newsect.asm

INSTALLSRC= \
	install\instout.asm \
	install\softpent.asm \
	install\sentrynm.asm \
	install\sentry.asm \
	install\putgroup.asm \
	install\defclass.asm \
	install\dsegmod.asm \
	install\nameinst.asm \
	install\finsegatc.c \
	install\fuzzinst.asm \
	install\getsegmt.asm \
	install\go.bat \
	install\ientry.asm \
	install\ientrynm.asm \
	install\install.mak \
	install\mycomdat.asm \
	install\instcomm.asm \
	install\instcommc.c \
	install\modname.asm \
	install\modinst.asm \
	install\instflnm.asm \
	install\instgrp.asm \
	install\instimpn.asm \
	install\instnmsp.asm \
	install\srcinst.asm \
	install\instpent.asm \
	install\instrelo.asm \
	install\instseg.asm \
	install\instsoft.asm \
	install\iresname.asm \
	install\irestype.asm \
	install\link \
	install\mlticseg.asm \
	install\instclas.asm \
	install\instfile.asm \
	install\farinst.asm \
	install\farinstc.c \
	install\addtoext.asm \
	install\modpagec.c

MOVESSRC= \
	moves\mvlname.asm \
	moves\pubcheck.asm \
	moves\go.bat \
	moves\dopublic.asm \
	moves\mvlname.d \
	moves\putdata.d \
	moves\pubcheck.d \
	moves\fakeleda.d \
	moves\dopublic.d \
	moves\moves.mak \
	moves\putdata.asm \
	moves\putdatac.c \
	moves\fakeleda.asm

NTIOSRC = \
	ntio\dosposa.asm \
	ntio\dot.asm \
	ntio\go.bat \
	ntio\gettime.asm \
	ntio\cvpackrn.asm \
	ntio\dosio.mak \
	ntio\loadx.asm \
	ntio\dotc.c \
	ntio\recohndl.asm \
	ntio\openout.asm \
	ntio\reopen.asm \
	ntio\capture.asm \
	ntio\prompt.asm \
	ntio\dosio.asm \
	ntio\findfirs.asm \
	ntio\openin.asm \
	ntio\dosposac.c \
	ntio\reopenc.c \
	ntio\findfirsc.c \
	ntio\openoutc.c \
	ntio\gettimec.c \
	ntio\recohndlc.c \
	ntio\capturec.c \
	ntio\flushes.asm \
	ntio\libstuff.asm \
	ntio\libstuffc.c \
	ntio\dosioc.c \
	ntio\doswrite.asm \
	ntio\doswritec.c \
	ntio\xcnotify.asm \
	ntio\flushesc.c \
	ntio\xcnotifyc.c \
	ntio\libreadt.asm \
	ntio\printf.c \
	ntio\fancy.asm \
	ntio\openinc.c \
	ntio\fancyc.c \
	ntio\openread.asm \
	ntio\openreadc.c \
	ntio\terrors.asm \
	ntio\terrorsc.c \
	ntio\promptc.c \
	ntio\opreadt.asm \
	ntio\ntio.mak \
	ntio\opreadtc.c \
	ntio\libreadtc.c

OS2LINKSRC= \
	os2link\mdgo.bat \
	os2link\try.bat \
	os2link\slrnt3 \
	os2link\2go.txt \
	os2link\slrnt1 \
	os2link\slrdll \
	os2link\db.bat \
	os2link\domaps.bat \
	os2link\fv.cfg \
	os2link\slr3dll \
	os2link\godll.bat \
	os2link\gowin.bat \
	os2link\help \
	os2link\slr3 \
	os2link\slr2 \
	os2link\l.bat \
	os2link\slr1 \
	os2link\setlibnt.bat \
	os2link\setlibdll.bat \
	os2link\setlib.bat \
	os2link\perform.doc \
	os2link\os2link.mak \
	os2link\optlinks.cfg \
	os2link\lwin32.bat \
	os2link\lwindll.bat \
	os2link\x.def \
	os2link\go.bat \
	os2link\build.bat \
	os2link\slr \
	os2link\launch.bat \
	os2link\release \
	os2link\relnotes.txt \
	os2link\linknd.din \
	os2link\makefile \
	os2link\optdll.mak \
	os2link\slrnt \
	os2link\install.bat \
	os2link\src\tsymc.asm \
	os2link\src\symc.asm \
	os2link\src\ressort.asm \
	os2link\src\prolog.asm \
	os2link\src\pe_debug.asm \
	os2link\src\realpers.asm \
	os2link\src\impexp.asm \
	os2link\src\mscope.asm \
	os2link\src\realia.asm \
	os2link\src\peimport.asm \
	os2link\src\peexport.asm \
	os2link\src\pe_rsrc.asm \
	os2link\src\entries.asm \
	os2link\src\def.asm \
	os2link\src\first.asm \
	os2link\src\director.asm \
	os2link\src\outdin.asm \
	os2link\src\outlibc.asm \
	os2link\src\outlib.asm \
	os2link\src\resource.asm

OVERLAYSSRC= \
	overlays\link \
	overlays\personal.asm \
	overlays\allocate.asm \
	overlays\allosect.asm \
	overlays\allovtbl.asm \
	overlays\aslrovl.asm \
	overlays\overlays.mak \
	overlays\declsegs.asm \
	overlays\genstuff.asm \
	overlays\go.bat \
	overlays\sizesegs.asm

PARSESRC= \
	parse\gtnxtu.asm \
	parse\go.bat \
	parse\mvfnasc.asm \
	parse\mvsrcfil.asm \
	parse\mvfilnam.asm \
	parse\mvpath.asm \
	parse\mvlistnf.asm \
	parse\gtnxtuc.c \
	parse\mvfn.asm \
	parse\chknul.asm \
	parse\parse_fn.asm \
	parse\get_fnc.c \
	parse\get_fn.asm \
	parse\chknulc.c \
	parse\mvfnc.c \
	parse\do_fn.asm \
	parse\mvsrcfilc.c \
	parse\mvfilnamc.c \
	parse\mvfnascc.c \
	parse\parse_fnc.c \
	parse\mvlistnfc.c \
	parse\do_fnc.c \
	parse\librtn.asm \
	parse\librtnc.c \
	parse\parse.mak \
	parse\hndllibsc.c \
	parse\hndllibs.asm \
	parse\mvpathc.c

SCSRC = sc\unmangle.c

SUBSSRC= \
	subs\storeseq.asm \
	subs\relexe_5.asm \
	subs\readseq.asm \
	subs\casecomp.asm \
	subs\putdata.asm \
	subs\perform.asm \
	subs\ovlterm.asm \
	subs\tdpartbl.asm \
	subs\tptrvptr.asm \
	subs\go.bat \
	subs\getname.asm \
	subs\link \
	subs\hexwout.asm \
	subs\getsmmod.asm \
	subs\xref.asm \
	subs\set_outf.asm \
	subs\eoind.asm \
	subs\local.asm \
	subs\unmangle.asm \
	subs\myis.asm \
	subs\subs.mak \
	subs\arrays.asm \
	subs\eoindc.c \
	subs\getnamec.c \
	subs\myisc.c \
	subs\dismap.asm \
	subs\listinit.asm

X32SRC= x32\lib\x32v.lib x32\lib\slr.obj x32\lib\zlx.lod

scp: makefile
	tolf makefile $(ALLOCCSRC) $(COMMONSRC) $(CVSRC)
	detab $(CVSRCC)
	$(SCP) makefile optlink_vs08.sln optlink_vs08.vcproj $(SCPDIR)
	$(SCP) $(ALLOCSRC) $(SCPDIR)/alloc
	$(SCP) $(COMMONSRC) $(SCPDIR)/common
	$(SCP) $(CVSRC) $(SCPDIR)/cv

zip:
	tolf common\all.h $(ALLOCCSRC) $(COMMONSRCC) $(CVSRCC)
	del $(ZIPFILE)
	zip32 $(ZIPFILE) $(ALLOCSRC)
	zip32 $(ZIPFILE) $(APPLOADSRC)
	zip32 $(ZIPFILE) $(BUGSSRC)
	zip32 $(ZIPFILE) $(CVSRC)
	zip32 $(ZIPFILE) $(DLLSTUFFSRC)
	zip32 $(ZIPFILE) $(DOTSRC)
	zip32 $(ZIPFILE) $(EXESRC)
	zip32 $(ZIPFILE) $(COMMONSRC)
	zip32 $(ZIPFILE) $(INSTALLSRC)
	zip32 $(ZIPFILE) $(MOVESSRC)
	zip32 $(ZIPFILE) $(NTIOSRC)
	zip32 $(ZIPFILE) $(OS2LINKSRC)
	zip32 $(ZIPFILE) $(OVERLAYSSRC)
	zip32 $(ZIPFILE) $(PARSESRC)
	zip32 $(ZIPFILE) $(SCSRC)
	zip32 $(ZIPFILE) $(SUBSSRC)
	zip32 $(ZIPFILE) $(X32SRC)

clean:
	del ntio\objnt\*.obj libnt\ntio.lib
	del os2link\objnt\*.obj libnt\os2link.lib
	del cv\objnt\*.obj libnt\cv.lib
	del exe\objnt\*.obj libnt\exe.lib
	del common\objnt\*.obj libnt\common.lib
	del subs\objnt\*.obj libnt\subs.lib
	del moves\objnt\*.obj libnt\moves.lib
	del alloc\objnt\*.obj libnt\alloc.lib
	del parse\objnt\*.obj libnt\parse.lib
	del install\objnt\*.obj libnt\install.lib
	del overlays\objnt\*.obj libnt\overlays.lib
