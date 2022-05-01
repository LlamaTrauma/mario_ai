#ifndef NESEMU_CPU6502_H
#define NESEMU_CPU6502_H
#include "helper.h"
#include "CPUBus.h"

struct CPU6502 {
public:
    int pc; //program counter
    //Mario bros sets sp to 0xff at start
    byte sp; //stack pointer (lower byte of $0100-$01FF)
    byte spstart = 0;

    byte a, x, y; //registers

    byte c : 1; //Carry flag
    byte z : 1; //Zero flag
    byte i : 1; //Interrupt disable
    byte d : 1; //Decimal mode
    byte b : 1; //Break command
    byte v : 1; //Overflow flag
    byte n : 1; //Negative flag

    byte thisInstruction[5];
    int instructionLength = 0;

    byte pastInstructions[1000][5];
    word pastPCs[1000];
    word pastSPs[1000];
    byte pastLengths[1000];
    word pastPosition = 0;

    byte instructionsRun[200];
    byte numInstructionsRun = 0;
    std::fstream outFile;
    bool stepping = false;
    bool paststate1;
    bool paststate2;

    //45: ac ae b1 be 9d aa 48 5 68 2a 3d f0 0 19 
    //Tested: 78, D8, A9, 8D, A2, 9A, AD, 10, A0, BD, C9, CA, 20, 85, 86, E0, 91, 88, C0, 60, 2C, 99, C8, 09, 29, 8A, 4C, EE, 4A
    //SEI_IMP, CLD_IMP, LDA_IMM, STA_ABS, LDX_IMM, TXS_IMP, LDA_ABS, BPL_REL, LDY_IMM, LDA_ABX, CMP_IMM, DEX_IMP, JSR_ABS, STA_ZPG, STX_ZPG, CPX_IMM, STA_INY, DEY_IMP, CPY_IMM, RTS_IMP, BIT_ABS, STA_ABY, INY_IMP, ORA_IMM, AND_IMM, TXA_IMP, JMP_ABS, INC_ABS, LSR_ACC
    //Not tested but should be fine (use same functions as tested stuff)
    //B0, D0
    //BCS_REL, BNE_REL

    //Donkey Kong:
    //76: 78 d8 a9 8d a2 9a ad 29 f0 a0 84 91 88 d0 c6 10 20 85 4c 48 8a 98 a5 c9 25 4a aa 18 65 ca a6 c8 69 60 68 a8 49 45 66 ae b1 bd 5 2a 86 6 b4 95 35 e8 a4 b0 46 30 90 8e 38 a 9 b5 d6 40 b9 ce e6 6d 8c 6a 9d e9 ee e5 c0 8 28 0
    //Checked:
    //6A
    //ROR_ACC
    //Glanced at:
    //9D, E9, EE, E5, C0, 08, 28
    //STA_ABX, SBC_IMM, INC_ABS, SBC_ZPG, CPY_IMM, PHP_IMP, PLP_IMP

    //Mario Bros:
    //57: 78 d8 a9 8d a2 9a ad 10 a0 bd c9 b0 ca d0 20 85 86 e0 91 88 c0 60 2c 99 c8 9 29 8a 4c ee ac ae b1 be 9d 4a aa 48 5 68 2a 3d f0 e8 38 f9 90 ce e6 45 18 7e a a8 6c a5 40 

    //aa bd b1 65 ae c9 b0 68 90 4a 98 a4 40 8e 79 

    static const byte LDA_IMM = 0xA9;
    static const byte LDA_ZPG = 0xA5;
    static const byte LDA_ZPX = 0xB5;
    static const byte LDA_ABS = 0xAD;
    static const byte LDA_ABX = 0xBD;
    static const byte LDA_ABY = 0xB9;
    static const byte LDA_INX = 0xA1;
    static const byte LDA_INY = 0xB1;

    static const byte LDX_IMM = 0xA2;
    static const byte LDX_ZPG = 0xA6;
    static const byte LDX_ZPY = 0xB6;
    static const byte LDX_ABS = 0xAE;
    static const byte LDX_ABY = 0xBE;

    static const byte LDY_IMM = 0xA0;
    static const byte LDY_ZPG = 0xA4;
    static const byte LDY_ZPX = 0xB4;
    static const byte LDY_ABS = 0xAC;
    static const byte LDY_ABX = 0xBC;

