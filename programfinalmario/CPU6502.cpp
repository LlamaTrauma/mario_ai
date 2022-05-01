#include <bitset>
#include "CPU6502.h"

#define LDA_SET z = (a == 0); \
n = (a & 0b10000000) != 0;

#define LDX_SET z = (x == 0); \
n = (x & 0b10000000) != 0;

#define LDY_SET z = (y == 0); \
n = (y & 0b10000000) != 0;

#define BIT_SET z = (andres == 0); \
v = (andres & 0b01000000) != 0; \
n = (andres & 0b10000000) != 0;

#define ADC_SET word adcres = static_cast<word>(adcval) + a + (c > 0); \
c = (adcres > 0xff); \
v = (!(((a ^ adcval) & 0x80) > 0)) && (((a ^ adcres) & 0x80) > 0);

//#define SBC_SET word sbcres = a - sbcval - (1 - c); \
//c = (sbcres > 0xff); \
//v = ((a ^ sbcres) & 0x80) && ((a ^ sbcval) & 0x80); \

#define SBC_SET sbcval ^= 0xff; \
word sbcres = static_cast<word>(sbcval + a + c); \
c = (sbcres > 0xff); \
v = (!(((a ^ sbcval) & 0x80) > 0)) && (((a ^ sbcres) & 0x80) > 0);

#define CMP_SET c = (cmpval < 0x100 && cmpval >= 0); \
z = (cmpval == 0); \
n = (cmpval & 0x80) != 0;

#define INC_SET z = (incres == 0); \
n = (incres & 0x80) != 0; \
cycles -= 1;

#define ASL_SET_A c = (srcval & 0x80) != 0; \
srcval <<= 1; \
z = (srcval == 0); \
n = (srcval & 0x80) != 0; 

#define ASL_SET byte srcval = readByte(mem, cycles, srcptr); \
ASL_SET_A \
writeByte(mem, cycles, srcptr, srcval);

#define LSR_SET_A c = (srcval & 0x1) != 0; \
srcval >>= 1; \
z = (srcval == 0); \
n = (srcval & 0x80) != 0; \

#define LSR_SET byte srcval = readByte(mem, cycles, srcptr); \
LSR_SET_A \
writeByte(mem, cycles, srcptr, srcval);

#define ROL_SET_A byte holdc = (srcval & 0x80) != 0; \
srcval = (srcval << 1) & 0xFF; \
if(c != 0){ \
    srcval |= 0x1; \
} \
c = holdc; \
z = (srcval == 0); \
n = (srcval & 0x80) != 0; 

#define ROL_SET byte srcval = readByte(mem, cycles, srcptr); \
ROL_SET_A \
writeByte(mem, cycles, srcptr, srcval);

#define ROR_SET_A bool holdc = (srcval & 0x1) != 0; \
srcval >>= 1; \
z = (srcval == 0); \
n = (srcval & 0x80) != 0; \
if(c){ \
srcval |= 0x80; \
} \
c = holdc;

#define ROR_SET byte srcval = readByte(mem, cycles, srcptr); \
ROR_SET_A \
writeByte(mem, cycles, srcptr, srcval);

/*#define BCC_SET //Should handle both cases because of underflow? \
//Decrease cycles by 1 if page crossed \
if((static_cast<word>(pc & 0xff) + addr) > 0xff){ \
    cycles -= 1; \
}*/

#define BCC_SET cycles -= 1;

#define CLC_SET cycles -= 1;

void CPU6502::reset(CPUBus& mem){
    //https://wiki.nesdev.com/w/index.php/Init_code
    isReset = false;
    std::cout.setstate(std::ios_base::failbit);
    byte resetSequence[67] = {0x78, 0xd8, 0xa2, 0x40, 0x8e, 0x17, 0x40, 0xa2, 0xff, 0x9a, 0xe8, 0x8e, 0x00, 0x20, 0x8e, 0x01, 0x20, 0x8e, 0x10, 0x40, 0x2c, 0x02, 0x20, 0x2c, 0x02, 0x20, 0x10, 0xfb, 0x8a, 0x9d, 0x00, 0x00, 0x9d, 0x00, 0x01, 0x9d, 0x00, 0x02, 0x9d, 0x00, 0x03, 0x9d, 0x00, 0x04, 0x9d, 0x00, 0x05, 0x9d, 0x00, 0x06, 0x9d, 0x00, 0x07, 0xe8, 0xd0, 0xe5, 0x2c, 0x02, 0x20, 0x10, 0xfb, 0xea, 0xb0, 0xfd, 0x90, 0xfb};
    int cycles = 310000;
    for(word i = 0; i < 66; i ++){
        writeByte(mem, cycles, i + 0x5000, resetSequence[i]);
    }
    pc = 0x5000;
    //execute(mem, cycles);
    for(word i = 0; i < 66; i ++){
        writeByte(mem, cycles, i + 0x5000, 0x0);
    }
    std::cout.clear();
    isReset = true;
    pc = readWord(mem, RESET_VECT);
    //pc = 0xC000;
    std::cout << "Starting execution at " << pc << '\n';
    sp = 0xFD;
    a = x = y = 0;
    c = z = i = d = b = v = n = 0;
}

void CPU6502::doNMI(CPUBus& mem, int cycles){
    pushSP(mem, cycles, pc);
    pushByteSP(mem, cycles, genStatusFlag());
    i = 1;
    setPC(cycles, readWord(mem, cycles, NMI_VECT));
}

CPU6502::CPU6502(){
    //outFile.open("out.txt");
    //byte a = 0xf7;
    //byte b = 153;
    //std::cout << a % 0xf0;
};

