cd os2link
nmake -nologo -f optdll.mak HOST_WIN32=1 %* OPTLINK
cd ..
