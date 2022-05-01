#ifndef MEMORY_H
#define MEMORY_H
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "Cartridge.h"
#include "Mapper.h"
#include <fstream>

namespace sn
{
    enum IORegisters
    {
        PPUCTRL = 0x2000,
        PPUMASK,
        PPUSTATUS,
        OAMADDR,
        OAMDATA,
        PPUSCROL,
        PPUADDR,
        PPUDATA,
        OAMDMA = 0x4014,
        JOY1 = 0x4016,
        JOY2 = 0x4017,
        //Not an IORegister but need a callback and this seems like the simplest way to add one
        //Also this is actually ScrollThirtyTwo
        CURRENTCOLUMNPOS = 0x073D
    };

    class MainBus
    {
        public:
            MainBus();
            void operator=(const MainBus& bus);
            Byte read(Address addr);
            void write(Address addr, Byte value);
            bool setMapper(Mapper* mapper);
            bool setWriteCallback(IORegisters reg, std::function<void(Byte)> callback);
            bool setReadCallback(IORegisters reg, std::function<Byte(void)> callback);
            const Byte* getPagePtr(Byte page);
            void toFile(std::string fname);
            void fromFile(std::string fname);
        private:
            std::vector<Byte> m_RAM;
            std::vector<Byte> m_extRAM;
            Mapper* m_mapper;

            std::map<IORegisters, std::function<void(Byte)>> m_writeCallbacks;
            std::map<IORegisters, std::function<Byte(void)>> m_readCallbacks;;
    };
};

#endif // MEMORY_H