byte CPU6502::genStatusFlag(){
    return c | ((z > 0) << 1) | ((i > 0) << 2) | ((d > 0) << 3) | (1 << 4) | ((b > 0) << 5) | ((v > 0) << 6) | ((n > 0) << 7);
    //return c + z * 2 + i * 4 + d * 8 + b * 32 + v * 64 + n * 128;
}

void CPU6502::setStatusFlag(byte flag){
    c = (flag & (0b00000001)) != 0;
    z = (flag & (0b00000010)) != 0;
    i = (flag & (0b00000100)) != 0;
    d = (flag & (0b00001000)) != 0;
    b = (flag & (0b00100000)) != 0;
    v = (flag & (0b01000000)) != 0;
    n = (flag & (0b10000000)) != 0;
    //std::cout << "Flag: " << int(genStatusFlag()) << '\n';
}

void CPU6502::clearInstruction(){
    //outFile.open("out.txt", std::fstream::app);
    //std::cout << std::hex;
    //outFile << std::hex;
    if(isReset && stepping) {
        //numWrites += 1;
        //std::cout << int(readByte(0x600 + x)) << '\n';
        for (int i = 0; i < instructionLength; i++) {
            std::cout << (int) thisInstruction[i] << ' ';
            //outFile << (int) thisInstruction[i] << ' ';
        }
        std::cout << '\n';
        std::cout << '\n' << "STACK (desc): ";
        //outFile << '\n' << "STACK (desc): ";
        for(int i = 0xFF; i >= sp; i --){
            std::cout << int(readByte(0x100 + i)) << ' ';
            //outFile << int(readByte(0x100 + i)) << ' ';
        }
        std::cout << '\n';
        //outFile << '\n';
        /*bool toadd = true;
        for(byte j = 0; j < numInstructionsRun; j ++){
            if(instructionsRun[j] == thisInstruction[0]){
                toadd = false;
                break;
            }
        }
        if (toadd){
            instructionsRun[numInstructionsRun] = thisInstruction[0];
            numInstructionsRun += 1;
            std::cout << std::dec << int(numInstructionsRun) << ": " << std::hex;
            for(byte j = 0; j < numInstructionsRun; j ++){
                std::cout << int(instructionsRun[j]) << ' ';
            }
            std::cout << std::dec << '\n';
        }*/
        std::cout << std::hex << "SP is now " << int(sp) << '\n';
        std::cout << "PC is now " << pc << '\n' << '\n';
        //outFile << std::hex << "SP is now " << int(sp) << '\n';
        //outFile << "PC is now " << pc << '\n' << '\n';
    }
    //outFile.close();
    /*for(int i = 0; i < instructionLength; i ++){
        pastInstructions[pastPosition][i] = thisInstruction[i];
    }
    pastLengths[pastPosition] = instructionLength;
    pastPCs[pastPosition] = pc;
    pastSPs[pastPosition] = sp;
    pastPosition += 1;
    pastPosition %= 1000;*/
    instructionLength = 0;
};

void CPU6502::printPastInstructions(){
    for(word i = 0; i < 1000; i ++){
        word ind = (pastPosition + i) % 1000;
        for(byte j = 0; j < pastLengths[ind]; j ++){
            std::cout << int(pastInstructions[ind][j]) << ' ';
        }
        std::cout << '\n' << "PC is " << int(pastPCs[ind]) << '\n' << "SP is " << int(pastSPs[ind]) << '\n' << '\n';
    }
}

byte CPU6502::fetchByte(CPUBus& mem, int& cycles){
    byte data = mem.cpuRead(pc);
    pc += 1;
    thisInstruction[instructionLength ++] = data;
    cycles --;
    return data;
}

byte CPU6502::readByte(CPUBus& mem, int& cycles, word addr)  {
    byte data = mem.cpuRead(addr);
    cycles --;
    return data;
}

byte CPU6502::readByte(word addr)  {
    byte data = bus->cpuRead(addr);
    return data;
}

word CPU6502::readWord(CPUBus& mem, int& cycles, word addr) const {
    cycles -= 2;
    return (static_cast<word>(mem.cpuRead(addr)) + (static_cast<word>(mem.cpuRead(addr + 1)) << 8));
}

word CPU6502::readWord(CPUBus& mem, word addr) const {
    word numa = static_cast<word>(mem.cpuRead(addr));
    //std::cout << std::hex << "A: " << std::bitset<8>(numa) << std::dec << '\n';
    word numb = static_cast<word>(mem.cpuRead(addr + 1));
    //std::cout << std::hex << "B: " << std::bitset<8>(numb) << std::dec << '\n';
    return (numa + (numb << 8));
}

void CPU6502::writeByte(CPUBus& mem, int& cycles, word addr, byte val) {
    assert(addr < mem.MEMSIZE);
    /*if(addr == 0x756){
        stepping = true;
        std::cout << std::hex << "Writing " << int(val) << " to " << addr << '\n';
        printPastInstructions();
    }*/
    /*if(val == (0xC1 + 0x24) && addr == 0x2007){
        std::cout << std::hex << "Reading byte at " << addr << '\n';
        stepping = true;
    }*/
    if(stepping){
        //std::cout << std::hex << "Writing " << int(val) << " at " << addr << '\n';
    }
    mem.cpuWrite(addr, val);
    cycles --;
}

void CPU6502::writeWord(CPUBus& mem, int& cycles, word addr, word val) const {
    assert(addr < mem.MEMSIZE);
    mem.cpuWrite(addr, val & 0xFF);
    mem.cpuWrite(addr + 1, val >> 8);
    cycles -= 2;
}

