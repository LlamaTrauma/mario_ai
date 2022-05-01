#include <StateGetter.h>

StateGetter::StateGetter(sn::MainBus* ram): state2(yRange, std::vector<int>(xRange, 0)) {
    this->ram = ram;
    for(int i = 0; i < NUMCOLS * 3 / 2; i ++){
        tiles.push_back(std::vector<int>(COLSIZE, 0));
    }
    for(int i = 0; i < NUMCOLS; i ++){
        state.push_back(std::vector<int>(COLSIZE, 0));
    }
    lastBlockScroll = 0;
    blockScrollOffset = 0;
    xVal = 0;
    lastScroll = 0;
    scrollPage = 0;
}

bool isEnemy(sn::Byte code){
    return code == 0 || (code >= 0x2 && code <= 0x3) || (code >= 0x5 && code <= 0x7) || (code >= 0xA && code <= 0x12) || code == 0x15 || code == 0x2D;
}

int getTileType(int tile){
    //https://datacrystal.romhacking.net/wiki/Super_Mario_Bros.:Notes
    if((tile >= 0x0D && tile <= 0x21) || (tile >= 0x51 && tile <= 0x56) || (tile >= 0x61 && tile <= 0x6C)){
        //solid
        return 2;
    } else if((tile >= 0x86 && tile <= 0x87)){
        //dangerous
        return 1;
    } else if ((tile >= 0xC0 && tile <= 0xC1) || (tile >= 0x57 && tile <= 0x60)){
        //good (removed coins from this for now)
        return 2;
    }
    //empty
    return 0;
}

bool floatyGE(float a, float b){
    return a >= b - 0.01;
}

bool floatyLE(float a, float b){
    return a <= b + 0.01;
}

bool linesIntersect(float A0, float B0, float A1, float B1, float A2, float B2, float A3, float B3){
    return ((A2-A0)*(B1-B0) - (B2-B0)*(A1-A0)) * ((A3-A0)*(B1-B0) - (B3-B0)*(A1-A0)) < 0
&&
((A0-A2)*(B3-B2) - (B0-B2)*(A3-A2)) * ((A1-A2)*(B3-B2) - (B1-B2)*(A3-A2)) < 0;

    // return ((x3-x1)*(y2-y1) - (y3-y1)*(x2-x1)) * ((x4-x1)*(y2-y1) - (y4-y1)*(x2-x1)) < 0
    // &&
    // ((x1-x3)*(y4-y3) - (y1-y3)*(x4-x3)) * ((x2-x3)*(y4-y3) - (y2-y3)*(x4-x3)) < 0;
    //I did something wrong here but spent too long looking for it so I got a much better method from stackoverflow
    //https://stackoverflow.com/questions/14176776/find-out-if-2-lines-intersect
    // const float FLOATADJ = 0;
    // if(x4 == x3 && x2 == x1){
    //     //std::cout << 1 << '\n';
    //     float ymin = std::min(y3, y4);
    //     float ymax = std::max(y3, y4);
    //     return (y1 + FLOATADJ >= ymin && y1 - FLOATADJ <= ymax) || (y2 + FLOATADJ >= ymin && y2 - FLOATADJ <= ymax);
    // }
    // if(x2 == x1){
    //     //std::cout << 2 << '\n';
    //     float a2 = (y4 - y3) / (x4 - x3);
    //     float b2 = y3 - x3 * a2;
    //     float y = a2 * x1 + b2;
    //     float ymin1 = std::min(y1, y2);
    //     float ymax1 = std::max(y1, y2);
    //     float ymin2 = std::min(y3, y4);
    //     float ymax2 = std::max(y3, y4);
    //     //std::cout << x2 << ", " << y2 << '\n';
    //     return y + FLOATADJ >= ymin1 && y - FLOATADJ <= ymax1 && y + FLOATADJ >= ymin2 && y - FLOATADJ <= ymax2;
    // }
    // if(x4 == x3){
    //     //std::cout << 3 << '\n';
    //     float a1 = (y2 - y1) / (x2 - x1);
    //     float b1 = y1 - x1 * a1;
    //     float y = a1 * x3 + b1;
    //     float ymin1 = std::min(y1, y2);
    //     float ymax1 = std::max(y1, y2);
    //     float ymin2 = std::min(y3, y4);
    //     float ymax2 = std::max(y3, y4);
    //     return y + FLOATADJ >= ymin1 && y - FLOATADJ <= ymax1 && y + FLOATADJ >= ymin2 && y - FLOATADJ <= ymax2;
    // }
    // float a1 = (y2 - y1) / (x2 - x1);
    // float a2 = (y4 - y3) / (x4 - x3);
    // float b1 = y1 - x1 * a1;
    // float b2 = y3 - x3 * a2;
    // if(a1 == 0){
    //     // float x = (y1 - b2) / a2;
    //     // float xmin = std::min(x3, x4);
    //     // float xmax = std::max(x3, x4);
    //     // return x - FLOATADJ <= xmax && x + FLOATADJ >= xmin;
    // }
    // if(abs(a1 - a2) < 0.0001){
    //     //std::cout << 4 << '\n';
    //     float xmin1 = std::min(x1, x2);
    //     float xmax1 = std::max(x1, x2);
    //     float xmin2 = std::min(x3, x4);
    //     float xmax2 = std::max(x3, x4);
    //     //std::cout << (b2 == b1) << ", " << abs(b2 - b1) << '\n';
    //     return b2 == b1 && 
    //     ((x3 + FLOATADJ >= xmin1 && x3 - FLOATADJ <= xmax1) || 
    //     (x4 + FLOATADJ >= xmin1 && x4 - FLOATADJ <= xmax1) ||
    //     (x1 + FLOATADJ >= xmin2 && x1 - FLOATADJ <= xmax2) ||
    //     (x2 + FLOATADJ >= xmin2 && x2 - FLOATADJ <= xmax2));
    // }
    // //std::cout << 5 << '\n';
    // float x = (b2 - b1) / (a1 - a2);
    // float xmin1 = std::min(x1, x2);
    // float xmax1 = std::max(x1, x2);
    // float xmin2 = std::min(x3, x4);
    // float xmax2 = std::max(x3, x4);
    // return x + FLOATADJ >= xmin1 && x - FLOATADJ <= xmax1 && x + FLOATADJ >= xmin2 && x - FLOATADJ <= xmax2;
}

