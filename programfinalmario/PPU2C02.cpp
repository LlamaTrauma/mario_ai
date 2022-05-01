#include "PPU2C02.h"

PPU2C02::PPU2C02(){
    //std::cout << "Status Register " << int(status.reg) << '\n';
    for(int i = 0; i < 256; i ++){
        oam[i] = 1;
    }
    for(int i = 0; i < 256 * 256 * 10 + 1; i ++){
        lastOutput[i] = '?';
    }
    std::ifstream paletteRead;
    paletteRead.open("C:/Users/gpoch/Documents/gabe/Gabes_Stuff/nesemu/programfinalmario/ntscpalette.pal");
    std::cout << "Reading\n";
    unsigned char val;
    float val2;
    char buffer[192];
    paletteRead.read(buffer, 192);
    for(int i = 0; i < 64; i ++){
        for(int j = 0; j < 3; j ++){
            palette[i][j] = buffer[i * 3 + j];
            //val2 = static_cast<float>(val);
            //std::cout << int(val2) << '\n';
            //val2 /= 256;
            //val2 = pow(val2, 2.2);
            //val2 *= 256;
            //std::cout << static_cast<int>(val2) << '\n';
            //palette[i][j] = static_cast<byte>(val2);
        }
    }
    for(int i = 0; i < 60; i ++){
        last60[i] = 0;
    }
    paletteRead.close();
    startTime = currentTimeDouble();
    lastTime =  currentTimeDouble() - startTime;
}

void PPU2C02::testDraw(){
    for(word i = 0; i < 160; i ++){
        for(word j = 0; j < 160; j ++){
            byte l;
            if(j % 4 == 0){
                l = 1;
            } else if (j % 4 == 1) {
                l = 4;
            } else if (j % 4 == 2) {
                l = 16;
            } else if (j % 4 == 3) {
                l = 64;
            }
            drawBuffer[i][j] = bus->ppuRead(i * 40 + j / 4) % (l * 4) / l;
        }
    }
}

