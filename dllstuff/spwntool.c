//_ spwntool.c
// Written by Walter Bright

#include <windows.h>
#include <string.h>
#include <dos.h>
#include <stdlib.h>
#include <time.h>
#include <setjmp.h>
#include "netspawn.h"
#include "dllrun.h"

extern int _argc;
extern char **_argv;

unsigned char *data_start();
unsigned data_size();
unsigned char *bss_start();
unsigned bss_size();

void *_osfhnd[];
int __cdecl _dll_write(int fd,void *buffer, unsigned int length);
void _sbrk_freeall();
void _cinit(void);
extern int (*__write_fp)(int, void *, unsigned int);
void DebugString(char * str);
extern void *_baslnk;
void (* _hookexitp)(int errstatus);
int _acrtused_winc = 1234;
int _acrtused_con  = 1234;
int _version;               // set by app to version number
static jmp_buf catchbuf;
tToolCallbacks _cdecl TaskCallbacks = {0, NULL, NULL, NULL, NULL, NULL, NULL,
				0, 0, 0};
long lLastLineNum; // last line number reported via ReportProgress
static tNetSpawnTranslateCallback DoTranslate = NULL;
static tNetSpawnDisposeCallback DoDispose = NULL;
static BOOL callbacks_initialized = FALSE;
 
//////////////////////////////////
// Entry point to the DLL.
// Called by DLLSTART.ASM.
// Returns:
//  0   error
//  !=0 success

BOOL WINAPI LibEntry (HANDLE hDLL, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason) 
    {
        case DLL_PROCESS_ATTACH:
           _clearbss();  // Clear uninitialized Data (needed under Chicago)
           break;  
        case  DLL_PROCESS_DETACH:
           break;
    }          
    return TRUE;
}
#pragma startaddress(LibEntry)

//////////////////////////////////
// Return the tool version number.

int WINAPI DllVersion()
{   extern int _version;        // global provided by application

    return _version;
}

//////////////////////////////
// Return DLLSHELL version number

int FAR PASCAL _export NetSpawnVersion(void)
{
    return NETSPAWN_VERSION;
}

//////////////////////////////
// Function we hook exit() with.

static void _dll_exit(int exitstatus)
{
    longjmp (catchbuf, exitstatus + 2);
}

//////////////////////////////
// Application entry point.
// Returns:
//  0   Success
//  1   Failure

static HGLOBAL persistent_handle = 0;
static void *persistent_ptr = 0;
static int persistent_size = 0;
    
int WINAPI DllEntry(int argc, char *argv[], tToolCallbacks *pCallbacks)
{
    int result;
    BOOL early_exit;
    HGLOBAL hmem;
    unsigned char *saved_data;
    int local_active_invocations;
    unsigned char *local_original_data;
    HGLOBAL local_persistent_handle;
    int local_persistent_size;
    void *local_persistent_ptr;
    tToolCallbacks local_callbacks;
    
    static unsigned char *original_data = NULL;
    static int active_invocations = 0;

    if (pCallbacks->version != VERSION_TOOLCALLBACKS) {
        MessageBox(NULL, "Fatal Error: IDE Using Improper Version of DLLSHELL",
                   "Version Control", MB_OK);
        return 1;
    }
    
    local_callbacks = *pCallbacks;
    
    if (original_data == NULL)
    {
       hmem = GlobalAlloc (GMEM_MOVEABLE, data_size());
       if (hmem == NULL) return 1;
       
       original_data = GlobalLock (hmem);
       active_invocations = 0;
       
       memcpy (original_data, data_start(), data_size());
    }
    else
    {
       if (active_invocations)
       {
          hmem = GlobalAlloc (GMEM_MOVEABLE, data_size()+bss_size());
          if (hmem == NULL) return 1;
          saved_data = GlobalLock (hmem);
          memcpy (saved_data, data_start(), data_size());
          memcpy (saved_data+data_size(), bss_start(), bss_size());
       }
       local_active_invocations = active_invocations;
       local_original_data = original_data;
       local_persistent_handle = persistent_handle;
       local_persistent_size = persistent_size;
       local_persistent_ptr = persistent_ptr;
       memcpy (data_start(), original_data, data_size());
       memset (bss_start(), 0, bss_size());
       persistent_ptr = local_persistent_ptr;
       persistent_size = local_persistent_size;
       persistent_handle = local_persistent_handle;
       original_data = local_original_data;
       active_invocations = local_active_invocations;
    }
   
    __write_fp = _dll_write;

    _argc = argc;
    _argv = argv;
    
    _hookexitp = _dll_exit;
    TaskCallbacks = local_callbacks;
    
    early_exit = setjmp (catchbuf);
    if (early_exit) result = early_exit - 2;
    else
    {
       active_invocations ++;
       _allocinit();
       _thread_init();
       _cinit();
       result = main(argc,argv);
    }

    active_invocations --;
    
    if (result >= 0) // skip these calls for fatal error
    {
       early_exit = setjmp (catchbuf); // catch any _exits in dtors
       if (!early_exit) 
          _dodtors();
       fcloseall();
    }   
    _allocwrap();	    // clean up allocator
    _sbrk_freeall();        // dump all global data

    if (active_invocations)
    {    
       memcpy (data_start(), saved_data, data_size());
       memcpy (bss_start(), saved_data+data_size(), bss_size());
       GlobalUnlock(hmem);
       GlobalFree(hmem);
    }

    return result;
}

