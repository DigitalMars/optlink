
#include "all.h"

void *CV_INDEX_PTR_LIMIT = 0;
void *CV_INDEX_BLOCK_END = 0;
extern unsigned CV_INDEX_SIZE;

void _write_cv_index(unsigned, unsigned, void *);
void *_cv_index_another_block();

void _handle_cv_index(unsigned EAX, unsigned ECX, void *EDI)
{
        // EAX IS OLD BYTES_SO_FAR
        // ECX IS INDEX TYPE

        unsigned EDX = ECX;
        ECX = BYTES_SO_FAR;
        BYTES_SO_FAR = EAX;
        _write_cv_index(EDX, ECX - EAX, EDI);
        BYTES_SO_FAR = ECX;
}

void _write_cv_index(unsigned EAX, unsigned ECX, void *EDI)
{
        // EAX IS INDEX TYPE, ECX IS LENGTH
        int overflow = 0;
        ++CV_INDEX_COUNT;

        void *EDX = CV_INDEX_PTR;               // LOCKED POINTER
        void *EBX = CV_INDEX_PTR_LIMIT;
        void *EDXsave;
        if (EBX < EDX)
        {
            // WON'T ALL FIT IN BLOCK, SO BUFFER IT IN CV_INDEX_TEMP
            EDXsave = EDX;
            EDX = &CV_INDEX_TEMP[0];
            overflow = 1;
        }

        ((unsigned *)EDX)[2] = ECX;             // seg fault here <<>>
        ((unsigned *)EDX)[1] = BYTES_SO_FAR;
        ((unsigned *)EDX)[0] = EAX | (CURNMOD_NUMBER << 16);
        EDX = (void *)((unsigned *)EDX + 3);

        CV_INDEX_PTR = EDX;

        if (!CV_4_TYPE)
        {
            CV_INDEX_PTR = (void *)((char *)EDX - 2);
            if (((unsigned *)EDX)[-1] >= 64 * 1024)
                _err_ret(CV_TOO_MANY_ERR);
        }
        if (overflow)
        {
            // MOVE AS MANY AS WILL FIT TO ORIGINAL BLOCK
            unsigned words = ((char *)CV_INDEX_BLOCK_END - (char *)EDXsave) >> 1;       // # OF WORDS IN FIRST BLOCK

            unsigned char *ESI = &CV_INDEX_TEMP[0];             // INDEX TEMP STORAGE

            memcpy(EDXsave, ESI, words * 2);
            ESI += words * 2;

            EDI = _cv_index_another_block();

            words = (CV_INDEX_SIZE >> 1) - words;       // # OF WORDS IN AN INDEX ENTRY
                                                    // MINUS WORDS ALREADY MOVED

            memcpy(EDI, ESI, words * 2);
            CV_INDEX_PTR = (void *)((char *)EDI + words * 2);
        }
}


void *_cv_index_another_block()
{
        void **EDX = CV_INDEX_BLK_PTR;
        void *EAX = _get_new_log_blk(); // LEAVE IN FASTER MEMORY

        *EDX = EAX;
        ++EDX;

        void *EDI = EAX;
        EAX = (void *)((char *)EAX + PAGE_SIZE);

        CV_INDEX_BLK_PTR = EDX;
        CV_INDEX_BLOCK_END = EAX;

        CV_INDEX_PTR = EDI;
        CV_INDEX_PTR_LIMIT = (void *)((char *)EAX - CV_INDEX_SIZE);
        return EDI;
}

/*
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
*/


void _write_index_block(void **EBX, unsigned ECX)
{
        // EBX POINTS TO BLOCK
        // ECX IS BLOCK SIZE, WRITE IT OUT

        _xdebug_normal(_convert_subbx_to_eax(EBX), ECX);

        void *EAX = *EBX;
        *EBX = 0;
        _release_block(EAX);
}

