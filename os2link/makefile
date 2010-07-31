


default : ..\COMMON\RELEASE nt


..\COMMON\RELEASE	: RELEASE
	copy RELEASE ..\COMMON

nt :
	nmake -nologo -f optdll.mak HOST_WIN32=1

