//
// Created by Folsense on 4/14/21.
//
#ifndef EMULATOR_CPP
#define EMULATOR_CPP
#include "Emulator.h"
#include <climits>
#include <thread>
#include <chrono>

Emulator::Emulator(){
    
}

void Emulator::reset(){
    mem.reset();
    ppumem.reset();
    cpu.reset(cpubus);
}

void Emulator::start(){
    rom.read(); 
    cpubus.rom = &rom;
    cpubus.mem = &mem;
    ppubus.rom = &rom;
    ppubus.mem = &ppumem;
    cpubus.ppu = &ppu;
    cpubus.player1 = &player1;
    ppu.bus = &ppubus;
    ppu.cpumem = &mem;
    ppubus.cpumem = &mem;
    cpu.bus = &cpubus;
    reset();
    //ppu.testDraw();

    /*cpu.pc = 0x500;
    cpu.a = 0x17;
    cpu.x = 0x14;
    cpu.y = 0x7f;
    byte sp = cpu.sp;
    //cpubus.cpuWrite(0x0142, 0x42);
    mem.data[cpu.pc] = CPU6502::SBC_IMM;
    cpubus.cpuWrite(0x114, 0x01);
    cpubus.cpuWrite(0x115, 0x02);
    std::cout << "value: " << std::hex << int(cpu.a) << '\n';
    mem.data[cpu.pc + 1] = 0x12;
    mem.data[cpu.pc + 2] = 0x01;
    cpu.execute(cpubus, 1);
    std::cout << "value: " << std::hex << int(cpu.a) << '\n';*/
    //cpu.execute(cpubus, 2000000);
}

void Emulator::step(){
    cpu.execute(cpubus, 2000, 1);
}

#endif