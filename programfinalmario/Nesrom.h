#ifndef NESEMU_NESROM_H
#define NESEMU_NESROM_H
#include "helper.h"
#include <fstream>

class Nesrom {
public:
    std::ifstream romFile;
    char magic[4];
    byte prgromlen;
    byte chrromlen;
    byte flags6;
    byte flags7;
    byte flags8;
    byte flags9;
    byte flags10;
    char padding[6];
    char prgromchar[2 * 16384 + 1];
    char chrromchar[2 * 8192 + 1];
    byte prgrom[2 * 16384 + 1];
    byte chrrom[2 * 8192 + 1];
    Nesrom();
    void read();
};


#endif //NESEMU_NESROM_H
