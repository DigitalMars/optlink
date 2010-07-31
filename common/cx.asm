; Copyright Doug Huffman
; Route 1 Box 54A
; Lenore, ID 83541

; February 18,1992
; FAX:   208-263-8772
; email: doug@proto.com


comment&
        32 bit Startup code for DOS extender.  This module must be linked
        first since it determines the segment order for all segments in the
        final executable.  This file is specifically designed for the Symantec
        compiler.
&

includelib zlx.lod              ;stub loader

.386

public __x386_zero_base_ptr,__x386_zero_base_selector,__data_bottom,__x386_init
public __x386_break,__heapbottom,__pastdata,__acrtused
public __dosx_zero_base_selector,__exe_type,__dosx_zero_base_ptr,__x386_version
public _sbrk,__x32_zero_base_ptr,__x32_zero_base_selector,__x386_stacklow
;public __x386_malloc,__x386_free

;define 16 bit segments first so they will be located at the beginning
;of the executable.

__X386_DATASEG_16      segment para use16 public 'DATA16'
;16 bit data segment for real mode and priv level 0 activities
        externdef __x386_dbg_hook:word
        dw      _TEXT           ;first word in extender code, used by loader
        dw      offset __x386_dbg_hook  ;debugger hook
__X386_DATASEG_16      ends


__X386_CODESEG_16       segment para use16 public 'CODE'
assume cs:__X386_CODESEG_16,ds:nothing,es:nothing,fs:nothing,gs:nothing,ss:nothing
extrn __x386_start:near,__x32_link_in_x32_lib:near
__start:
        jmp     __x386_start    ;jump to 16 bit initialization
__X386_CODESEG_16       ends

_TEXT   segment para public 'CODE' ;must be paragraph aligned here

externdef _malloc:near,_free:near
externdef __exit:near,_exit:near,_main:near,__cinit:near

_TEXT   ends

_DATA   segment dword   public  'DATA'

externdef __stack:dword,__psp:dword,__osmajor:byte,__osminor:byte,_errno:dword
externdef __argc:dword,__argv:dword,__envptr:dword,__x386_data_16_alias:word
externdef __8087:dword,__baslnk:dword

DGROUP  group   _TEXT,_DATA,_BSS,HUGE_BSS
__acrtused      equ     1234    ;cause linker to pull in this module.

__data_bottom   label byte
;CAUTION!!!, the variables __pastdata and __x386_baslnk must be located
;immediately after __x386_stacklow for compatibility with the coreleft
;function.
__x386_stacklow label   dword   ;used by call back things
__x386_break    dd      ?       ;used by alloca
__pastdata      dd      offset DGROUP:EEND      ;address of 1 past data segment
__x386_baslnk   dd      offset DGROUP:__baslnk  ;used by coreleft
__heapbottom    dd      offset DGROUP:EEND      ;lowest address in heap (used
                                ;to detect free() errors and stack wrap)
__stk_param_ptr dd      ?       ;pointer to stack size parameter in cmd strg

;__ss            dw      ?       ;storage for default SS

;debugger requires two variables below to be positioned together
__x32_zero_base_selector        label   word
__x386_zero_base_selector       label   word    ;for compatibility with V3.0
__dosx_zero_base_selector       dw      34h     ;writable segment, base = 0
__x32_zero_base_ptr             label   dword
__x386_zero_base_ptr            label   dword   ;for compatibility with V3.0
__dosx_zero_base_ptr            dd      0f0000000h
;__x386_malloc                   dd      offset _malloc
;__x386_free                     dd      offset _free

__x386_disp_base        dw      0               ;storage for _dispbase
__x386_version          dw      310h            ;DOSX version #
__exe_type              dw      10h             ;Set exe file type

null_code_ptr   db      13,10,'NULL code pointer called',13,10,24h
nomem           db      13,10,'Not enough memory',13,10,24h
stk_overflow    db      13,10,'Stack Overflow',13,10,24h

_DATA   ends

;now define the order of the remaining segments
XIB     segment dword public 'DATA'
XIB     ends
XI      segment dword public 'DATA'
XI      ends
XIE     segment dword public 'DATA'
XIE     ends
XCB     segment dword public 'DATA'
XCB     ends
XC      segment dword public 'DATA'
XC      ends
XCE     segment dword public 'DATA'
XCE     ends
CONST   segment dword public 'CONST'
CONST   ends
_BSS    segment dword public 'BSS'
_BSS    ends
HUGE_BSS        segment dword public 'HUGE_BSS'
HUGE_BSS        ends
EEND    segment para public 'ENDBSS'
EEND    ends

_TEXT   segment para public 'CODE'

if @Version LT 600
        assume  CS:_TEXT
