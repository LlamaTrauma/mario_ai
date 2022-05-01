#ifndef PPUBUS_CPP
#define PPUBUS_CPP

#include "PPUBus.h"

byte PPUBus::ppuRead (const int index) {
    //std::cout << "read from " << index << '\n';
    assert(index < MEMSIZE);
    assert(index >= 0);
    if(index < 0x1000) {
        //std::cout << "Read from memory at " << index % 0x800 << '\n';
        return rom->chrrom[index];
    } else if (index < 0x2000){
        //std::cout << "Read from PPU2C02 register at " << 0x2000 + (index % 0x8) << '\n';
        return rom->chrrom[index];
    } else if(index < 0x3000){
        //Hard-coded vertical mirroring
        return mem->data[0x2000 + (index % 0x800)];
    }else if(index < 0x3F00){
        return mem->data[0x2000 + (index % 0x800)];
    } else if (index < 0x4000){
        //This is hacky, only for 16kb nrom like Balloon Fight rn
        //std::cout << "Read from prgrom at " << index % 0x4000 << '\n';
        if(index == 0x3F10 || index == 0x3F14 || index == 0x3F18 || index == 0x3F1C){
            //Mirrors
            return mem->data[index - 0x10];    
        }
        return mem->data[index];
    } else {
        std::cout << "Oh no a weird memory read thing happened\n";
    }
}

void PPUBus::ppuWrite (const int index, byte val) {
    //std::cout << "write to " << std::hex << index << " val " << int(val) << '\n';
    //numWrites += 1;
    assert(index < MEMSIZE);
    assert(index >= 0);
    if(index < 0x1000) {
        //std::cout << "Read from memory at " << index % 0x800 << '\n';
        rom->chrrom[index] = val;
    } else if (index < 0x2000){
        //std::cout << "write to PPU2C02 register at " << index << '\n';
        rom->chrrom[index] = val;
    } else if(index < 0x3000){
        //std::cout << "Write to PPU val " << int(val) << " at " << std::hex << index << '\n';
        //Hard coded vertical mirroring
        mem->data[0x2000 + (index % 0x800)] = val;
    } else if(index < 0x3F00){
        word index2 = index - 0x1000;
        mem->data[0x2000 + index2 % 0x800] = val;
    } else if (index < 0x4000){
        if(index == 0x3F10 || index == 0x3F14 || index == 0x3F18 || index == 0x3F1C){
            //Mirrors
            mem->data[index - 0x10] = val;    
        } else {
            mem->data[index] = val;
        }
    } else {
        std::cout << "Oh no a weird memory read thing happened\n";
    }
}

byte PPUBus::cpuRead (const int index) {
    //std::cout << "read from " << index << '\n';
    assert(index < MEMSIZE);
    assert(index >= 0);
    if(index < 0x2000) {
        //std::cout << "Read from memory at " << index % 0x800 << '\n';
        return cpumem->data[index % 0x800];
    } else if (index < 0x4000){
        //std::cout << "Read from PPU2C02 register at " << 0x2000 + (index % 0x8) << '\n';
        return cpumem->data[0x2000 + (index % 0x8)];
    } else if(index < 0x8000){
        //std::cout << "Read from memory at " << index % 0x4000 << '\n';
        return mem->data[index];
    } else if (index >= 0x8000){
        //This is hacky, only for 16kb nrom like Balloon Fight rn
        //std::cout << "Read from prgrom at " << index % 0x4000 << '\n';
        word mod = (word) 0x4000 * (rom->prgromlen);
        return rom->prgrom[index % mod];
    } else {
        //std::cout << "Oh no a weird memory read thing happened\n";
    }
}

#endif