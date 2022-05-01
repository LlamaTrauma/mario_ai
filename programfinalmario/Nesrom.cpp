#include <bitset>
#include "Nesrom.h"

Nesrom::Nesrom(){
    //romFile.open("../roms/mariobros.nes");
    //Using binary mode to make Windows work
    romFile.open("C:/Users/gpoch/Documents/gabe/Gabes_Stuff/nesemu/programfinalmario/mariobros.nes", std::ios::in | std::ios::binary);
    //romFile.open("./golf.nes", std::ios::in | std::ios::binary);
    romFile.clear();
    romFile.seekg(0, std::ios::beg);
}

void Nesrom::read (){
    //Read four magic bytes
    std::cout << "Position: " << romFile.tellg() << '\n';
    romFile.read(magic, 4);
    std::cout << "Magic: " << magic << '\n';
    romFile >> prgromlen >> chrromlen >> flags6 >> flags7 >> flags8 >> flags9 >> flags10;
    std::cout << "PRG ROM length (in 16kb blocks): " << int(prgromlen) << '\n';
    std::cout << "CHR ROM length (in 8kb blocks): " << int(chrromlen) << '\n';
    std::cout << "flags6: " << std::bitset<8>(flags6) << '\n';
    std::cout << "flags7: " << std::bitset<8>(flags7) << '\n';
    std::cout << "flags8: " << std::bitset<8>(flags8) << '\n';
    std::cout << "flags9: " << std::bitset<8>(flags9) << '\n';
    std::cout << "flags10: " << std::bitset<8>(flags10) << '\n';
    romFile.read(padding, 5);
    romFile.read(prgromchar, 16384 * prgromlen);
    for(int i = 0; i < 16384 * prgromlen; i ++){
        prgrom[i] = static_cast<byte>(0xFF) & static_cast<char>(prgromchar[i]);
    }
    romFile.read(chrromchar, 8192 * chrromlen);
    for(int i = 0; i < 8192 * chrromlen; i ++){
        chrrom[i] = static_cast<byte>(0xFF) & static_cast<char>(chrromchar[i]);
    }
    romFile.close();
}