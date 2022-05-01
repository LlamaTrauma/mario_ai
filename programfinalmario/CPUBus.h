#ifndef NESEMU_CPUBUS_H
#define NESEMU_CPUBUS_H

#include "helper.h"
#include "Nesrom.h"
#include "Memory.h"
#include "PPU2C02.h"
#include "Controller.h"

using ushort = unsigned short;

struct CPUBus {
public:
    static const dword MEMSIZE = 0x10000;
    Nesrom* rom;
    Memory* mem;
    PPU2C02* ppu;
    Controller* player1;
    void reset();
    /* Read a byte from the data */
    //byte operator[](const int index) const;
    //byte& operator[](const int index);
    byte cpuRead(int index);
    void cpuWrite(int index, byte val);
};

#endif //NESEMU_CPUBUS_H