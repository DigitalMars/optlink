
#include "all.h"

unsigned CV_INDEX_PTR_LIMIT = 0;
unsigned CV_INDEX_BLOCK_END = 0;

void _write_cv_index(unsigned, unsigned);

void _handle_cv_index(unsigned EAX, unsigned ECX)
{
        // EAX IS OLD BYTES_SO_FAR
        // ECX IS INDEX TYPE

        unsigned EDX = ECX;
        ECX = BYTES_SO_FAR;
        BYTES_SO_FAR = EAX;
        _write_cv_index(EDX, ECX - EAX);
        BYTES_SO_FAR = ECX;
}

/*
WRITE_CV_INDEX  PROC
                ;
                ;EAX IS INDEX TYPE, ECX IS LENGTH
                ;
                PUSH    EBX
                MOV     EBX,CV_INDEX_COUNT

                MOV     EDX,CV_INDEX_PTR                ;LOCKED POINTER
                INC     EBX

                MOV     CV_INDEX_COUNT,EBX
                MOV     EBX,CV_INDEX_PTR_LIMIT

                CMP     EBX,EDX
                JB      L1$

L3$:
                MOV     EBX,CURNMOD_NUMBER
                ; seg fault here <<>>
                MOV     [EDX+8],ECX

                SHL     EBX,16
                MOV     ECX,BYTES_SO_FAR

                OR      EAX,EBX
                MOV     [EDX+4],ECX

                MOV     [EDX],EAX
                ADD     EDX,12

                GETT    AL,CV_4_TYPE
                MOV     CV_INDEX_PTR,EDX

                OR      AL,AL
                JZ      L35$
L36$:
                POP     EBX
                RET

L35$:
                MOV     EAX,[EDX-4]
                SUB     EDX,2

                CMP     EAX,64K
                MOV     CV_INDEX_PTR,EDX

                POP     EBX
                JAE     L37$

                RET

L37$:
                MOV     AX,CV_TOO_MANY_ERR
                CALL    ERR_RET

                RET

L1$:
                ;
                ;WON'T ALL FIT IN BLOCK, SO BUFFER IT IN CV_INDEX_TEMP
                ;
                MOV     EBX,OFF L2$
                PUSH    EDX

                PUSH    EBX                     ;PUSH THROUGH DI IF NECESSARY
                PUSH    EBX                     ;GARBAGE TO POP BEFORE RETURN

                MOV     EDX,OFF CV_INDEX_TEMP
                JMP     L3$

L2$:
                ;
                ;MOVE AS MANY AS WILL FIT TO ORIGINAL BLOCK
                ;
                POP     EDX
                MOV     ECX,CV_INDEX_BLOCK_END

                PUSH    EDI
                SUB     ECX,EDX                 ;# OF BYTES IN FIRST BLOCK

                SHR     ECX,1                   ;# OF WORDS
                PUSH    ESI

                MOV     ESI,OFF CV_INDEX_TEMP   ;INDEX TEMP STORAGE
                MOV     EDX,ECX

                REP     MOVSW

                PUSH    EDX
                CALL    CV_INDEX_ANOTHER_BLOCK

                POP     EDX
                MOV     ECX,CV_INDEX_SIZE       ;# OF WORDS IN AN INDEX ENTRY
                SHR     ECX,1
                SUB     ECX,EDX                 ;MINUS WORDS ALREADY MOVED
                REP     MOVSW
                MOV     CV_INDEX_PTR,EDI
                POPM    ESI,EDI,EBX

                RET

WRITE_CV_INDEX  ENDP


CV_INDEX_ANOTHER_BLOCK  PROC
                ;
                ;
                ;
                MOV     EDX,CV_INDEX_BLK_PTR
                CALL    GET_NEW_LOG_BLK         ;LEAVE IN FASTER MEMORY

                MOV     [EDX],EAX
                ADD     EDX,4

                MOV     EDI,EAX
                ADD     EAX,PAGE_SIZE

                MOV     CV_INDEX_BLK_PTR,EDX
                MOV     CV_INDEX_BLOCK_END,EAX

                SUB     EAX,CV_INDEX_SIZE
                MOV     CV_INDEX_PTR,EDI

                MOV     CV_INDEX_PTR_LIMIT,EAX

                RET

CV_INDEX_ANOTHER_BLOCK  ENDP


FLUSH_CV_INDEXES        PROC
                ;
                ;WRITE OUT BUFFERED INDEX ENTRIES
                ;
                ;FIRST, STORE TOTAL SIZE OF INFORMATION
                ;
                CALL    CV_DWORD_ALIGN

                MOV     EDX,DPTR CV_HEADER
                MOV     EAX,CV_INDEX_COUNT

                MOV     DPTR CV_INDEX_TEMP,EDX
                MOV     EDX,CV_INDEX_SIZE

                MUL     EDX

                MOV     EDX,CV_INDEX_HDR_SIZE
                ADD     EAX,8                   ;SIZE OF THIS SIGNATURE

                ADD     EAX,EDX
                MOV     EDX,BYTES_SO_FAR

                ADD     EAX,EDX

                MOV     DPTR CV_INDEX_TEMP+4,EAX

                MOV     EDX,CV_INDEX_PTR
                MOV     ECX,CV_INDEX_BLOCK_END

                MOV     EAX,DPTR CV_INDEX_TEMP
                SUB     ECX,EDX                 ;ROOM LEFT IN BUFFER

                CMP     ECX,8
                JB      L5$

                MOV     ECX,DPTR CV_INDEX_TEMP+4
                MOV     [EDX],EAX

                MOV     [EDX+4],ECX
                ADD     EDX,8
L6$:
                MOV     CV_INDEX_PTR,EDX
L1$:
                ;
                ;NEED TO FLUSH INDEX BUFFERS...
                ;
                PUSH    EBX
                MOV     EBX,OFF CV_INDEX_TABLE
L2$:
                MOV     EAX,DPTR [EBX+4]
                MOV     ECX,PAGE_SIZE           ;WRITE FULL-SIZED BLOCK

                TEST    EAX,EAX                 ;LAST BLOCK?
                JZ      L3$

                CALL    WRITE_INDEX_BLOCK

                ADD     EBX,4
                JMP     L2$

L3$:
                ;
                ;NOW OUTPUT PARTIAL BLOCK
                ;
                MOV     EDX,CV_INDEX_BLOCK_END
                MOV     ECX,CV_INDEX_PTR

                SUB     EDX,PAGE_SIZE

                SUB     ECX,EDX
                JZ      L4$

                CALL    WRITE_INDEX_BLOCK
L4$:
                POP     EBX

                RET

L5$:
                PUSHM   EDI,ESI

                MOV     EDI,EDX
                MOV     EDX,8

                MOV     ESI,OFF CV_INDEX_TEMP
                SUB     EDX,ECX

                REP     MOVSB

                PUSH    EDX
                CALL    CV_INDEX_ANOTHER_BLOCK

                POP     ECX

                REP     MOVSB

                MOV     EDX,EDI
                JMP     L6$

FLUSH_CV_INDEXES        ENDP


WRITE_INDEX_BLOCK       PROC    NEAR
                ;
                ;BX POINTS TO BLOCK
                ;CX IS BLOCK SIZE, WRITE IT OUT
                ;
                CALL    CONVERT_SUBBX_TO_EAX

                push    ECX
                push    EAX
                call    _xdebug_normal
                add     ESP,8

                XOR     ECX,ECX
                MOV     EAX,[EBX]

                MOV     [EBX],ECX
                JMP     RELEASE_BLOCK

WRITE_INDEX_BLOCK       ENDP


                .DATA?



                END

*/
