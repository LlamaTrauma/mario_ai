#include "CPUBus.h"

byte CPUBus::cpuRead (const int index) {
    assert(index < MEMSIZE);
    assert(index >= 0);
    if(index < 0x2000) {
        //std::cout << "Read from memory at " << index % 0x800 << '\n';
        return mem->data[index % 0x800];
    } else if(index < 0x4000){
        //std::cout << "Read from memory at " << index % 0x4000 << '\n';
        word index2 = 0x2000 + index % 0x0008;
        /*if(index2 != 0x2002){
            std::cout << "read: " << std::hex << int(index2) << std::dec << '\n';
        }*/
        if(index2 == 0x2000){
            return 0;
        } else if(index2 == 0x2001){
            return 0;
        } else if(index2 == 0x2002){
            return ppu->getPPUSTATUS();
        } else if(index2 == 0x2003){
            return 0;
        } else if(index2 == 0x2004){
            return ppu->getOAMDATA();
        } else if(index2 == 0x2005){
            return 0;
        } else if(index2 == 0x2006){
            return 0;
        } else if(index2 == 0x2007){
            return ppu->getPPUDATA();
        } 
    } else if(index < 0x4020){
        if(index == 0x4014){
            return 0;
        } else if(index == 0x4016){
            return player1->returnButton();
        } else if(index == 0x4017){
            //return player1->returnButton();
            return 0;
        }else {
            return mem->data[index];
        }
    } else if (index >= 0x8000){
        //This is hacky, only for 16kb nrom like Balloon Fight rn
        //std::cout << "Read from prgrom at " << index % 0x4000 << '\n';
        word mod = (word) 0x4000 * (rom->prgromlen);
        return rom->prgrom[index % mod];
    } else {
        //std::cout << "Oh no a weird memory read thing happened\n";
    }
}

void CPUBus::cpuWrite (const int index, byte val) {
    //std::cout << "write to " << std::hex << index << std::dec << '\n';
    assert(index < MEMSIZE);
    assert(index >= 0);
    if(index < 0x2000) {
        mem->data[(index % 0x800)] = val;
    } else if(index < 0x4000){
        word index2 = 0x2000 + index % 0x0008;
        //std::cout << "write: " << std::hex << int(index2) << std::dec << '\n';
        if(index2 == 0x2000){
            ppu->setPPUCTRL(val);
        } else if(index2 == 0x2001){
            ppu->setPPUMASK(val);
        } else if(index2 == 0x2002){
            return;
        } else if(index2 == 0x2003){
            ppu->setOAMADDR(val);
        } else if(index2 == 0x2004){
            ppu->setOAMDATA(val);
        } else if(index2 == 0x2005){
            ppu->setPPUSCROLL(val);
        } else if(index2 == 0x2006){
            ppu->setPPUADDR(val);
        } else if(index2 == 0x2007){
            ppu->setPPUDATA(val);
        } 
    } else if(index < 0x4020){
        if(index == 0x4014){
            ppu->doOAMDMA(val);
        } else if(index == 0x4016){
            player1->resetRead();
        } else {
            mem->data[index] = val;
        }
    } else if (index >= 0x8000){
        //This is hacky, only for 16kb nrom like Balloon Fight rn
        rom->prgrom[index % ((word) 0x4000 * (rom->prgromlen))] = val;
    }
}