void PPU2C02::draw(){
    if(txtoutput){
        if(numDraws % drawmod == 0){
	        //txtOutput[outputLength] = '\0';
            //std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
            /*std::cout << "/ ___)/ )( \\(  _ \\(  __)(  _ \\  / )( \\(  ( \\(  )/ __)/  \\(    \\(  __)  (  _ \\(  _ \\ /  \\ / ___)\n";
            std::cout << "\\___ \\) \\/ ( ) __/ ) _)  )   /  ) \\/ (/    / )(( (__(  O )) D ( ) _)    ) _ ( )   /(  O )\\___ \\\n";
            std::cout << "(____/\\____/(__)  (____)(__\\_)  \\____/\\_)__)(__)\\___)\\__/(____/(____)  (____/(__\\_) \\__/ (____/\n\n";*/
            /*std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"; 
            std::cout << "  _____                                   _    _           _                      _            ____" << '\n';
            std::cout << "  / ____|                                 | |  | |         (_)                    | |          |  _ \\" << '\n';
            std::cout << " | (___    _   _   _ __     ___   _ __    | |  | |  _ __    _    ___    ___     __| |   ___    | |_) |  _ __    ___    ___" << '\n';
            std::cout << "  \\___ \\  | | | | | '_ \\   / _ \\ | '__|   | |  | | | '_ \\  | |  / __|  / _ \\   / _` |  / _ \\   |  _ <  | '__|  / _ \\  / __|" << '\n';
            std::cout << "  ____) | | |_| | | |_) | |  __/ | |      | |__| | | | | | | | | (__  | (_) | | (_| | |  __/   | |_) | | |    | (_) | \\__ \\  _" << '\n';
            std::cout << " |_____/   \\__,_| | .__/   \\___| |_|       \\____/  |_| |_| |_|  \\___|  \\___/   \\__,_|  \\___|   |____/  |_|     \\___/  |___/ (_)" << '\n';
            std::cout << "                  | |" << '\n';
            std::cout << "                  |_|" << '\n';*/


            /*int score = bus->cpuRead(0x7DD) * 10000000 + bus->cpuRead(0x7DE) * 1000000 + bus->cpuRead(0x7DF) * 100000 + bus->cpuRead(0x7E0) * 1000 + bus->cpuRead(0x7E1) * 100 + bus->cpuRead(0x7E2) * 10;
            int time = bus->cpuRead(0x7F8) * 100 + bus->cpuRead(0x7F9) * 10 + bus->cpuRead(0x7FA);
            std::cout << std::left << "SCORE: " << std::setw(10) << score << "COINS: " << std::setw(5) << int(bus->cpuRead(0x75E)) << "WORLD: " << int(bus->cpuRead(0x75F)) << " - " << std::setw(5) << int(bus->cpuRead(0x760)) << "TIME: " << std::setw(5) << time << "LIVES: " << std::setw(5) << int(bus->cpuRead(0x75A)) << "\n";*/
            int i = 0;
            char newOutput[5000];
            int newOutputLength = 0;
            COORD pos = {0, 0};
            HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
            //SetConsoleCursorPosition(output, pos);
            if(numDraws == 0){
                system("cls");
            }
            int differences = 0;
            for(int y = 0; y < 240 / scanlinemod; y += 1){
                for(int x = 0; x < 256 / cyclemod + 1; x += 1){
                    if(lastOutput[i] == txtOutput[i] && newOutputLength == 0){
                        i += 1;
                        continue;
                    }
                    if(lastOutput[i] != txtOutput[i]){
                        differences += 1;
                        lastOutput[i] = txtOutput[i];
                    }
                    if(newOutputLength == 0){
                        pos = {x, y};
                    }
                    newOutput[newOutputLength++] = txtOutput[i];
                    if(newOutputLength >= 500){
                        if(differences > 0){
                            SetConsoleCursorPosition(output, pos);
                            newOutput[newOutputLength] = '\0';
                            std::cout << newOutput;
                            differences = 0;
                        }
                        newOutputLength = 0;
                    }
                    i += 1;
                }    
            }
            
            if(differences > 0){
                SetConsoleCursorPosition(output, pos);
                newOutput[newOutputLength] = '\0';
                std::cout << newOutput;
            }
            
            outputLength = 0;
            numDraws += 1;
        }
        //writeTo.close();
        //system("cat out2.txt");
    }
    numDraws += 1;
    //}
        //std::cout << "NEW DRAW\n\n\n";
    /*    for(int i = 0; i < 0x800; i ++){
            byte num = bus->ppuRead(0x2000 + i);
            word k = 3 * (num / 32);
            word j = 3 * (num % 32);
            drawTile(j, k, num);
    }
    //}
        //drawBackground();
    

    //drawBackground();
    /*if(timer > -1){
        timer -= 1;
    }
    if(timer == 0){
        exit(1);
    }*/
}