/////////////////////////////////
// _flushterm() and _fillbuf() call sleep(), so that
// WINC programs will yield.
// We preemt the calls by providing our own, redirecting it to dllshell_yield().

void sleep(time_t t)
{
    dllshell_yield();
}

/////////////////////////////////
//  isatty must return TRUE for stdout and stderr
//  (since these files are not really open or available, but only redirected
//  here by the RTL, we must not call NT to ask this question (it will say
//  NO, and then stdout&stderr will never get their buffers flushed).

int isatty(int fd)
{
   switch (fd)
   {
   case 1:
   case 2:
      return TRUE;
   default:
      return (GetFileType (_osfhnd[fd]) == FILE_TYPE_CHAR);
   }
}

void __faterr (char *msg_text)
{
   tToolMsg msg;
   
   if (strnicmp (msg_text+2, "Thread error", 12) == 0) // these are RTL errors
      msg_text = "Thread error (out of memory?)";
      
   if (strnicmp (msg_text+2, "Semaphore error", 15) == 0)
      msg_text = "Semaphore error (out of memory?)";
   
   if (TaskCallbacks.ReportMessage)
   {
      msg.version = TOOLMSG_VERSION;
      msg.msgText = msg_text;
      msg.msgType = eMsgFatalError;
      msg.fileName = "";
      msg.lineNumber = kNoLineNumber;
      msg.colNumber = kNoColNumber;
      msg.msgNumber = kNoMsgNumber;
      (*TaskCallbacks.ReportMessage)(&msg);
   }   
   longjmp (catchbuf, 1);
}

void _exit (int exitstatus)
{
   longjmp (catchbuf, exitstatus + 2);
}

/////////////////////////////////
// Output to stdout and stderr is caught and routed through here.
// Returns:
//  -1  error
//  0   end of file
//  n   number of bytes actually read/written

int __cdecl _dll_write(int fd,void *buffer, unsigned int length)
{   char *p;

    // If no callback for output, ignore it
    if (!TaskCallbacks.ReportOutput)
        return length;

    if (length)
    {
    p = (char *) calloc(length + 1,1);
    if (p)
    {   memcpy(p,buffer,length);
        (*TaskCallbacks.ReportOutput)(p);
        free(p);
    }
    else
    {   char c;

        // Out of memory. Clumsilly hack off last byte to 0-terminate.
        p = (char *)&buffer + length - 1;
        c = *p;
        *p = 0;
        (*TaskCallbacks.ReportOutput)(buffer);
        *p = c;
    }
    }
    return length;
}

void * NETSPAWNAPI NetSpawnPersistentAlloc (int size)
{
   if (persistent_handle == 0)
   {
      persistent_handle = GlobalAlloc (GMEM_ZEROINIT, size);
      persistent_ptr = GlobalLock (persistent_handle);
      persistent_size = size;
   }
   else if (size != persistent_size)
   {
      persistent_handle = GlobalReAlloc (persistent_handle, size, 
                                         GMEM_ZEROINIT);
      persistent_ptr = GlobalLock (persistent_handle);
      persistent_size = size;
   }
   if (persistent_handle == 0)
   {
      persistent_ptr = 0;
      persistent_size = 0;
      return NULL;
   }
   if (persistent_ptr == NULL)
   {
      GlobalFree (persistent_handle);
      persistent_handle = 0;
      persistent_size = 0;
      return NULL;
   }
   return persistent_ptr;
}

NETSPAWNSTATUS NETSPAWNAPI NetSpawnYield()
{
   if (TaskCallbacks.YieldMode != yieldOften)
      return NetSpawnOK;

   // assert TaskCallbacks.ReportProgress != NULL
   
   if ((*TaskCallbacks.ReportProgress)(kNoLineNumber))
      return NetSpawnFail;
   else
      return NetSpawnOK;   
}

NETSPAWNSTATUS NETSPAWNAPI NetSpawnProgress (long linnum)
{
   if (TaskCallbacks.LineReportMode == reportByFile)
   {
      lLastLineNum = linnum;
      if (TaskCallbacks.YieldMode != yieldOften)
         return NetSpawnOK;
   }
   
   // assert TaskCallbacks.ReportProgress != NULL
   
   if ((*TaskCallbacks.ReportProgress)(linnum))
      return NetSpawnFail;
   else
      return NetSpawnOK;   
}

