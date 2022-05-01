#ifndef NESEMU_MEMORY_H
#define NESEMU_MEMORY_H
#include "helper.h"

class Memory {
public:
    dword MEMSIZE = 0x10000;
    byte data[0x10000];
    void reset();
};


#endif //NESEMU_MEMORY_H
