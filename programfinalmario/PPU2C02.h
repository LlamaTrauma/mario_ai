#ifndef NESEMU_PPU2C02_H
#define NESEMU_PPU2C02_H
#include "helper.h"
#include "PPUBus.h"

class PPU2C02 {
//https://emudev.de/nes-emulator/cartridge-loading-pattern-tables-and-ppu-registers/
public:
    //byte PPUCTRL = 0;
    union PPUCTRL {
        struct {
            //0: 0x2000, 1: 0x2400, 2: 0x2800, 3: 0x2C00
            byte nametableAddress : 2;
            byte vraminc : 1;
            //0: 0x0000, 1: 0x1000
            byte spritePatternTbl : 1;
            //0: 0x0000, 1: 0x1000
            byte backgrndPatternTbl : 1;
            //0: 8x8, 1: 8x16
            byte spriteSize : 1;
            byte masterselect : 1;
            byte nmigen : 1;
        };
        byte reg = 0;
    } control;
    //byte PPUMASK = 0;
    union PPUMASK {
        struct {
            byte greyscale : 1;
            byte backgroundleft : 1;
            byte spritesleft : 1;
            byte showbackground : 1;
            byte showsprites : 1;
            byte emphred : 1;
            byte emphgreen : 1;
            byte emphblue : 1;
        };
        byte reg = 0;
    } mask;
    //byte PPUSTATUS = 0;
    union PPUSTATUS {
        struct {
            byte unsure : 5;
            byte spriteoverflow : 1;
            byte sprite0hit : 1;
            byte vblank : 1;
        };
        byte reg = 0;
    } status;
    byte OAMADDR = 0;
    int timer = -1;
    byte OAMDATA = 0;
    byte PPUSCROLLX = 0;
    word PPUADDR = 0;
    word PPUADDR2 = 0;
    word ppuaddrbyte = 0;
    byte PPUDATA = 0;
    byte ppudatabuffer = 0;
    byte paletteTable[32];
    int numDraws = 0;
    byte scrollbyte = 0;
    int sprite0FirstHit = -1;
    byte oam[256];
    byte secondaryoamsize = 0;
    byte secondaryoambuffer = 0xFF;
    byte secondaryoam[32];
    std::fstream writeTo;
    //byte palette[64][3] = {{84, 84, 84}, {0, 30, 116}, {8, 16, 144}, {48, 0, 136}, {68, 0, 100}, {92, 0, 48}, {84, 4, 0}, {60, 24, 0}, {32, 42, 0}, {8, 58, 0}, {0, 64, 0}, {0, 60, 0}, {0, 50, 60}, {0, 0, 150}, {152, 8, 76}, {196, 48, 50}, {236, 92, 30}, {228, 136, 20}, {176, 160, 20}, {100, 152, 34}, {32, 120, 60}, {0, 84, 90}, {0, 40, 114}, {0, 8, 124}, {0, 0, 118}, {40, 0, 102}, {120, 0, 0}, {238, 236, 76}, {154, 236, 120}, {124, 236, 176}, {98, 236, 228}, {84, 236, 236}, {88, 180, 236}, {106, 100, 212}, {136, 32, 160}, {170, 0, 116}, {196, 0, 76}, {208, 32, 56}, {204, 108, 56}, {180, 204, 60}, {60, 238, 236}, {168, 204, 236}, {188, 188, 236}, {212, 178, 236}, {236, 174, 236}, {236, 174, 212}, {236, 180, 176}, {228, 196, 144}, {204, 210, 120}, {180, 222, 120}, {168, 226, 144}, {152, 226, 180}, {160, 214, 228}, {160, 162, 160}};
    byte palette[64][3];
    byte lastwassprite = 0;
    byte emojis[64][5] = 
    {
    "🕸", "🐋", "🌀", "👾", "👾", "🗼", "🛑", "🏺", "🍈", "📗", "📗", "📗", "🌀", "🌚", "🕶", "🌚", 
    "🦏", "🐋", "🚙", "💜", "👾", "🗼", "🛑", "🏺", "🍈", "🔋", "🔋", "🔋", "🎵", "🕶", "🌚", "🕶", 
    "🕛", "🐋", "🔮", "🌂", "🌂", "🍭", "🥔", "👋", "👌", "🌳", "🌳", "🌳", "🐦", "🕸", "🕶", "🌚", 
    "🕛", "😱", "😱", "🌂", "🍭", "🍭", "🥔", "🕛", "👌", "🌿", "🌿", "🌿", "😱", "🦏", "🌚", "🕶"};
    /*byte emojis[64][5] = 
    {"🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", 
    "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", 
    "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱",
    "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱", "🅱"};*/
    bool txtoutput = true;
    char txtOutput[256 * 256 * 10 + 1];
    char lastOutput[256 * 256 * 10 + 1];
    int outputLength = 0;
    int cyclemod = 4;
    float scanlinemod = 8;
    float scanlinectr = 0;
    int scanlinesdrawn = 0;
    int drawmod = 1;
    int scanlineon = 0;
    byte pixels[256 * 256 * 5];
    int scanline = -1;
    int cycle = 0;
    byte backgroundbit = 0;
    word backgroundsprite = 0;
    byte spritestodraw[256];
    byte numspritestodraw = 0;
    word spriteids[256];
    word spritepriors[256];
    word spritepixelvals[256];
    word spritepixelvalsr[256];
    word spritepixelvalsg[256];
    word spritepixelvalsb[256];
    byte backgroundspritebytes[2];
    bool doNMI = false;
    bool doNMI2 = false;
    PPUBus* bus;
    Memory* cpumem;
    byte drawBuffer[160][160];
    byte nameTable = 0;
    int cycleBalance = 0;

    byte nameTableByte = 0;
    byte finex = 0;
    word clrpos = 0;
    byte clrval = 0;
    double lastTime;
    double startTime;
    double last60[60];
    float secondsPerFrame = 0.0166667;
    int numTimes = 0;
    byte attributeLatch = 0;
    byte nametableLatch = 0;
    byte patternLowLatch = 0;
    byte patternHighLatch = 0;
    byte attributeTableLow = 0;
    byte attributeTableHigh = 0;
    word patternTableTileLow = 0;
    word patternTableTileHigh = 0;

    PPU2C02 makeCopy();
    PPU2C02();
    void testDraw();
    void updatePixel();
    void updatePosition(int cycles);
    byte readFromNametable(word addr);
    void draw();
    void drawAllTiles();
    void drawTile(byte x, byte y, byte num);
    void drawOAM();
    void doCycles(int cycles);
    void drawBackground();
    byte getPPUSTATUS();
    byte getPPUDATA();
    byte getOAMDATA();
    void setPPUCTRL(byte data);
    void setPPUMASK(byte data);
    void setOAMADDR(byte data);
    void setOAMDATA(byte data);
    void setPPUSCROLL(byte data);
    void setPPUADDR(byte data);
    void setPPUDATA(byte data);
    void doOAMDMA(byte data);
};


#endif //NESEMU_PPU2C02_H