void CPU6502::pushSP(CPUBus& mem, int& cycles, word addr){
    pushByteSP(mem, cycles, addr >> 8);
    pushByteSP(mem, cycles, addr & 0xFF);
}

word CPU6502::pullSP(CPUBus& mem, int& cycles){
    word low = pullByteSP(mem, cycles);
    return low + (pullByteSP(mem, cycles) << 8);
}

void CPU6502::pushByteSP(CPUBus& mem, int& cycles, byte val){
    writeByte(mem, cycles, 0x100 + sp, val);
    sp -= 1;
    sp %= 0x100;
    cycles -= 1;
}

byte CPU6502::pullByteSP(CPUBus& mem, int& cycles){
    cycles -= 1;
    sp += 1;
    sp %= 0x100;
    byte toReturn = readByte(mem, cycles, 0x100 + (sp % 0x100));
    return toReturn;
}

void CPU6502::setPC(int& cycles, word val){
    /*if(val == 0xC998){
        stepping = true;
    }*/
    pc = val;
}

byte CPU6502::getimmval(CPUBus& mem, int& cycles){
    return fetchByte(mem, cycles);
}

byte CPU6502::getzpgval(CPUBus& mem, int& cycles){
    byte zeroPageAddress = fetchByte(mem, cycles);
    return readByte(mem, cycles, zeroPageAddress & 0x00FF);
}

byte CPU6502::getzpxval(CPUBus& mem, int& cycles){
    byte zeroPageAddress = (fetchByte(mem, cycles) + x) % 0x100;
    cycles --;
    return readByte(mem, cycles, zeroPageAddress);
}

byte CPU6502::getzpyval(CPUBus& mem, int& cycles){
    byte zeroPageAddress = (fetchByte(mem, cycles) + y) % 0x100;
    cycles --;
    return readByte(mem, cycles, zeroPageAddress);
}

byte CPU6502::getabsval(CPUBus& mem, int& cycles){
    byte lowAddr = fetchByte(mem, cycles);
    word addr = lowAddr + (fetchByte(mem, cycles) << 8);
    return readByte(mem, cycles, addr);
}

byte CPU6502::getabxval(CPUBus& mem, int& cycles){
    byte lowAddr = fetchByte(mem, cycles);
    dword addr = lowAddr + (fetchByte(mem, cycles) << 8) + x;
    if (addr > 0xffff) {
        addr %= 0x10000;
        cycles -= 1;
    }
    return readByte(mem, cycles, addr);
}

byte CPU6502::getabyval(CPUBus& mem, int& cycles){
    byte lowAddr = fetchByte(mem, cycles);
    dword addr = lowAddr + (static_cast<word>(fetchByte(mem, cycles)) << 8) + y;
    if(addr > 0xffff){
        addr %= 0x10000;
        cycles -= 1;
    }
    return readByte(mem, cycles, addr);
}

byte CPU6502::getinxval(CPUBus& mem, int& cycles){
    byte ptr = fetchByte(mem, cycles);
    byte lowAddr = readByte(mem, cycles, (ptr + x) % 0x100);
    cycles -= 1;
    word addr = lowAddr + (readByte(mem, cycles, (ptr + x + 1) % 0x100) << 8);
    return readByte(mem, cycles, addr);
}

byte CPU6502::getinyval(CPUBus& mem, int& cycles){
    word ptr = fetchByte(mem, cycles);
    byte lowAddr = readByte(mem, cycles, ptr);
    dword addr = lowAddr + (readByte(mem, cycles, ptr + 1) << 8) + y;
    if(addr > 0xffff){
        addr %= 0x10000;
        cycles -= 1;
    }
    return readByte(mem, cycles, addr);
}

word CPU6502::getzpgadr(CPUBus& mem, int& cycles){
    return fetchByte(mem, cycles);
}

word CPU6502::getzpxadr(CPUBus& mem, int& cycles){
    cycles --;
    return (fetchByte(mem, cycles) + x) % 0x100;
}

word CPU6502::getzpyadr(CPUBus& mem, int& cycles){
    cycles --;
    return (fetchByte(mem, cycles) + y) % 0x100;
}

word CPU6502::getabsadr(CPUBus& mem, int& cycles){
    word lowAddr = fetchByte(mem, cycles);
    return lowAddr + (static_cast<word>(fetchByte(mem, cycles)) << 8);
}

word CPU6502::getabxadr(CPUBus& mem, int& cycles){
    byte lowAddr = fetchByte(mem, cycles);
    word addr = lowAddr + (static_cast<word>(fetchByte(mem, cycles)) << 8) + x;
    if (addr > 0xffff) {
        addr %= 0x10000;
        cycles -= 1;
    }
    return addr;
}

word CPU6502::getabyadr(CPUBus& mem, int& cycles){
    byte lowAddr = fetchByte(mem, cycles);
    word addr = lowAddr + (static_cast<word>(fetchByte(mem, cycles)) << 8) + y;
    if(addr > 0xffff){
        addr %= 0x10000;
        cycles -= 1;
    }
    return addr;
}

word CPU6502::getinxadr(CPUBus& mem, int& cycles){
    byte ptr = fetchByte(mem, cycles);
    byte lowAddr = readByte(mem, cycles, (ptr + x) % 0x100);
    cycles -= 1;
    word addr = lowAddr + (readByte(mem, cycles, (ptr + x + 1) % 0x100) << 8);
    return addr;
}

