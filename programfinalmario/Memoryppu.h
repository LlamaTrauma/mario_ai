#ifndef NESEMU_MEMORYPPU_H
#define NESEMU_MEMORYPPU_H
#include "helper.h"
#include "Memory.h"

class Memoryppu : public Memory {
public:
    dword MEMSIZE = 0x4000;
    byte data[0x4000];
    void reset();
};


#endif //NESEMU_MEMORY_H