void PPU2C02::doCycles(int cycles){
    //https://wiki.nesdev.com/w/index.php/PPU_rendering#Cycle_0
    cycleBalance += cycles;
    while(cycleBalance > 0){
    if(scanline == -1){
        if(cycle == 0){
            cycle += 1;
            cycleBalance -= 1;
            status.sprite0hit = 0;
            sprite0FirstHit = -1;
            //Hacked in for Mario Bros
            control.nametableAddress = 0;
        } else if (cycle <= 256){
            cycle += 8;
            cycleBalance -= 8; 
        } else if (cycle <= 320){
            cycle += 64;
            cycleBalance -= 64;
        } else if (cycle <= 336){
            word x = PPUSCROLLX;
            word pos = (x % 0xFF) / 8 + ((scanline + 1) / 8) * 32;
            while(x > 0xFF){
                pos += 0x400;
                x -= 0xFF;
            }
            nameTableByte = bus->ppuRead(0x2000 + pos + control.nametableAddress * 0x400);
            attributeTableLow = bus->ppuRead(0x2000 + pos + control.nametableAddress * 0x400);
            patternTableTileLow = bus->ppuRead(nameTableByte * 16 + (scanline + 1) % 8 + 0x1000) << 8;
            patternTableTileHigh = bus->ppuRead(nameTableByte * 16 + 8 + (scanline + 1) % 8 + 0x1000) << 8;
            attributeTableLow;

            x = 8 + PPUSCROLLX;
            pos = (x % 0x100) / 8 + ((scanline + 1) / 8) * 32;
            while(x > 0xFF){
                pos += 0x400;
                x -= 0xFF;
            }
            nameTableByte = bus->ppuRead(0x2000 + pos + control.nametableAddress * 0x400);
            patternTableTileLow |= bus->ppuRead(nameTableByte * 16 + (scanline + 1) % 8 + 0x1000);
            patternTableTileHigh |= bus->ppuRead(nameTableByte * 16 + 8 + (scanline + 1) % 8 + 0x1000);
            attributeTableLow;

            cycle += 16;
            cycleBalance -= 16;
        } else if(cycle <= 340){
            cycle += 4;
            cycleBalance -= 4;
        } else {
            byte spriteid = 0;
            numspritestodraw = 0;
            for(int j = 0; j < 256; j ++){
                spritepixelvals[j] = 0xFF;
                spritepixelvalsr[j] = 0xFF;
                spritepixelvalsg[j] = 0xFF;
                spritepixelvalsb[j] = 0xFF;
                spriteids[j] = 0x200;
            }
            for(int j = OAMADDR; j < 256; j += 4){
                byte ypos = oam[j];
                if(ypos <= scanline + 1 && ypos + 7 >= scanline + 1 && numspritestodraw < 8){
                    byte yoff = scanline + 1 - ypos;
                    byte byte0 = bus->ppuRead(oam[j + 1] * 16 + yoff + 0x1000 * control.spritePatternTbl);
                    byte byte1 = bus->ppuRead(oam[j + 1] * 16 + yoff + 8 + 0x1000 * control.spritePatternTbl);     
                    spritestodraw[numspritestodraw++] = j;
                    for(int k = 0; k < 8; k ++){
                        byte xoff = 7 - k;
                        if(oam[j + 2] & 0b01000000){
                            xoff = k;
                        }
                        byte clrvalsprite = (byte0 >> k & 1) + 2 * (byte1 >> k & 1);
                        byte paletteIndex;
                        if(clrvalsprite == 0){
                            continue;
                        } else{
                            paletteIndex = bus->ppuRead(0x3F11 + 4 * (oam[j + 2] & 0b00000011) + clrvalsprite - 1);
                        }
                        //std::cout << "Sprite index is " << int(paletteIndex) << '\n';
                        spriteids[static_cast<byte>(xoff + oam[j + 3]) & 0xff] = spriteid;
                        spritepixelvals[static_cast<byte>(xoff + oam[j + 3]) & 0xff] = paletteIndex;
                        spritepixelvalsr[static_cast<byte>(xoff + oam[j + 3]) & 0xff] = palette[paletteIndex][0];
                        spritepixelvalsg[static_cast<byte>(xoff + oam[j + 3]) & 0xff] = palette[paletteIndex][1];
                        spritepixelvalsb[static_cast<byte>(xoff + oam[j + 3]) & 0xff] = palette[paletteIndex][2];
                    }
                }
                spriteid += 1;
            }
            scanline += 1;
            cycle = 0;
        }
    } 
    else if (scanline <= 239){
        if(cycle == 0){
            //Hacked in for Mario Bros
            if(scanline == 30){
                status.sprite0hit = 1;
            }
            cycle += 1;
            cycleBalance -= 1;
            status.spriteoverflow = 0;
        } else if (cycle <= 256){
            word x = cycle + 15 + PPUSCROLLX;
            word pos = (x % 0x100) / 8 + (scanline / 8) * 32;
            while(x > 0xFF){
                pos += 0x400;
                x -= 0xFF;
            }
            nameTableByte = bus->ppuRead(0x2000 + pos + control.nametableAddress * 0x400);
            /*attributeLatch = bus->ppuRead(0x2000 + control.nametableAddress * 0x400 + 0x3C0 + (x + 1) / 32 + (scanline / 32) * 8);
            if((x + 1) % 32 > 15){
                attributeLatch >>= 2;
            }
            if(scanline % 32 > 15){
                attributeLatch >>= 4;
            }*/
            patternLowLatch = bus->ppuRead(nameTableByte * 16 + scanline % 8 + 0x1000);
            patternHighLatch = bus->ppuRead(nameTableByte * 16 + 8 + scanline % 8 + 0x1000);
            signed char pos2 = -1;
            byte paletteInd = 0;
            byte clr = 0;
            byte type = 0;
            for(int i = PPUSCROLLX % 8; i < PPUSCROLLX % 8 + 8; i ++){
                paletteInd = 0;
                type = 0;
                if(spriteids[cycle + pos2] != 0x200){
                    paletteInd = spritepixelvals[cycle + pos2];
                }   
                clr = ((patternTableTileLow & (0x8000 >> i)) > 0) + 2 * ((patternTableTileHigh & (0x8000 >> i)) > 0);
                int nowclrpos = (cycle + PPUSCROLLX + pos2) / 16 + (scanline / 16) * 16;
                //if(nowclrpos != clrpos){
                int readPaletteFrom = cycle + PPUSCROLLX + pos2;
                word add2 = 0;
                while(readPaletteFrom >= 0x100){
                    readPaletteFrom -= 0x100;
                    add2 += 0x400;
                }
                int finalPosition = 0x2000 + control.nametableAddress * 0x400 + 0x3C0 + (readPaletteFrom) / 32 + add2 + (scanline / 32) * 8;
                clrval = bus->ppuRead(finalPosition);
                if((cycle + PPUSCROLLX + pos2) % 32 > 15){
                    clrval >>= 2;
                }
                if(scanline % 32 > 15){
                    clrval >>= 4;
                }
                clrval &= 0x3;
                //}
                byte paletteIndex;
                if(clr == 0){
                    paletteIndex = bus->ppuRead(0x3F00);
                } else {
                    paletteIndex = bus->ppuRead(0x3F01 + clrval * 4 + clr - 1);
                }
                /*if(spriteids[cycle + pos2] == 0 && clr != 0){
                    if(sprite0FirstHit == -1){
                        sprite0FirstHit = scanline;
                    } else if(sprite0FirstHit != scanline) {
                        status.sprite0hit = 1;
                    }
                }*/
                type = 1;
                if(spriteids[cycle + pos2] == 0x200){
                    paletteInd = paletteIndex;
                    type = 1;
                } else if (spritepriors[cycle + pos2] == 1 && clr != 0){
                    paletteInd = paletteIndex;
                    type = 1;
                }
                if(txtoutput){
                    if(lastwassprite > 0){
                        lastwassprite -= 1;
                    } else if((cycle + pos2) % cyclemod == (PPUSCROLLX % cyclemod) && scanline >= scanlinectr && numDraws % drawmod == 0){
                        if(type == 0){
                            for(int i = 0; i < 4; i ++){
                                txtOutput[outputLength++] = emojis[paletteInd][i];
                            }
                            lastwassprite = cyclemod;
                        } else {
                            //float grey = pow(static_cast<float>(palette[paletteInd][0] * palette[paletteInd][0]) + static_cast<float>(palette[paletteInd][1] * palette[paletteInd][1]) + static_cast<float>(palette[paletteInd][2] * palette[paletteInd][2]), 0.5);
                            /*float grey = palette[paletteInd][0];
                            if(palette[paletteInd][1] > grey){
                                grey = palette[paletteInd][1];
                            }
                            if(palette[paletteInd][2] > grey){
                                grey = palette[paletteInd][2];
                            }*/

                            float grey = (palette[paletteInd][0] + palette[paletteInd][1] + palette[paletteInd][2]) / 3;
                            int ind = static_cast<int>(grey * 10 / 255);
                            if(ind > 9){
                                ind = 9;
                            }
                            if(ind < 0){
                                ind = 0;
                            }
                            if(clr == 0 && (spriteids[cycle + pos2] == 0x200)){
                                txtOutput[outputLength++] = ' ';
                            } else if(scanline <= 30){
                                txtOutput[outputLength++] = "123456*^c+=#&M"[ind];
                            } else if(palette[paletteIndex][0] > palette[paletteIndex][1]){
                                txtOutput[outputLength++] = " .:-=+*#%@"[ind];
                            } else if(palette[paletteIndex][1] > palette[paletteIndex][2]){
                                txtOutput[outputLength++] = " .:-=+*#%@"[ind];
                            } else {
                                txtOutput[outputLength++] = " .:-=+*#%@"[ind];
                            }
                        }
                        /*char colorstring[7] = "\e[31m";
                        for(int chr = 0; chr < 6; chr ++){
                            txtOutput[outputLength++] = colorstring[chr];
                        }*/
                        
                        /*char colorstring2[6] = "\e[0m";
                        for(int chr = 0; chr < 5; chr ++){
                            txtOutput[outputLength++] = colorstring[chr];
                        }*/
                    }
                } else {
                    pixels[(scanline * 256 + cycle + pos2) * 4] = palette[paletteInd][0];
                    pixels[(scanline * 256 + cycle + pos2) * 4 + 1] = palette[paletteInd][1];
                    pixels[(scanline * 256 + cycle + pos2) * 4 + 2] = palette[paletteInd][2];
                    pixels[(scanline * 256 + cycle + pos2) * 4 + 3] = 255;
                }
                pos2 += 1;
            }

            patternTableTileLow <<= 8;
            patternTableTileHigh <<= 8;
            patternTableTileLow |= patternLowLatch;
            patternTableTileHigh |= patternHighLatch;
            //attributeTableLow <<= 8;
            //attributeTableLow |= attributeLatch;

            cycle += 8;
            cycleBalance -= 8;
        } else if (cycle <= 320){
            cycle += 64;
            cycleBalance -= 64;
        } else if (cycle <= 336){
            word x = PPUSCROLLX;
            word pos = (x % 0xFF) / 8 + ((scanline + 1) / 8) * 32;
            while(x > 0xFF){
                pos += 0x400;
                x -= 0xFF;
            }
            nameTableByte = bus->ppuRead(0x2000 + pos + control.nametableAddress * 0x400);
            attributeTableLow = bus->ppuRead(0x2000 + pos + control.nametableAddress * 0x400);
            patternTableTileLow = bus->ppuRead(nameTableByte * 16 + (scanline + 1) % 8 + 0x1000) << 8;
            patternTableTileHigh = bus->ppuRead(nameTableByte * 16 + 8 + (scanline + 1) % 8 + 0x1000) << 8;
            attributeTableLow;

            x = 8 + PPUSCROLLX;
            pos = (x % 0x100) / 8 + ((scanline + 1) / 8) * 32;
            while(x > 0xFF){
                pos += 0x400;
                x -= 0xFF;
            }
            nameTableByte = bus->ppuRead(0x2000 + pos + control.nametableAddress * 0x400);
            patternTableTileLow |= bus->ppuRead(nameTableByte * 16 + (scanline + 1) % 8 + 0x1000);
            patternTableTileHigh |= bus->ppuRead(nameTableByte * 16 + 8 + (scanline + 1) % 8 + 0x1000);
            attributeTableLow;

            cycle += 16;
            cycleBalance -= 16;
        } else if(cycle <= 340){
            cycle += 4;
            cycleBalance -= 4;
        } else {
            byte spriteid = 0;
            numspritestodraw = 0;
            for(int j = 0; j < 256; j ++){
                spritepixelvals[j] = 0xFF;
                spritepriors[j] = 1;
                spritepixelvalsr[j] = 0xFF;
                spritepixelvalsg[j] = 0xFF;
                spritepixelvalsb[j] = 0xFF;
                spriteids[j] = 0x200;
            }
            for(int j = OAMADDR; j < 256; j += 4){
                byte ypos = oam[j];
                if(ypos <= scanline + 1 && ypos + 7 >= scanline + 1){
                    byte yoff = scanline + 1 - ypos;
                    if(oam[j + 2] & 0b10000000){
                        yoff = 7 - yoff;
                    }
                    byte byte0 = bus->ppuRead(oam[j + 1] * 16 + yoff + 0x1000 * control.spritePatternTbl);
                    byte byte1 = bus->ppuRead(oam[j + 1] * 16 + yoff + 8 + 0x1000 * control.spritePatternTbl);     
                    spritestodraw[numspritestodraw++] = j;
                    for(int k = 0; k < 8; k ++){
                        byte xoff = 7 - k;
                        if(oam[j + 2] & 0b01000000){
                            xoff = k;
                        }
                        byte clrvalsprite = (byte0 >> k & 1) + 2 * (byte1 >> k & 1);
                        byte paletteIndex;
                        if(clrvalsprite != 0 && (xoff + oam[j + 3] < 0x100)){
                            paletteIndex = bus->ppuRead(0x3F11 + 4 * (oam[j + 2] & 0b00000011) + clrvalsprite - 1);
                            spriteids[static_cast<byte>(xoff + oam[j + 3]) & 0xff] = spriteid;
                            spritepriors[xoff + oam[j + 3]] = (oam[j + 2] >> 5) & 0x1;
                            spritepixelvals[static_cast<byte>(xoff + oam[j + 3]) & 0xff] = paletteIndex;
                            spritepixelvalsr[static_cast<byte>(xoff + oam[j + 3]) & 0xff] = palette[paletteIndex][0];
                            spritepixelvalsg[static_cast<byte>(xoff + oam[j + 3]) & 0xff] = palette[paletteIndex][1];
                            spritepixelvalsb[static_cast<byte>(xoff + oam[j + 3]) & 0xff] = palette[paletteIndex][2];
                        }
                    }
                }
                spriteid += 1;
            }
            if(scanline >= scanlinectr && numDraws % drawmod == 0){
                //Commented out because only apply to Mario Bros
                /*int score = bus->cpuRead(0x7DD) * 10000000 + bus->cpuRead(0x7DE) * 1000000 + bus->cpuRead(0x7DF) * 100000 + bus->cpuRead(0x7E0) * 1000 + bus->cpuRead(0x7E1) * 100 + bus->cpuRead(0x7E2) * 10;
                int time = bus->cpuRead(0x7F8) * 100 + bus->cpuRead(0x7F9) * 10 + bus->cpuRead(0x7FA);
                std::cout << std::left << "SCORE: " << std::setw(10) << score << "COINS: " << std::setw(5) << int(bus->cpuRead(0x75E)) << "WORLD: " << int(bus->cpuRead(0x75F)) << " - " << std::setw(5) << int(bus->cpuRead(0x760)) << "TIME: " << std::setw(5) << time << "LIVES: " << std::setw(5) << int(bus->cpuRead(0x75A)) << "\n";*/
                /*if(scanlinesdrawn == 10){
                    int score = bus->cpuRead(0x7DD) * 10000000 + bus->cpuRead(0x7DE) * 1000000 + bus->cpuRead(0x7DF) * 100000 + bus->cpuRead(0x7E0) * 1000 + bus->cpuRead(0x7E1) * 100 + bus->cpuRead(0x7E2) * 10;
                    for(int i = 0; i < 8; i ++){
                        txtOutput[outputLength++] = " SCORE: "[i];
                    }
                    std::string scoreStr = std::to_string(score);
                    for(int i = 0; i < scoreStr.length(); i ++){
                        txtOutput[outputLength++] = scoreStr[i];
                    }
                } else if(scanlinesdrawn == 12){
                    for(int i = 0; i < 8; i ++){
                        txtOutput[outputLength++] = " COINS: "[i];
                    }
                    std::string coinStr = std::to_string(bus->cpuRead(0x75E));
                    for(int i = 0; i < coinStr.length(); i ++){
                        txtOutput[outputLength++] = coinStr[i];
                    }
                } else if(scanlinesdrawn == 14){
                    for(int i = 0; i < 8; i ++){
                        txtOutput[outputLength++] = " WORLD: "[i];
                    }
                    std::string worldStr = std::to_string(bus->cpuRead(0x75F));
                    std::string levelStr = std::to_string(bus->cpuRead(0x760));

                    for(int i = 0; i < worldStr.length(); i ++){
                        txtOutput[outputLength++] = worldStr[i];
                    }
                    for(int i = 0; i < 3; i ++){
                        txtOutput[outputLength++] = " - "[i];
                    }
                    for(int i = 0; i < levelStr.length(); i ++){
                        txtOutput[outputLength++] = levelStr[i];
                    }
                } else if(scanlinesdrawn == 16){
                    for(int i = 0; i < 7; i ++){
                        txtOutput[outputLength++] = " TIME: "[i];
                    }
                    int time = bus->cpuRead(0x7F8) * 100 + bus->cpuRead(0x7F9) * 10 + bus->cpuRead(0x7FA);
                    std::string timeStr = std::to_string(time);
                    for(int i = 0; i < timeStr.length(); i ++){
                        txtOutput[outputLength++] = timeStr[i];
                    }
                } else if(scanlinesdrawn == 18){
                    for(int i = 0; i < 8; i ++){
                        txtOutput[outputLength++] = " LIVES: "[i];
                    }
                    std::string livesStr = std::to_string(bus->cpuRead(0x75A));
                    for(int i = 0; i < livesStr.length(); i ++){
                        txtOutput[outputLength++] = livesStr[i];
                    }
                } else if(scanlinesdrawn == 20){
                    for(int i = 0; i < 11; i ++){
                        txtOutput[outputLength++] = " POWER UP: "[i];
                    }
                    std::string powerUpStr = std::to_string(bus->cpuRead(0x756));
                    for(int i = 0; i < powerUpStr.length(); i ++){
                        txtOutput[outputLength++] = powerUpStr[i];
                    }
                } else if(scanlinesdrawn == 22){
                    for(int i = 0; i < 12; i ++){
                        txtOutput[outputLength++] = " A/B ARE Z/X"[i];
                    }
                } else if(scanlinesdrawn == 24){
                    for(int i = 0; i < 11; i ++){
                        txtOutput[outputLength++] = " START IS C"[i];
                    }
                } else if(scanlinesdrawn == 26){
                    for(int i = 0; i < 12; i ++){
                        txtOutput[outputLength++] = " SELECT IS V"[i];
                    }
                } else if(scanlinesdrawn == 28){
                    for(int i = 0; i < 16; i ++){
                        txtOutput[outputLength++] = " D-PAD IS ARROWS"[i];
                    }
                } else if(scanlinesdrawn == 30){
                    for(int i = 0; i < 10; i ++){
                        txtOutput[outputLength++] = " Q TO QUIT"[i];
                    }
                } else if(scanlinesdrawn == 31){
                    for(int i = 0; i < 16; i ++){
                        txtOutput[outputLength++] = " (AND LOOK BUSY)"[i];
                    }
                }*/
                scanlinesdrawn += 1;
                txtOutput[outputLength++] = '\n';
                scanlinectr += scanlinemod;
            }
            scanline += 1;
            cycle = 0;
        }
    } 
    else if(scanline == 240){
        if(cycle < 340){
            cycle += 10;
            cycleBalance -= 10;
        } else {
            cycle = 0;
            scanline += 1;
        }
    } 
    else if(scanline == 241){
        if(cycle < 340){
            cycle += 1;
            cycleBalance -= 1;
            if(cycle == 2){
                status.vblank = 1;
                if(control.nmigen > 0){
                    doNMI = true;
                }
                doNMI2 = true;
                draw();
                double thisTime = currentTimeDouble() - startTime;
                last60[numTimes % 60] = thisTime;
                numTimes += 1;
                double toSleep = secondsPerFrame * 1000 - (thisTime - lastTime);
                lastTime = thisTime;
                /*if(numTimes > 59){
                    //toSleep = secondsPerFrame * 1000 - (thisTime - lastTime);
                    //toSleep2 = static_cast<int>(secondsPerFrame * 1000 * 60) - (thisTime - last60[numTimes % 60]);
		            //toSleep = static_cast<int>(secondsPerFrame * 1000 * 60) - (thisTime - last60[numTimes % 60]);
                } else {
                    //toSleep = static_cast<int>(secondsPerFrame * 1000) - (thisTime - lastTime);
                    //toSleep2 = static_cast<int>(secondsPerFrame * 1000 * numTimes) - (thisTime - last60[numTimes % 60]);
                    //toSleep = static_cast<int>(secondsPerFrame * 1000 * numTimes) - (thisTime - last60[numTimes % 60]);
                }*/
                    /*if(toSleep2 > toSleep){
                        toSleep = toSleep2;
                    }*/
                    //if(toSleep > 0 && toSleep < 1000){
                        //std::this_thread::sleep_for(std::chrono::milliseconds(toSleep));
                        //using namespace std;
                        /*#ifdef LINUX
                            usleep(toSleep * 1000);   // usleep takes sleep time in us (1 millionth of a second)
                        #endif
                        #ifdef WINDOWS
                            Sleep(toSleep);
                        #endif*/
                        //std::cout.flush();
                        //std::cout << "Sdfsd: " << int(toSleep) << '\n';
                        preciseSleep(toSleep);
                        /*while((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startTime) < thisTime + toSleep){
                            std::cout << "asd" << (thisTime + toSleep) << '\n' << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startTime << '\n';
                        }*/
                        //sleep(toSleep);
                    //}
                    //std::cout << std::dec << 60.0 / (thisTime - last60[numTimes % 60]) * 1000 << '\n';
                //}
            }
        } else {
            cycle = 0;
            scanline += 1;
        }
    } 
    else if(scanline <= 260){
        if(cycle < 340){
            cycle += 10;
            cycleBalance -= 10;
        } else {
            cycle = 0;
            scanline += 1;
        }
    } else {
        scanline = -1;
        scanlinectr = 0;
        scanlinesdrawn = 0;
    }
    }
}

