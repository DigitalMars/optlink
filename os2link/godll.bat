@ECHO OFF
COPY	RELEASE ..\COMMON
NMAKE OS2LINK.MAK HOST_WIN32DLL=
IF ERRORLEVEL 1 GOTO END
CD..\DLLSTUFF
CALL GO.BAT HOST_WIN32DLL
IF ERRORLEVEL 1 GOTO END
CD..\CV
CALL GO.BAT HOST_WIN32DLL
IF ERRORLEVEL 1 GOTO END
CD..\EXE
CALL GO.BAT HOST_WIN32DLL
IF ERRORLEVEL 1 GOTO END
CD..\COMMON
CALL GO.BAT HOST_WIN32DLL
IF ERRORLEVEL 1 GOTO END
CD..\SUBS
CALL GO.BAT HOST_WIN32DLL
IF ERRORLEVEL 1 GOTO END
CD..\MOVES
CALL GO.BAT HOST_WIN32DLL
IF ERRORLEVEL 1 GOTO END
CD..\ALLOC
CALL GO.BAT HOST_WIN32DLL
IF ERRORLEVEL 1 GOTO END
CD..\PARSE
CALL GO.BAT HOST_WIN32DLL
IF ERRORLEVEL 1 GOTO END
CD..\INSTALL
CALL GO.BAT HOST_WIN32DLL
IF ERRORLEVEL 1 GOTO END
CD..\NTIO
CALL GO.BAT HOST_WIN32DLL
IF ERRORLEVEL 1 GOTO END
CD..\OVERLAYS
CALL GO.BAT HOST_WIN32DLL
IF ERRORLEVEL 1 GOTO END
CD..\OS2LINK
CALL LWINDLL.BAT

:END
CD..\OS2LINK


