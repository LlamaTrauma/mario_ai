#ifndef NESEMU_PPUBUS_H
#define NESEMU_PPUBUS_H

#include "Nesrom.h"
#include "Memory.h"
#include "helper.h"

using ushort = unsigned short;

struct PPUBus {
public:
    static const dword MEMSIZE = 0x10000;
    Nesrom* rom;
    Memory* mem;
    Memory* cpumem;
    int numWrites = 0;
    /* Read a byte from the data */
    //byte operator[](const int index) const;
    //byte& operator[](const int index);
    byte ppuRead(int index);
    byte cpuRead(int index);
    void ppuWrite(int index, byte val);
};

#endif //NESEMU_PPUBUS_H
