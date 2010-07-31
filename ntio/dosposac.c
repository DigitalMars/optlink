#include <windows.h>

#include "all.h"

MYO_STRUCT *_dosposition_a(MYO_STRUCT *EAX, int ECX)
{
    EAX->MYO_SPEC_FLAGS |= F_SEEK_FIRST;
    EAX->MYO_DESPOT = ECX;
    return EAX;
}