void NETSPAWNAPI NetSpawnMessage(const tToolMsg *p)
{
   if (TaskCallbacks.ReportMessage)
      (*TaskCallbacks.ReportMessage)(p);
}

void NETSPAWNAPI NetSpawnTarget(const char *filename)
{
   if (TaskCallbacks.ReportTarget)
      (*TaskCallbacks.ReportTarget)(filename);
}

void NETSPAWNAPI NetSpawnFile(const char *filename,int includelevel)
{
   if (TaskCallbacks.LineReportMode == reportByFile)
   {
      if (TaskCallbacks.ReportProgress)
         (*TaskCallbacks.ReportProgress)(lLastLineNum);
      lLastLineNum = 0;
   }
   if (TaskCallbacks.ReportFile)
      (*TaskCallbacks.ReportFile)(filename,includelevel);
}

void NETSPAWNAPI NetSpawnActivity(const tToolData *tooldata)
{
   if (TaskCallbacks.ReportActivity)
      (*TaskCallbacks.ReportActivity)(tooldata);
}

void NETSPAWNAPI NetSpawnCmdline1(int argc, char ** argv)
{
    if (TaskCallbacks.ReportMessage && argc) {
        int cmdlineLen, i;
        char *cmdline;

        for (i = 0, cmdlineLen = 0; i < argc; i++)
            cmdlineLen += (strlen(argv[i]) + 1);

        cmdline = malloc(cmdlineLen + 1);
        if (!cmdline)
        {
            NetSpawnCmdline2(argv[0]);
            return;
        }
        else
        {
            cmdline[0] = '\0';

            for (i = 0; i < argc; i++) {
                strcat(cmdline, argv[i]);
                strcat(cmdline, " ");
            }
            NetSpawnCmdline2(cmdline);
            free(cmdline);
        }
    }
}

void NETSPAWNAPI NetSpawnCmdline2(char *cmdline)
{
    if (TaskCallbacks.ReportMessage) {
        tToolMsg m;

        m.version = TOOLMSG_VERSION;
        m.msgText = cmdline;
        m.msgType = eMsgToolCmdLine;
        m.fileName = NULL;
        m.lineNumber = kNoLineNumber;
        m.colNumber = kNoColNumber;
        m.msgNumber = kNoMsgNumber;
        (*TaskCallbacks.ReportMessage)(&m);
    }
}

int dllshell_progress(long linnum)
{
   if (NetSpawnProgress(linnum) == NetSpawnOK)
      return 0;

   return 1;
}

void dllshell_message(const tToolMsg *p)
{
   NetSpawnMessage(p);
}

void dllshell_target(const char *filename)
{
   NetSpawnTarget(filename);
}

void dllshell_file(const char *filename,int includelevel)
{
   NetSpawnFile(filename, includelevel);
}

void dllshell_activity(const tToolData *tooldata)
{
   NetSpawnActivity(tooldata);
}

void dllshell_cmdline1(int argc, char **argv)
{
   NetSpawnCmdline1 (argc, argv);
}

void dllshell_cmdline2(char * cmdline)
{
   NetSpawnCmdline2 (cmdline);
}

int dllshell_yield()
{
   if (NetSpawnYield() == NetSpawnOK)
      return 0;

   return 1;
}

static InitializeCallbacks()
{
   HINSTANCE hSPWNLND;
   tNetSpawnTranslateCallback (*retrieve_translate_callback)();
   tNetSpawnDisposeCallback (*retrieve_dispose_callback)();
   
   hSPWNLND = LoadLibrary ("SPWNLND.DLL");
   (FARPROC) retrieve_translate_callback = 
      GetProcAddress (hSPWNLND, "RetrieveTranslateCallback");
   (FARPROC) retrieve_dispose_callback = 
      GetProcAddress (hSPWNLND, "RetrieveDisposeCallback");
      
   if (retrieve_translate_callback)
      DoTranslate = retrieve_translate_callback();
   
   if (retrieve_dispose_callback)
      DoDispose = retrieve_dispose_callback();
      
   callbacks_initialized = TRUE;
}

char * NETSPAWNAPI NetSpawnTranslateFileName (char *filename, char *mode)
{
   if (!callbacks_initialized)
      InitializeCallbacks();
   
   if (DoTranslate)
      return DoTranslate (filename, mode);
   else
      return filename;
}

void NETSPAWNAPI NetSpawnDisposeFile (char *newfilename)
{
   if (!callbacks_initialized)
      InitializeCallbacks();
   
   if (DoDispose)
      DoDispose (newfilename);
}

unsigned long NETSPAWNAPI NetSpawnGetCompilerFlags()
{
   unsigned long flags;
   flags = GetCompilerFlags();
   SetCompilerFlags (flags & ~(NETSPAWN_FIRST_COMPILE | 
                               NETSPAWN_DUMP_COMPILE_CONTEXT));
   return flags;
}