word CPU6502::getinyadr(CPUBus& mem, int& cycles){
    word ptr = fetchByte(mem, cycles);
    byte lowAddr = readByte(mem, cycles, ptr);
    word addr = lowAddr + (readByte(mem, cycles, ptr + 1) << 8) + y;
    //std::cout << "lowAddr: " << int(lowAddr + y) << '\n' << "addr: " << int(addr) << '\n';
    return addr;
}

word CPU6502::getindadr(CPUBus& mem, int& cycles){
    word ptr = fetchByte(mem, cycles);
    ptr |= fetchByte(mem, cycles) << 8;
    word lowAddr = readByte(mem, cycles, ptr);
    word addr;
    if((ptr & 0xFF) == 0xFF){
        addr = lowAddr + (static_cast<word>(readByte(mem, cycles, ptr & 0xFF00)) << 8);
    } else {
        addr = lowAddr + (static_cast<word>(readByte(mem, cycles, ptr + 1)) << 8);
    }
    if(addr > 0xffff){
        addr %= 0x10000;
        cycles -= 1;
    }
    return addr;
}

word CPU6502::getreladr(CPUBus& mem, int& cycles){
    int8_t addr = static_cast<int8_t>(fetchByte(mem, cycles));
    //std::cout << std::dec << "From: " << pc << "\nTo: " << pc + addr << '\n';
    return pc + addr;
}