endif
        assume  DS:DGROUP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;This is the very beginning of the 32 bit code segment.
;It must be paragraph aligned so that the offset of the first instruction
;will be zero.  If the address immediately below is ever called, it will be
;due to an null code pointer.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        push    ebp
        mov     ebp,esp
        mov     edx,offset DGROUP:null_code_ptr
        jmp     prn_msg          ;exit with error message
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;This is the location where 16 bit code transfers control to when
;first entering 32 bit code.  DS will = ES = __x386_data_32, GS =
;__x386_seg_env, FS = __x386_seg_psp.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

__x386_init:
        mov     ebx,ds
        mov     ecx,offset DGROUP:__x386_init[-1]     ;pointer to bottom of code
        mov     ax,3505h
        int     21h             ;set ds limit

;        mov     __ss,ss
        mov     ebx,gs                   ;environment segment
        mov     ax,3504h                ;get address of selector in bx
        int     21h                     ;returns address in ecx
        add     ecx,__x386_zero_base_ptr ;convert to DGROUP relative
        mov     __envptr,ecx            ;Save this for now.

        mov     ebx,fs                   ;environment segment
        mov     ax,3504h                ;get address of selector in bx
        int     21h                     ;returns address in ecx
        add     ecx,__x386_zero_base_ptr ;convert to DGROUP relative
        mov     __psp,ecx
;initialize _BSS
        mov     ecx,offset DGROUP:EEND  ;end of _BSS
        mov     edi,offset DGROUP:_BSS  ;start of _BSS is in es:edi
        sub     ecx,edi                 ;size in bytes of _BSS
        shr     ecx,2                   ;size in dwords of _BSS
        xor     eax,eax                 ;zero the block
;EEND must be dword aligned for this to work
        cld
        rep     stosd

;reserve space for stack

        mov     edx,__stack
        mov     ecx,16*1024
        cmp     ecx,edx
        ja short s1
        mov     ecx,edx                 ;use the largest of __stack or 16k
s1:     mov     ax,350dh                ;setup stack
        mov     ebx,esp
;call with ecx = desired size of stack, ebx = present esp value
;returns new esp in ebx and recommended limit for ss in edx
        int     21h
        jc      insuf_mem
        mov     esp,ebx                 ;switch to new stack
        mov     ecx,edx                 ;new limit for ss
        mov     ebx,ss
        mov     ax,3505h                ;set selector limit

        int     21h
;        mov     ah,30h                  ;get DOS version number
;        int     21h
;        mov     __osminor,ah
;        mov     __osmajor,al

;        call    __x386_coproc_init      ;initialize coprocessor
        xor     eax,eax
        mov     fs,eax
        mov     gs,eax                  ;make mode switches faster
        push    eax                     ;ebp value for last stack frame
        mov     ebp,esp

;        call    init_envp               ;Malloc space for environment.

        call    _main           ;call _main(__argc,__argv)
;        push    eax             ;push exit status
;        call    _exit
;no return from _exit

insuf_mem:
        mov     edx,offset DGROUP:nomem

prn_msg:
        mov     ah,9
        int     21h
        push    1               ;error exit code
        call    __exit          ;abort

comment&
        Sbrk is optimized for use with the Zortech libraries.  __pastdata is
        used by Zortech libraries to test for a corrupted heap.  __pastdata
        should always be above the highest block of memory in the pool.  A
        peculiarity of Zortech's sbrk is that the actual size allocated may be
        increased above requested and if so the actual size is returned on the
        stack.  Sbrk is normally called with a request for a minimum of
        16kbytes.

 Allocate a block of memory from the operating system.

 sbrk(nbytes);  nbytes = number of bytes to allocate
 Returns:
       near pointer to memory allocated in eax
       -1 in eax if error
       size of memory block allocated on stack
&

_sbrk   proc    near
        mov     ecx,ss:[esp+4]  ;requested block size
        neg     ecx
        and     cx,0f000h       ;round up to nearest 4k boundary
        neg     ecx
        mov     ax,350ch        ;allocate memory
;function below allocates a block of memory of size = ecx.  If successful,
;CF = 0, eax points to new block, edx is new lowest legal value for esp.  If
;unsuccessful, CF = 1.
        int     21h
        jc short sbrk_err               ;jmp if insufficient memory
        mov     __x386_break,edx
        mov     ss:[esp+4],ecx          ;return actual size of block
        add     ecx,eax                 ;point to top of block
        cmp     ecx,__pastdata
        jb short sbrk_ret
;__pastdata should be top most legal address on heap
        mov     __pastdata,ecx
sbrk_ret:
        ret

sbrk_err:
        sbb     eax,eax
        ret
_sbrk   endp


_TEXT   ends

end     __start