//out should be an array of ints probably with my current setup
bool StateGetter::inVision(int x, int y, std::vector<float>& out){
    int minX = x < 0 ? x : 0;
    int maxX = x < 0 ? 0 : x;
    int minY = y < 0 ? y : 0;
    int maxY = y < 0 ? 0 : y;
    //std::cout << linesIntersect(0.5, 0.5, 0.5, 1.5, 0, 1, 1, 2) << '\n';
    for(int i = 0; i < xRange; i ++){
        for(int j = 0; j < yRange; j ++){
            //if(out[i * yRange + j] == 1.0 && actualX != x && actualY != y && actualX >= minX - 1 && actualX <= maxX + 1 && actualY >= minY - 1 && actualY <= maxY + 1 && 
            if(out[i * yRange + j] == 1 && (x != i || y != j)){
                bool allBlocked = true;
                int blockLines[4][4] = {{0, 0, 1, 0}, {1, 0, 1, 1}, {1, 1, 0, 1}, {0, 1, 0, 0}};
                float faces[4][2] = {{0.5, 1}, {1, 0.5}, {0.5, 0}, {0, 0.5}};
                for(int k = 0; k < 4; k ++){
                    bool thisBlocked = false;
                    for(int l = 0; l < 4; l ++){
                        //if(lineCollision(3.5, 4.5, x + faces[k][0], y + faces[k][1], i + blockLines[l]))
                    }
                    if(thisBlocked == false){
                        allBlocked = false;
                        break;
                    }
                }
                if(allBlocked){
                    return false;
                }
            }
        }
    }
    return true;
}