    static const byte STA_ZPG = 0x85;
    static const byte STA_ZPX = 0x95;
    static const byte STA_ABS = 0x8D;
    static const byte STA_ABX = 0x9D;
    static const byte STA_ABY = 0x99;
    static const byte STA_INX = 0x81;
    static const byte STA_INY = 0x91;

    static const byte STX_ZPG = 0x86;
    static const byte STX_ZPY = 0x96;
    static const byte STX_ABS = 0x8E;

    static const byte STY_ZPG = 0x84;
    static const byte STY_ZPX = 0x94;
    static const byte STY_ABS = 0x8C;

    static const byte TAX_IMP = 0xAA;
    static const byte TAY_IMP = 0xA8;
    static const byte TXA_IMP = 0x8A;
    static const byte TYA_IMP = 0x98;

    static const byte TSX_IMP = 0xBA;
    static const byte TXS_IMP = 0x9A;
    static const byte PHA_IMP = 0x48;
    static const byte PHP_IMP = 0x08;
    static const byte PLA_IMP = 0x68;
    static const byte PLP_IMP = 0x28;

    static const byte AND_IMM = 0x29;
    static const byte AND_ZPG = 0x25;
    static const byte AND_ZPX = 0x35;
    static const byte AND_ABS = 0x2D;
    static const byte AND_ABX = 0x3D;
    static const byte AND_ABY = 0x39;
    static const byte AND_INX = 0x21;
    static const byte AND_INY = 0x31;

    static const byte EOR_IMM = 0x49;
    static const byte EOR_ZPG = 0x45;
    static const byte EOR_ZPX = 0x55;
    static const byte EOR_ABS = 0x4D;
    static const byte EOR_ABX = 0x5D;
    static const byte EOR_ABY = 0x59;
    static const byte EOR_INX = 0x41;
    static const byte EOR_INY = 0x51;

    static const byte ORA_IMM = 0x09;
    static const byte ORA_ZPG = 0x05;
    static const byte ORA_ZPX = 0x15;
    static const byte ORA_ABS = 0x0D;
    static const byte ORA_ABX = 0x1D;
    static const byte ORA_ABY = 0x19;
    static const byte ORA_INX = 0x01;
    static const byte ORA_INY = 0x11;

    static const byte BIT_ZPG = 0x24;
    static const byte BIT_ABS = 0x2C;

    static const byte ADC_IMM = 0x69;
    static const byte ADC_ZPG = 0x65;
    static const byte ADC_ZPX = 0x75;
    static const byte ADC_ABS = 0x6D;
    static const byte ADC_ABX = 0x7D;
    static const byte ADC_ABY = 0x79;
    static const byte ADC_INX = 0x61;
    static const byte ADC_INY = 0x71;

    static const byte SBC_IMM = 0xE9;
    static const byte SBC_ZPG = 0xE5;
    static const byte SBC_ZPX = 0xF5;
    static const byte SBC_ABS = 0xED;
    static const byte SBC_ABX = 0xFD;
    static const byte SBC_ABY = 0xF9;
    static const byte SBC_INX = 0xE1;
    static const byte SBC_INY = 0xF1;

    static const byte CMP_IMM = 0xC9;
    static const byte CMP_ZPG = 0xC5;
    static const byte CMP_ZPX = 0xD5;
    static const byte CMP_ABS = 0xCD;
    static const byte CMP_ABX = 0xDD;
    static const byte CMP_ABY = 0xD9;
    static const byte CMP_INX = 0xC1;
    static const byte CMP_INY = 0xD1;

    static const byte CPX_IMM = 0xE0;
    static const byte CPX_ZPG = 0xE4;
    static const byte CPX_ABS = 0xEC;

    static const byte CPY_IMM = 0xC0;
    static const byte CPY_ZPG = 0xC4;
    static const byte CPY_ABS = 0xCC;

    static const byte INC_ZPG = 0xE6;
    static const byte INC_ZPX = 0xF6;
    static const byte INC_ABS = 0xEE;
    static const byte INC_ABX = 0xFE;

    static const byte DEC_ZPG = 0xC6;
    static const byte DEC_ZPX = 0xD6;
    static const byte DEC_ABS = 0xCE;
    static const byte DEC_ABX = 0xDE;

    static const byte INX_IMP = 0xE8;
    static const byte INY_IMP = 0xC8;

    static const byte DEX_IMP = 0xCA;
    static const byte DEY_IMP = 0x88;

    static const byte ASL_ACC = 0x0A;
    static const byte ASL_ZPG = 0x06;
    static const byte ASL_ZPX = 0x16;
    static const byte ASL_ABS = 0x0E;
    static const byte ASL_ABX = 0x1E;

