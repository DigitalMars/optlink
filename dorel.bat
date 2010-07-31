@echo off
md 750.%1
xcopy /s 750.005 750.%1
cd 750.%1
del *.zip
copy ..\os2link\objdll\LINKND.DLL rel\bin
copy ..\os2link\objnt\LINK.exe rel\bin
copy ..\os2link\relnotes.txt other
zip -j -r B7506 rel\bin\link.exe rel\bin\linknd.dll other\relnotes.txt
cd..