std::vector<float> StateGetter::getState(){
    //https://gist.github.com/1wErt3r/4048722 very helpful
    const int blockSize = 16;
    //From 0x500 to 0x06A0 is the current screen
    //Length of the actual data is 0x138, wraps around
    //Structured so it 0x0500 to 0x05D0 and 0x05D0 to 0x06A0 both look like
    //a screen when viewed in the memory viewer with 16 columns
    //The start address is 0x0500 + (scroll / 16 % 32) + 0xD0 sometimes
    //Column number + page number * 16 - scroll x / block size % 2 (because columns are added two at a time)
    //Get page position of player (0 or 1)
    unsigned int sprObjPageLoc = ram->read(0x6d);
    //Get x position of player
    unsigned int sprObjXPos = ram->read(0x86) + 8;
    sprObjPageLoc &= 1;
    sprObjXPos >>= 4;
    //sprXPos += (sprObj_PageLoc << 7); 
    //sprObjXPos += 0x500 + sprObjPageLoc * 0xD0;
    sprObjXPos += sprObjPageLoc * 16;
    unsigned int sprObjYPos = ram->read(0xCE) + 16;
    sprObjYPos &= 0b11110000;
    sprObjYPos -= 32;
    sprObjYPos >>= 4;
    
    int blockScroll = sprObjXPos;
    
    int size = SIZE;
    std::vector<float> out(size, 0.0);
    int nearPos = 0;
    int iPos = 0;
    for(int i = -2; i < xRange - 2; i ++){
        int jPos = 0;
        int bufferX = (blockScroll + i + 32) % 32;
        for(int j = int(sprObjYPos) - yRadius - 1; j <= int(sprObjYPos) + yRadius - 1; j ++){
            int addr = 0x0500 + (bufferX % 16) + (bufferX > 15 ? 0xD0 : 0) + j * NUMCOLS;
            int val = 0;
            if(j >= 0 && j < COLSIZE){
                val = getTileType(ram->read(addr));
                //it->at(j) = 3;
            }
            //Make pits bad
            if(val == 0 && j == 12){
                val = 1;
            }
            if(val >= 0 && val < 3){
                if(val == 0){
                    //empty
                    out[xRange * yRange * 0 + nearPos] = 0;
                } else if(val == 2){
                    //good
                    out[xRange * yRange * 0 + nearPos] = 1;
                } else if(val == 1){
                    //dangerous
                    out[xRange * yRange * 0 + nearPos] = -1;
                }
            }
            nearPos ++;
        }
        iPos ++;
    }
    for(int i = 0; i < 5; i ++){
        //If an enemy is on screen and active and exists in that spot
        if(!ram->read(0x03D8 + i) && ram->read(0x000F + i) && isEnemy(ram->read(0x0016 + i))){
            unsigned int enemyObjPageLoc = ram->read(0x6e + i);
            //Get x position of player
            unsigned int enemyObjXPos = ram->read(0x87 + i) + 8;
            enemyObjPageLoc &= 1;
            enemyObjXPos >>= 4;
            //sprXPos += (sprObj_PageLoc << 7); 
            //sprObjXPos += 0x500 + sprObjPageLoc * 0xD0;
            enemyObjXPos += enemyObjPageLoc * 16;
            unsigned int enemyObjYPos = ram->read(0xCF + i) + 16;
            enemyObjYPos &= 0b11110000;
            enemyObjYPos -= 32;
            enemyObjYPos >>= 4;
            //Deal with wrapping around 32
            int l = int(blockScroll) - 3;
            if(abs(int(enemyObjXPos) - l) > abs(int(enemyObjXPos) - (l + 32))){
                l += 32;
            } else if(abs(int(enemyObjXPos) - l) > abs(int(enemyObjXPos) - (l - 32))){
                l -= 32;
            }
            int x = int(enemyObjXPos) - l;
            //std::cout << x << '\n';
            int y = int(enemyObjYPos) - (int(sprObjYPos) - yRadius - 1);
            if(x >= 0 && x < xRange && y >= 0 && y < yRange){
                out[x * yRange + y] = -1;
            }
        }
    }
    std::vector<int> makeZero(0);
    for(int i = 0; i < xRange * yRange; i ++){
        int x = i / yRange;
        int y = i % yRange;
        if(out[i] == 1 && (x > 0 && out[(x - 1) * yRange + y] == 1) && (y > 0 && out[x * yRange + y - 1] == 1)){
            makeZero.push_back(i);
        }
    }
    for(int i = 0; i < makeZero.size(); i ++){
        out[makeZero[i]] = 0;
    }
    out[size - 2] = ram->read(0x1D) == 0 ? 1 : -1;
    out[size - 1] = 1;
    // int scrollAmount = ram->read(0x073F);
    // if(scrollAmount < 128 && lastScroll > 128){
    //     scrollPage += 256;
    // }
    // xVal = scrollAmount + ram->read(0x03AD) + scrollPage;
    // lastScroll = scrollAmount;
    // out[size - 2] = blockScroll + ram->read(0x03AD) / 16 - 2;
    // out[size - 1] = ram->read(0x03B8) / blockSize;
    return out;
}

