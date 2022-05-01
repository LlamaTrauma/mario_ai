#include "Memory.h"

void Memory::reset(){
    for(dword i = 0; i < MEMSIZE; i ++) {
        data[i] = 0x00;
    }
}