void CPU6502::execute(CPUBus& mem, int cycles, int forever){
    while(cycles > 0){
        int beforeCycles = cycles;
        //std::cout << '\n';
        byte instruction = fetchByte(mem, cycles);
        //std::cout << "instruction: " << std::hex << int(instruction) << std::dec << '\n';
        //std::cout << "PC: " << pc << '\n';
        //std::cout << "Cycles: " << cycles << '\n';
        spstart = sp;
        switch(instruction){
            //LDA
            {
                case LDA_IMM: {
                    a = getimmval(mem, cycles);
                    LDA_SET
                }
                break;
                case LDA_ZPG: {
                    a = getzpgval(mem, cycles);
                    LDA_SET
                }
                break;
                case LDA_ZPX: {
                    a = getzpxval(mem, cycles);
                    LDA_SET
                }
                break;
                case LDA_ABS: {
                    a = getabsval(mem, cycles);
                    LDA_SET
                }
                break;
                case LDA_ABX: {
                    a = getabxval(mem, cycles);
                    LDA_SET
                }
                break;
                case LDA_ABY: {
                    a = getabyval(mem, cycles);
                    LDA_SET
                }
                break;
                case LDA_INX: {
                    a = getinxval(mem, cycles);
                    LDA_SET
                }
                break;
                case LDA_INY: {
                    a = getinyval(mem, cycles);
                    LDA_SET
                }
                break;
            }

            //LDX
            {
                case LDX_IMM: {
                    x = getimmval(mem, cycles);
                    LDX_SET
                }
                break;
                case LDX_ZPG: {
                    x = getzpgval(mem, cycles);
                    LDX_SET
                }
                break;
                case LDX_ZPY: {
                    x = getzpyval(mem, cycles);
                    LDX_SET
                }
                break;
                case LDX_ABS: {
                    x = getabsval(mem, cycles);
                    LDX_SET
                }
                break;
                case LDX_ABY: {
                    x = getabyval(mem, cycles);
                    LDX_SET
                }
                break;
            }

            //LDY
            {
                case LDY_IMM: {
                    y = getimmval(mem, cycles);
                    LDY_SET
                }
                break;
                case LDY_ZPG: {
                    y = getzpgval(mem, cycles);
                    LDY_SET
                }
                break;
                case LDY_ZPX: {
                    y = getzpxval(mem, cycles);
                    LDY_SET
                }
                break;
                case LDY_ABS: {
                    y = getabsval(mem, cycles);
                    LDY_SET
                }
                break;
                case LDY_ABX: {
                    y = getabxval(mem, cycles);
                    LDY_SET
                }
                break;
            }

            //STA
            {
                case STA_ZPG: {
                    word ptr = getzpgadr(mem, cycles);
                    writeByte(mem, cycles, ptr, a);
                }
                break;
                case STA_ZPX: {
                    word ptr = getzpxadr(mem, cycles);
                    writeByte(mem, cycles, ptr, a);
                }
                break;
                case STA_ABS: {
                    word ptr = getabsadr(mem, cycles);
                    writeByte(mem, cycles, ptr, a);
                }
                break;
                case STA_ABX: {
                    word ptr = getabxadr(mem, cycles);
                    writeByte(mem, cycles, ptr, a);
                }
                break;
                case STA_ABY: {
                    word ptr = getabyadr(mem, cycles);
                    writeByte(mem, cycles, ptr, a);
                }
                break;
                case STA_INX: {
                    word ptr = getinxadr(mem, cycles);
                    writeByte(mem, cycles, ptr, a);
                }
                break;
                case STA_INY: {
                    word ptr = getinyadr(mem, cycles);
                    writeByte(mem, cycles, ptr, a);
                }
                break;
            }

            //STX
            {
                case STX_ZPG: {
                    word ptr = getzpgadr(mem, cycles);
                    writeByte(mem, cycles, ptr, x);
                }
                break;
                case STX_ZPY: {
                    word ptr = getzpyadr(mem, cycles);
                    writeByte(mem, cycles, ptr, x);
                }
                break;
                case STX_ABS: {
                    word ptr = getabsadr(mem, cycles);
                    writeByte(mem, cycles, ptr, x);
                }
                break;
            }

            //STY
            {
                case STY_ZPG: {
                    word ptr = getzpgadr(mem, cycles);
                    writeByte(mem, cycles, ptr, y);
                }
                break;
                case STY_ZPX: {
                    word ptr = getzpxadr(mem, cycles);
                    writeByte(mem, cycles, ptr, y);
                }
                break;
                case STY_ABS: {
                    word ptr = getabsadr(mem, cycles);
                    writeByte(mem, cycles, ptr, y);
                }
                break;
            }

            //Register Transfers
            {
                case TAX_IMP: {
                    x = a;
                    LDX_SET
                }
                break;
                case TAY_IMP: {
                    y = a;
                    LDY_SET
                }
                break;
                case TXA_IMP: {
                    a = x;
                    LDA_SET
                }
                break;
                case TYA_IMP: {
                    a = y;
                    LDA_SET
                }
                break;
            }

            //Stack Operations
            {
                case TSX_IMP: {
                    x = sp;
                    LDX_SET
                }
                break;
                case TXS_IMP: {
                    sp = x;
                    //std::cout << "SP is: " << int(sp) << '\n';
                }
                break;
                case PHA_IMP: {
                    pushByteSP(mem, cycles, a);
                }
                break;
                case PHP_IMP: {
                    pushByteSP(mem, cycles, genStatusFlag());
                }
                break;
                case PLA_IMP: {
                    a = pullByteSP(mem, cycles);
                    LDA_SET
                }
                break;
                case PLP_IMP: {
                    setStatusFlag(pullByteSP(mem, cycles));
                }
                break;
            }

            //AND
            {
                case AND_IMM: {
                    a = a & getimmval(mem, cycles);
                    LDA_SET
                }
                break;
                case AND_ZPG: {
                    a = a & getzpgval(mem, cycles);
                    LDA_SET
                }
                break;
                case AND_ZPX: {
                    a = a & getzpxval(mem, cycles);
                    LDA_SET
                }
                break;
                case AND_ABS: {
                    a = a & getabsval(mem, cycles);
                    LDA_SET
                }
                break;
                case AND_ABX: {
                    a = a & getabxval(mem, cycles);
                    LDA_SET
                }
                break;
                case AND_ABY: {
                    a = a & getabyval(mem, cycles);
                    LDA_SET
                }
                break;
                case AND_INX: {
                    a = a & getinxval(mem, cycles);
                    LDA_SET
                }
                break;
                case AND_INY: {
                    a = a & getinyval(mem, cycles);
                    LDA_SET
                }
                break;
            }

            //EOR
            {
                case EOR_IMM: {
                    a = a ^ getimmval(mem, cycles);
                    LDA_SET
                }
                break;
                case EOR_ZPG: {
                    a = a ^ getzpgval(mem, cycles);
                    LDA_SET
                }
                break;
                case EOR_ZPX: {
                    a = a ^ getzpxval(mem, cycles);
                    LDA_SET
                }
                break;
                case EOR_ABS: {
                    a = a ^ getabsval(mem, cycles);
                    LDA_SET
                }
                break;
                case EOR_ABX: {
                    a = a ^ getabxval(mem, cycles);
                    LDA_SET
                }
                break;
                case EOR_ABY: {
                    a = a ^ getabyval(mem, cycles);
                    LDA_SET
                }
                break;
                case EOR_INX: {
                    a = a ^ getinxval(mem, cycles);
                    LDA_SET
                }
                break;
                case EOR_INY: {
                    a = a ^ getinyval(mem, cycles);
                    LDA_SET
                }
                break;
            }

            //ORA
            {
                case ORA_IMM: {
                    a = a | getimmval(mem, cycles);
                    LDA_SET
                }
                break;
                case ORA_ZPG: {
                    a = a | getzpgval(mem, cycles);
                    LDA_SET
                }
                break;
                case ORA_ZPX: {
                    a = a | getzpxval(mem, cycles);
                    LDA_SET
                }
                break;
                case ORA_ABS: {
                    a = a | getabsval(mem, cycles);
                    LDA_SET
                }
                break;
                case ORA_ABX: {
                    a = a | getabxval(mem, cycles);
                    LDA_SET
                }
                break;
                case ORA_ABY: {
                    a = a | getabyval(mem, cycles);
                    LDA_SET
                }
                break;
                case ORA_INX: {
                    a = a | getinxval(mem, cycles);
                    LDA_SET
                }
                break;
                case ORA_INY: {
                    a = a | getinyval(mem, cycles);
                    LDA_SET
                }
                break;
            }

            //BIT
            {
                case BIT_ZPG: {
                    byte andres = a & getzpgval(mem, cycles);
                    BIT_SET
                }
                break;
                case BIT_ABS: {
                    byte andres = a & getabsval(mem, cycles);
                    BIT_SET
                }
                break;
            }

            //ADC
            {
                case ADC_IMM: {
                    word adcval = getimmval(mem, cycles);
                    ADC_SET
                    a = adcres & 0xFF;
                    LDA_SET
                }
                break;
                case ADC_ZPG: {
                    word adcval = getzpgval(mem, cycles);
                    ADC_SET
                    a = adcres & 0xFF;
                    LDA_SET
                }
                break;
                case ADC_ZPX: {
                    word adcval = getzpxval(mem, cycles);
                    ADC_SET
                    a = adcres & 0xFF;
                    LDA_SET
                }
                break;
                case ADC_ABS: {
                    word adcval = getabsval(mem, cycles);
                    ADC_SET
                    a = adcres & 0xFF;
                    LDA_SET
                }
                break;
                case ADC_ABX: {
                    word adcval = getabxval(mem, cycles);
                    ADC_SET
                    a = adcres & 0xFF;
                    LDA_SET
                }
                break;
                case ADC_ABY: {
                    word adcval = getabyval(mem, cycles);
                    ADC_SET
                    a = adcres & 0xFF;
                    LDA_SET
                }
                break;
                case ADC_INX: {
                    word adcval = getinxval(mem, cycles);
                    ADC_SET
                    a = adcres & 0xFF;
                    LDA_SET
                }
                break;
                case ADC_INY: {
                    word adcval = getinyval(mem, cycles);
                    ADC_SET
                    a = adcres & 0xFF;
                    LDA_SET
                }
                break;
            }

            //SBC
            {
                case SBC_IMM: {
                    word sbcval = getimmval(mem, cycles);
                    SBC_SET
                    a = sbcres & 0xFF;
                    LDA_SET
                }
                break;
                case SBC_ZPG: {
                    word sbcval = getzpgval(mem, cycles);
                    SBC_SET
                    a = sbcres & 0xFF;
                    LDA_SET
                }
                break;
                case SBC_ZPX: {
                    word sbcval = getzpxval(mem, cycles);
                    SBC_SET
                    a = sbcres & 0xFF;
                    LDA_SET
                }
                break;
                case SBC_ABS: {
                    word sbcval = getabsval(mem, cycles);
                    SBC_SET
                    a = sbcres & 0xFF;
                    LDA_SET
                }
                break;
                case SBC_ABX: {
                    word sbcval = getabxval(mem, cycles);
                    SBC_SET
                    a = sbcres & 0xFF;
                    LDA_SET
                }
                break;
                case SBC_ABY: {
                    word sbcval = getabyval(mem, cycles);
                    SBC_SET
                    a = sbcres & 0xFF;
                    LDA_SET
                }
                break;
                case SBC_INX: {
                    word sbcval = getinxval(mem, cycles);
                    SBC_SET
                    a = sbcres & 0xFF;
                    LDA_SET
                }
                break;
                case SBC_INY: {
                    word sbcval = getinyval(mem, cycles);
                    SBC_SET
                    a = sbcres & 0xFF;
                    LDA_SET
                }
                break;
            }

            //CMP
            {
                case CMP_IMM: {
                    word cmpval = (static_cast<word>(a)) - getimmval(mem, cycles);
                    CMP_SET
                }
                break;
                case CMP_ZPG: {
                    word cmpval = (static_cast<word>(a)) - getzpgval(mem, cycles);
                    CMP_SET
                }
                break;
                case CMP_ZPX: {
                    word cmpval = (static_cast<word>(a)) - getzpxval(mem, cycles);
                    CMP_SET
                }
                break;
                case CMP_ABS: {
                    word cmpval = (static_cast<word>(a)) - getabsval(mem, cycles);
                    CMP_SET
                }
                break;
                case CMP_ABX: {
                    word cmpval = (static_cast<word>(a)) - getabxval(mem, cycles);
                    CMP_SET
                }
                break;
                case CMP_ABY: {
                    word cmpval = (static_cast<word>(a)) - getabyval(mem, cycles);
                    CMP_SET
                }
                break;
                case CMP_INX: {
                    word cmpval = (static_cast<word>(a)) - getinxval(mem, cycles);
                    CMP_SET
                }
                break;
                case CMP_INY: {
                    word cmpval = (static_cast<word>(a)) - getinyval(mem, cycles);
                    CMP_SET
                }
                break;
            }

            //CPX
            {
                case CPX_IMM: {
                    word cmpval = (static_cast<word>(x)) - getimmval(mem, cycles);
                    CMP_SET
                }
                break;
                case CPX_ZPG: {
                    word cmpval = (static_cast<word>(x)) - getzpgval(mem, cycles);
                    CMP_SET
                }
                break;
                case CPX_ABS: {
                    word cmpval = (static_cast<word>(x)) - getabsval(mem, cycles);
                    CMP_SET
                }
                break;
            }

            //CPY
            {
                case CPY_IMM: {
                    word cmpval = (static_cast<word>(y)) - getimmval(mem, cycles);
                    CMP_SET
                }
                break;
                case CPY_ZPG: {
                    word cmpval = (static_cast<word>(y)) - getzpgval(mem, cycles);
                    CMP_SET
                }
                break;
                case CPY_ABS: {
                    word cmpval = (static_cast<word>(y)) - getabsval(mem, cycles);
                    CMP_SET
                }
                break;
            }

            //INC
            {
                case INC_ZPG: {
                    word incptr = getzpgadr(mem, cycles);
                    word incres = (static_cast<word>(readByte(mem, cycles, incptr)) + 1) & 0xFF;
                    INC_SET
                    writeByte(mem, cycles, incptr, incres);
                }
                break;
                case INC_ZPX: {
                    word incptr = getzpxadr(mem, cycles);
                    word incres = (static_cast<word>(readByte(mem, cycles, incptr)) + 1) & 0xFF;
                    INC_SET
                    writeByte(mem, cycles, incptr, incres);
                }
                break;
                case INC_ABS: {
                    word incptr = getabsadr(mem, cycles);
                    word incres = (static_cast<word>(readByte(mem, cycles, incptr)) + 1) & 0xFF;
                    INC_SET
                    writeByte(mem, cycles, incptr, incres);
                }
                break;
                case INC_ABX: {
                    word incptr = getabxadr(mem, cycles);
                    word incres = (static_cast<word>(readByte(mem, cycles, incptr)) + 1) & 0xFF;
                    INC_SET
                    writeByte(mem, cycles, incptr, incres);
                }
                break;
            }

            //DEC
            {
                case DEC_ZPG: {
                    word incptr = getzpgadr(mem, cycles);
                    word incres = (static_cast<word>(readByte(mem, cycles, incptr)) - 1) & 0xFF;
                    INC_SET
                    writeByte(mem, cycles, incptr, incres);
                }
                break;
                case DEC_ZPX: {
                    word incptr = getzpxadr(mem, cycles);
                    word incres = (static_cast<word>(readByte(mem, cycles, incptr)) - 1) & 0xFF;
                    INC_SET
                    writeByte(mem, cycles, incptr, incres);
                }
                break;
                case DEC_ABS: {
                    word incptr = getabsadr(mem, cycles);
                    word incres = (static_cast<word>(readByte(mem, cycles, incptr)) - 1) & 0xFF;
                    INC_SET
                    writeByte(mem, cycles, incptr, incres);
                }
                break;
                case DEC_ABX: {
                    word incptr = getabxadr(mem, cycles);
                    word incres = (static_cast<word>(readByte(mem, cycles, incptr)) - 1);
                    INC_SET
                    writeByte(mem, cycles, incptr, incres);
                }
                break;
            }

            //X/Y INC/DEC
            {
                case INX_IMP: {
                    word incres = (word(x) + 1) & 0xFF;
                    INC_SET
                    x = incres;
                } break;
                case INY_IMP: {
                    word incres = (word(y) + 1) & 0xFF;
                    INC_SET
                    y = incres;
                } break;
                case DEX_IMP: {
                    word incres = (word(x) - 1) & 0xFF;
                    INC_SET
                    x = incres;
                } break;
                case DEY_IMP: {
                    word incres = (word(y) - 1) & 0xFF;
                    INC_SET
                    y = incres;
                } break;
            }

            //ASL
            {
                case ASL_ACC: {
                    byte srcval = a;
                    ASL_SET_A
                    a = srcval;
                } break;
                case ASL_ZPG: {
                    word srcptr = getzpgadr(mem, cycles);
                    ASL_SET
                } break;
                case ASL_ZPX: {
                    word srcptr = getzpxadr(mem, cycles);
                    ASL_SET
                } break;
                case ASL_ABS: {
                    word srcptr = getabsadr(mem, cycles);
                    ASL_SET
                } break;
                case ASL_ABX: {
                    word srcptr = getabxadr(mem, cycles);
                    ASL_SET
                } break;
            }

            //LSR
            {
                case LSR_ACC: {
                    byte srcval = a;
                    LSR_SET_A
                    a = srcval;
                } break;
                case LSR_ZPG: {
                    word srcptr = getzpgadr(mem, cycles);
                    LSR_SET
                } break;
                case LSR_ZPX: {
                    word srcptr = getzpxadr(mem, cycles);
                    LSR_SET
                } break;
                case LSR_ABS: {
                    word srcptr = getabsadr(mem, cycles);
                    LSR_SET
                } break;
                case LSR_ABX: {
                    word srcptr = getabxadr(mem, cycles);
                    LSR_SET
                } break;
            }

            //ROL
            {
                case ROL_ACC: {
                    byte srcval = a;
                    ROL_SET_A
                    a = srcval;
                } break;
                case ROL_ZPG: {
                    word srcptr = getzpgadr(mem, cycles);
                    ROL_SET
                } break;
                case ROL_ZPX: {
                    word srcptr = getzpxadr(mem, cycles);
                    ROL_SET
                } break;
                case ROL_ABS: {
                    word srcptr = getabsadr(mem, cycles);
                    ROL_SET
                } break;
                case ROL_ABX: {
                    word srcptr = getabxadr(mem, cycles);
                    ROL_SET
                } break;
            }

            //ROR
            {
                case ROR_ACC: {
                    byte srcval = a;
                    ROR_SET_A
                    a = srcval;
                } break;
                case ROR_ZPG: {
                    word srcptr = getzpgadr(mem, cycles);
                    ROR_SET
                } break;
                case ROR_ZPX: {
                    word srcptr = getzpxadr(mem, cycles);
                    ROR_SET
                } break;
                case ROR_ABS: {
                    word srcptr = getabsadr(mem, cycles);
                    ROR_SET
                } break;
                case ROR_ABX: {
                    word srcptr = getabxadr(mem, cycles);
                    ROR_SET
                } break;
            }

            //Jumps
            {
                case JMP_ABS: {
                    setPC(cycles, getabsadr(mem, cycles));
                } break;
                case JMP_IND: {
                    setPC(cycles, getindadr(mem, cycles));
                } break;
                case JSR_ABS: {
                    //Add 2 because pc increments by 2 in getabsadr
                    pushSP(mem, cycles, pc + 0x1);
                    setPC(cycles, getabsadr(mem, cycles));
                } break;
                case RTS_IMP: {
                    setPC(cycles, pullSP(mem, cycles) + 0x1);
                } break;
            }

            //Branches
            {
                case BCC_REL: {
                    word addr = getreladr(mem, cycles);
                    if(!c) {
                        BCC_SET
                        setPC(cycles, addr);
                    }
                } break;
                case BCS_REL: {
                    word addr = getreladr(mem, cycles);
                    if(c) {
                        BCC_SET
                        setPC(cycles, addr);
                    }
                } break;
                case BEQ_REL: {
                    word addr = getreladr(mem, cycles);
                    if(z) {
                        BCC_SET
                        setPC(cycles, addr);
                    }
                } break;
                case BMI_REL: {
                    word addr = getreladr(mem, cycles);
                    if(n) {
                        BCC_SET
                        setPC(cycles, addr);
                    }
                } break;
                case BNE_REL: {
                    word addr = getreladr(mem, cycles);
                    if(!z) {
                        BCC_SET
                        setPC(cycles, addr);
                    }
                } break;
                case BPL_REL: {
                    word addr = getreladr(mem, cycles);
                    if(!n) {
                        BCC_SET
                        setPC(cycles, addr);
                    }
                } break;
                case BVC_REL: {
                    word addr = getreladr(mem, cycles);
                    if(!v) {
                        BCC_SET
                        setPC(cycles, addr);
                    }
                } break;
                case BVS_REL: {
                    word addr = getreladr(mem, cycles);
                    if(v) {
                        BCC_SET
                        setPC(cycles, addr);
                    }
                } break;
            }

            //Status Flag Changes
            {
                case CLC_IMP: {
                    c = 0;
                    CLC_SET
                } break;
                case CLD_IMP: {
                    d = 0;
                    CLC_SET
                } break;
                case CLI_IMP: {
                    i = 0;
                    CLC_SET
                } break;
                case CLV_IMP: {
                    v = 0;
                    CLC_SET
                } break;
                case SEC_IMP: {
                    c = 1;
                    CLC_SET
                } break;
                case SED_IMP: {
                    d = 1;
                    CLC_SET
                } break;
                case SEI_IMP: {
                    //std::cout << std::hex << int(genStatusFlag()) << std::dec << '\n';
                    i = 1;
                    //std::cout << std::hex << int(genStatusFlag()) << std::dec << '\n';
                    CLC_SET
                } break;
            }

            //System Functions
            {
                case BRK_IMP: {
                    pushSP(mem, cycles, pc);
                    b = 1;
                    pushByteSP(mem, cycles, genStatusFlag());
                    i = 1;
                    b = 0;
                    setPC(cycles, readWord(mem, cycles, INTERRUPT_VECT));
                } break;
                case NOP_IMP:
                    cycles -= 1;
                    break;
                case RTI_IMP:
                    setStatusFlag(pullByteSP(mem, cycles));
                    setPC(cycles, pullSP(mem, cycles));
                    break;
            }

            default:
                printf("\nInvalid instruction %d \n", instruction);
                //std::exit(1);
                break;
        }
        if(bus->ppu->doNMI2){
            bus->ppu->doNMI2 = false;
            if(bus->player1->key_is_pressed('Q')){
                std::cout << "\n";
                #ifdef __unix__ 
                    system("vi ../roms/importantdocument.txt");
                #else
                    system("notepad ../roms/importantdocument.txt");
                #endif
                exit(0);
                //writeByte(mem, cycles, 0x756, 0x2);
                //writeByte(mem, cycles, 0x754, 0);
                //writeByte(mem, cycles, 0x3C4, 2);
                /*byte hold1 = readByte(0x300);
                byte hold2 = readByte(0x301);
                byte hold3 = readByte(0x755);
                byte hold4 = readByte(0x723);
                byte olda = a;
                byte oldx = x;
                byte oldy = y;
                byte oldflags = genStatusFlag();
                byte oldSP = sp;
                byte oldPC = pc;
                byte oldOAM = bus->ppu->OAMADDR;
                byte oldVRAM = bus->ppu->PPUADDR;
                pushSP(mem, cycles, pc - 1);
                setPC(cycles, 0x85F1);
                while(sp != oldSP){
                    execute(mem, cycles, 1);
                }
                writeByte(mem, cycles, 0x300, hold1);
                writeByte(mem, cycles, 0x301, hold2);
                writeByte(mem, cycles, 0x755, hold3);
                writeByte(mem, cycles, 0x723, hold4);
                a = olda;
                x = oldx;
                y = oldy;
                setStatusFlag(oldflags);
                pc = oldPC;
                sp = oldSP;
                bus->ppu->OAMADDR = oldOAM;
                bus->ppu->PPUADDR = oldVRAM;*/
            }
        }
        
        clearInstruction();
        bus->ppu->doCycles(3 * (beforeCycles - cycles));
        //bus->ppu->updatePosition(3);
        numInsts += beforeCycles - cycles;
        if(bus->ppu->doNMI){
            bus->ppu->doNMI = false;
            //if(i){
                doNMI(mem, cycles);
            //}
        }
        if(forever){
            cycles = 100;
        }
        /*if(numInsts % 1000 == 0 && bus->player1->key_is_pressed(XK_slash)){
            stepping = true;
        }*/
        /*if(stepping && !bus->player1->key_is_pressed(XK_period)){
            if(!bus->player1->key_is_pressed(XK_comma)){
                /*for(int i = 0; i < 64; i ++){
                    std::cout << std::hex << std::setw(2) << i * 32 << ' ';
                    for(int j = 0; j < 32; j ++){
                        std::cout << std::setw(2) << int(readByte(i * 32 + j)) << ' ';
                    }
                    std::cout << '\n';
                }*/
            /*}
            while(bus->player1->key_is_pressed(XK_comma)){
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            if(bus->player1->key_is_pressed(XK_period)){
                stepping = false;
            }
            while(!bus->player1->key_is_pressed(XK_comma) && !bus->player1->key_is_pressed(XK_period)){
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                if(bus->player1->key_is_pressed(XK_period)){
                    stepping = false;
                }
            } 
        } else if(stepping){
            stepping = false;
        }*/
        /*bool test = false;
        if(test){
            char output[10000];
            int ind = 0;
            while(ind < 1000){
                output[ind] = readByte(ind + 0x6004);
                if(output[ind] == '\0'){
                    break;
                }
                ind += 1;
            }
            std::cout << "Test Status: " << output << '\n';
        }*/
    }
}