std::vector<int> StateGetter::getXY(){
    int scrollThirtyTwo = ram->read(0x073D);
    scrollThirtyTwo /= 16;
    int scroll = ram->read(0x073F);
    int columnPos = ram->read(0x0726);
    //Seems like it doesn't update the columnPos the same frame scrollThirtyTwo updates so check for that
    if(((scroll / 16) / 2 * 2) % 4 != columnPos % 4){
        scrollThirtyTwo += 1;
    }
    int pageLoc = ram->read(0x0725);
    int blockScroll = columnPos + pageLoc * 16 - 24 + scrollThirtyTwo;
    return std::vector<int> {blockScroll + ram->read(0x03AD) / 16, ram->read(0x03B8) / 16};
}

void StateGetter::drawToScreen(sf::RenderWindow& window){
    const int xPos = 20;
    const int yPos = 20;
    const int blockSize = 10;
    // int x = 0;
    // for(auto i = state.begin(); i != state.end(); i++){
    //     for(int y = 0; y < COLSIZE; y ++){
    //         int val = i->at(y);
    //         switch(val){
    //             case 0:
    //             break;
    //             case 1:
    //                 {sf::RectangleShape rectangle(sf::Vector2f(blockSize, blockSize));
    //                 rectangle.setPosition(sf::Vector2f(xPos + x * blockSize, yPos + y * blockSize));
    //                 rectangle.setFillColor(sf::Color(255, 0, 0, 150));
    //                 window.draw(rectangle);}
    //             break;
    //             case 2:
    //                 {sf::RectangleShape rectangle(sf::Vector2f(blockSize, blockSize));
    //                 rectangle.setPosition(sf::Vector2f(xPos + x * blockSize, yPos + y * blockSize));
    //                 rectangle.setFillColor(sf::Color(100, 100, 100, 175));
    //                 window.draw(rectangle);}
    //             break;
    //             case 3:
    //                 {sf::RectangleShape rectangle(sf::Vector2f(blockSize, blockSize));
    //                 rectangle.setPosition(sf::Vector2f(xPos + x * blockSize, yPos + y * blockSize));
    //                 rectangle.setFillColor(sf::Color(0, 255, 0, 150));
    //                 window.draw(rectangle);}
    //             break;
    //             default:
    //             break;
    //         }
    //     }
    //     x += 1;
    // }
    for(int y = 0; y < yRange; y ++){
        for(int x = 0; x < xRange; x ++){
            int val = state2[y][x];
            switch(val){
                case 0:
                break;
                case 1:
                    {sf::RectangleShape rectangle(sf::Vector2f(blockSize, blockSize));
                    rectangle.setPosition(sf::Vector2f(xPos + x * blockSize, yPos + y * blockSize));
                    rectangle.setFillColor(sf::Color(255, 0, 0, 150));
                    window.draw(rectangle);}
                break;
                case 2:
                    {sf::RectangleShape rectangle(sf::Vector2f(blockSize, blockSize));
                    rectangle.setPosition(sf::Vector2f(xPos + x * blockSize, yPos + y * blockSize));
                    rectangle.setFillColor(sf::Color(100, 100, 100, 175));
                    window.draw(rectangle);}
                break;
                case 3:
                    {sf::RectangleShape rectangle(sf::Vector2f(blockSize, blockSize));
                    rectangle.setPosition(sf::Vector2f(xPos + x * blockSize, yPos + y * blockSize));
                    rectangle.setFillColor(sf::Color(0, 255, 0, 150));
                    window.draw(rectangle);}
                break;
                    {sf::RectangleShape rectangle(sf::Vector2f(blockSize, blockSize));
                    rectangle.setPosition(sf::Vector2f(xPos + x * blockSize, yPos + y * blockSize));
                    rectangle.setFillColor(sf::Color(0, 0, 255, 150));
                    window.draw(rectangle);}
                default:
                break;
            }
        }
    }
    sf::RectangleShape rectangle(sf::Vector2f(blockSize * xRange, blockSize * yRange));
    rectangle.setPosition(sf::Vector2f(xPos, yPos));
    rectangle.setOutlineColor(sf::Color(0, 0, 0, 200));
    rectangle.setOutlineThickness(2);
    rectangle.setFillColor(sf::Color(0, 0, 0, 0));
    window.draw(rectangle);
}

void StateGetter::reset(){
    lastScroll = 0;
    xVal = 0;
    scrollPage = 0;
}