    static const byte LSR_ACC = 0x4A;
    static const byte LSR_ZPG = 0x46;
    static const byte LSR_ZPX = 0x56;
    static const byte LSR_ABS = 0x4E;
    static const byte LSR_ABX = 0x5E;

    static const byte ROL_ACC = 0x2A;
    static const byte ROL_ZPG = 0x26;
    static const byte ROL_ZPX = 0x36;
    static const byte ROL_ABS = 0x2E;
    static const byte ROL_ABX = 0x3E;

    static const byte ROR_ACC = 0x6A;
    static const byte ROR_ZPG = 0x66;
    static const byte ROR_ZPX = 0x76;
    static const byte ROR_ABS = 0x6E;
    static const byte ROR_ABX = 0x7E;

    static const byte JMP_ABS = 0x4C;
    static const byte JMP_IND = 0x6C;
    static const byte JSR_ABS = 0x20;
    static const byte RTS_IMP = 0x60;

    static const byte BCC_REL = 0x90;
    static const byte BCS_REL = 0xB0;
    static const byte BEQ_REL = 0xF0;
    static const byte BMI_REL = 0x30;
    static const byte BNE_REL = 0xD0;
    static const byte BPL_REL = 0x10;
    static const byte BVC_REL = 0x50;
    static const byte BVS_REL = 0x70;

    static const byte CLC_IMP = 0x18;
    static const byte CLD_IMP = 0xD8;
    static const byte CLI_IMP = 0x58;
    static const byte CLV_IMP = 0xB8;
    static const byte SEC_IMP = 0x38;
    static const byte SED_IMP = 0xF8;
    static const byte SEI_IMP = 0x78;

    static const byte BRK_IMP = 0x00;
    static const byte NOP_IMP = 0xEA;
    static const byte RTI_IMP = 0x40;

    static const word RESET_VECT = 0xFFFC;
    static const word INTERRUPT_VECT = 0xFFFE;
    static const word NMI_VECT = 0xFFFA;
    int numWrites = 0;

    CPU6502();
    bool isReset = true;
    void reset(CPUBus& mem);
    void doNMI(CPUBus& mem, int cycles);
    void execute(CPUBus& mem, int cycles, int forever);
    byte genStatusFlag();
    void setStatusFlag(byte);

    CPUBus* bus;
    void clearInstruction();
    void printPastInstructions();
    byte fetchByte(CPUBus& mem, int& cycles);
    byte readByte(CPUBus& mem, int& cycles, word addr);
    byte readByte(word addr);
    word readWord(CPUBus& mem, int& cycles, word addr) const;
    word readWord(CPUBus& mem, word addr) const;
    void writeByte(CPUBus& mem, int& cycles, word addr, byte val);
    void writeWord(CPUBus& mem, int& cycles, word addr, word val) const;

    void pushSP(CPUBus& mem, int& cycles, word addr);
    word pullSP(CPUBus& mem, int& cycles);
    void pushByteSP(CPUBus& mem, int& cycles, byte val);
    byte pullByteSP(CPUBus& mem, int& cycles);

    void setPC(int& cycles, word val);
    int numInsts = 0;

    byte getimmval(CPUBus& mem, int& cycles);
    byte getzpgval(CPUBus& mem, int& cycles);
    byte getzpxval(CPUBus& mem, int& cycles);
    byte getzpyval(CPUBus& mem, int& cycles);
    byte getabsval(CPUBus& mem, int& cycles);
    byte getabxval(CPUBus& mem, int& cycles);
    byte getabyval(CPUBus& mem, int& cycles);
    byte getinxval(CPUBus& mem, int& cycles);
    byte getinyval(CPUBus& mem, int& cycles);

    word getzpgadr(CPUBus& mem, int& cycles);
    word getzpxadr(CPUBus& mem, int& cycles);
    word getzpyadr(CPUBus& mem, int& cycles);
    word getabsadr(CPUBus& mem, int& cycles);
    word getabxadr(CPUBus& mem, int& cycles);
    word getabyadr(CPUBus& mem, int& cycles);
    word getinxadr(CPUBus& mem, int& cycles);
    word getinyadr(CPUBus& mem, int& cycles);
    word getindadr(CPUBus& mem, int& cycles);
    word getreladr(CPUBus& mem, int& cycles);
};


#endif //NESEMU_CPU6502_H