PPU2C02 PPU2C02::makeCopy(){

}

byte PPU2C02::readFromNametable(word addr){
    //control.nametableAddress = 1;
    //std::cout << "Returns: " << std::hex << int(bus->ppuRead(0x2000 + control.nametableAddress * 0x400 + addr)) << '\n';
    return bus->ppuRead(0x2000 + control.nametableAddress * 0x400 + addr);
}

byte PPU2C02::getPPUSTATUS(){
    /*if(status.vblank == 1){
        std::cout << "VBLANK IS SET\n\n\n\n\n";
        timer = 10;
    }*/
    //status.spriteoverflow = 1;
    //status.sprite0hit = 1;
    //std::cout << "Status read " << '\n';
    byte toreturn = status.reg;
    status.vblank = 0;
    ppuaddrbyte = 0;
    scrollbyte = 0;
    return toreturn;
}
byte PPU2C02::getPPUDATA(){
    if(PPUADDR - 1 >= 0x3f00){
        byte toReturn = bus->ppuRead(PPUADDR);
        if(control.vraminc == 0){
            PPUADDR += 1;
        } else{
            PPUADDR += 0x20;
        }
        return toReturn;
    }
    byte toreturn = ppudatabuffer;
    ppudatabuffer = bus->ppuRead(PPUADDR);
    if(control.vraminc == 0){
        PPUADDR += 1;
    } else{
        PPUADDR += 0x20;
    }
    return toreturn;
}
byte PPU2C02::getOAMDATA(){
    return OAMDATA;
}
void PPU2C02::setPPUCTRL(byte data){
    //if(status.vblank){
        control.reg = data;
        if(scanline >= 256){
            if(control.nmigen != 0 && status.vblank == 1){
                doNMI = true;
            }
        }
    //}
    //std::cout << "PPUCTRL is: " << int(control.reg) << '\n';
}
void PPU2C02::setPPUMASK(byte data){
    mask.reg = data;
}
void PPU2C02::setOAMADDR(byte data){
    OAMADDR = data;
}
void PPU2C02::setOAMDATA(byte data){
    OAMDATA = data;
    oam[OAMADDR] = OAMDATA;
    OAMADDR += 1;
}
void PPU2C02::setPPUSCROLL(byte data){
    //std::cout << int(data) << '\n';
    if(scrollbyte == 0){
        PPUSCROLLX = data;
        scrollbyte = 1;
    } else {
        scrollbyte = 0;
    }
}
void PPU2C02::setPPUADDR(byte data){
    //std::cout << "Setting addr byte " << int(ppuaddrbyte) << " to " << int(data) << '\n';
    if(ppuaddrbyte == 0){
        //PPUADDR2 = (PPUADDR & 0x00FF) | data;
        PPUADDR2 = data;
        PPUADDR2 <<= 8;
        ppuaddrbyte = 1;
    } else {
        PPUADDR2 = PPUADDR2 | data; 
        PPUADDR = PPUADDR2;
        ppuaddrbyte = 0;
        // /std::cout << "PPUADDR: " << int(PPUADDR) << '\n';
    }
}
void PPU2C02::setPPUDATA(byte data){
    //exit(0);
    //std::cout << std::hex << "Writing " << int(data) << " to vram at " << PPUADDR << std::dec << '\n';
    //if(PPUADDR >= 0x3F00){
        //if(data == 0xC0){
        //    std::cout << "PIPE" << '\n';
        //}
        //std::cout << "Write PPU data: " << std::hex << int(data) << " at " << int(PPUADDR) << '\n';
        //std::cout << "Increment is " << int(control.vraminc) << '\n';
        //drawTile(3 * data / 0x20, 3 * (data % 0x32), data);
    //}
    bus->ppuWrite(PPUADDR, data);
    if(control.vraminc == 0){
        PPUADDR += 1;
    } else{
        PPUADDR += 0x20;
    }
}
void PPU2C02::doOAMDMA(byte data){
    word addr = data << 8;
    for(word i = 0; i < 0x100; i ++){
        oam[i] = bus->cpuRead(addr + i);
    }
    //exit(0);
}