#include "NetworkWrapper.h"

sn::Cartridge NetworkWrapper::cartridge = sn::Cartridge();
int NetworkWrapper::inSize = StateGetter::SIZE;
int NetworkWrapper::outSize = 4;
bool NetworkWrapper::initialized = false;

void NetworkWrapper::init(){
    cartridge.loadFromFile(std::string("../../roms/mariobros.nes"));
}

NetworkWrapper::NetworkWrapper(const NetworkWrapper* other) {
    if(!initialized){
        init();
        initialized = true;
    }
    pos = other->pos;
    initialPos = other->initialPos;
    movement = other->movement;
    lastPos[0] = other->lastPos[0];
    lastPos[1] = other->lastPos[1];
    fitness = other->fitness;
    ticks = other->ticks;
    jumpTime = other->jumpTime;
    species = other->species;
    complexity = other->complexity;
    network = other->network;
    finished = other->finished;
}

NetworkWrapper& NetworkWrapper::operator=(const NetworkWrapper* other) {
    pos = other->pos;
    initialPos = other->initialPos;
    movement = other->movement;
    lastPos[0] = other->lastPos[0];
    lastPos[1] = other->lastPos[1];
    fitness = other->fitness;
    ticks = other->ticks;
    jumpTime = other->jumpTime;
    species = other->species;
    complexity = other->complexity;
    network = other->network;
    finished = other->finished;
    return *this;
}

NetworkWrapper& NetworkWrapper::operator=(const NetworkWrapper& other) {
    pos = other.pos;
    initialPos = other.initialPos;
    movement = other.movement;
    lastPos[0] = other.lastPos[0];
    lastPos[1] = other.lastPos[1];
    fitness = other.fitness;
    ticks = other.ticks;
    jumpTime = other.jumpTime;
    species = other.species;
    complexity = other.complexity;
    network = other.network;
    finished = other.finished;
    return *this;
}

NetworkWrapper::NetworkWrapper(const NetworkWrapper& other) {
    if(!initialized){
        init();
        initialized = true;
    }
    pos = other.pos;
    initialPos = other.initialPos;
    movement = other.movement;
    lastPos[0] = other.lastPos[0];
    lastPos[1] = other.lastPos[1];
    fitness = other.fitness;
    ticks = other.ticks;
    jumpTime = other.jumpTime;
    species = other.species;
    complexity = other.complexity;
    network = other.network;
    finished = other.finished;
}

NetworkWrapper::NetworkWrapper() : network() {
    if(!initialized){
        init();
        initialized = true;
    }
    complexity = network.connectionIDs.size();
    species = 0;
    frameNum = 0;
    setCallbacks();
}

NetworkWrapper::NetworkWrapper(NeuralNet other) : network(other) {
    if(!initialized){
        init();
        initialized = true;
    }
    complexity = network.connectionIDs.size();
    species = 0;
    frameNum = 0;
    setCallbacks();
}

//Both arguments default to 0
void NetworkWrapper::show(int baseNum, int genNum){
    basePath = "";
    frameNum = 0;
    if(!NeuralNet::playTest){
        basePath += "framesOut/" + std::to_string(baseNum) + '/' + std::to_string(genNum);
        mkdir(basePath.c_str(), 0);
    }
    emulator.stop = false;
    movement = 0;
    emulator.show = true;
    float fitnessHold = fitness;
    emulator.reset();
    for(int i = 0; i < 4; i ++){
        lastInputs[i] = 0;
    }
    emulator.run(cartridge, 0);
    fitness = fitnessHold;
    emulator.show = false;
}

void NetworkWrapper::reset(){
    ticks = 0;
    times = 0;
    movement = 0;
    lastPos[0] = 0;
    lastPos[1] = 0;
    jumpTime = -1;
    jumpBuffer = 0;
    fitness = 0;
    pos = 0;
    initialPos = 0;
    for(int i = 0; i < 4; i ++){
        lastInputs[i] = 0;
    }
    emulator.reset();
}

unsigned int NetworkWrapper::feedForward(std::vector<float> input){
    finished = false;
    if(times == 0){
        initialPos = emulator.stateGetter.getXY()[0] + emulator.m_bus.read(0x0760) * 300;
    }
    const int size = StateGetter::SIZE;
    int playerState = emulator.m_bus.read(0x0E);
    bool noControl = (playerState >= 0x02 && playerState <= 0x05) || playerState == 0x07 || playerState == 0;
    if(!noControl){
        times += 1;
    } else {
        finished = true;
    }
    times += 1;
    std::vector<int> thisPos = emulator.stateGetter.getXY();
    int move = thisPos[0] - lastPos[0];
    lastPos[0] = thisPos[0];
    if(thisPos[0] > pos){
        pos = thisPos[0];
    }
    lastPos[1] = thisPos[1];
    if(noControl && !emulator.show){
        emulator.stopRun();
    }
    if(move == 0){
        movement += 1;
        if(noControl){
            movement = 0;
        } else if(movement > 75){
            emulator.stopRun();
        }
    } else {
        movement = 0;
    }
    //Was a button pressed last frame?
    input[StateGetter::SIZE - 3] = (emulator.m_bus.read(0x0D) & 0b10000000) != 0 ? 1 : -1;
    // input[StateGetter::SIZE - 5] = lastInputs[1];
    // input[StateGetter::SIZE - 4] = lastInputs[2];
    // input[StateGetter::SIZE - 3] = lastInputs[3];
    std::vector<float> output = network.feedForward(input);
    unsigned int inputs = 0;
    //jump
    if(output[0] > 0.5){
        inputs += 1;
        lastInputs[0] = 1;
    } else {
        lastInputs[0] = -1;
    }
    if(output[1] > 0.5){
        inputs += 2;
        lastInputs[1] = 1;
    } else {
        lastInputs[1] = -1;
    }
    // if(output[2] > 0.5){
    //     inputs += 16;
    // }
    // if(output[3] > 0.5){
    //     inputs += 32;
    // }
    //left
    if(output[2] > 0.5){
        inputs += 64;
        lastInputs[2] = 1;
    } else {
        lastInputs[2] = -1;
    }
    //right
    if(output[3] > 0.5){
        inputs += 128;
        lastInputs[3] = 1;
        //fitness[i] += 0.01;
    } else {
        lastInputs[3] = -1;
    }
    return inputs;
}

void NetworkWrapper::run(int saveFile){
    fitness = 0;
    reset();
    emulator.run(cartridge, saveFile);
    if(times == 0){
        fitness = 0.001; 
    } else {
        pos -= initialPos;
        //level * 300
        pos += emulator.m_bus.read(0x0760) * 300;
        fitness = static_cast<float>(pos * pos) / times;
    }
    if(fitness <= 0){
        fitness = 0.001;
    }
}

void NetworkWrapper::saveToFile(std::string fname){
    network.saveToFile(fname);
}

NetworkWrapper NetworkWrapper::merge(NetworkWrapper& other){
    return NetworkWrapper(network.merge(other.network));
}

bool NetworkWrapper::similar(NetworkWrapper& other){
    return network.similar(other.network);
}

void NetworkWrapper::setCallbacks(){
    emulator.setInputCallback(
        [&](){
            std::vector<float> state = emulator.stateGetter.getState();
            input = state;
            unsigned int inputs = feedForward(state);
            if(!NeuralNet::playTest){
                int size = state.size();
                emulator.m_controller1.setKeystates(inputs);
            }
        }
    );
    emulator.setDrawCallback([&](){
        const int xPos = 20;
        const int yPos = 20;
        const int blockSize = 20;
        sf::RectangleShape rectangle(sf::Vector2f(blockSize * StateGetter::xRange, blockSize * StateGetter::yRange));
        rectangle.setPosition(sf::Vector2f(xPos, yPos));
        rectangle.setOutlineColor(sf::Color(0, 0, 0, 200));
        rectangle.setOutlineThickness(2);
        rectangle.setFillColor(sf::Color(0, 0, 0, 0));
        emulator.m_window.draw(rectangle);
        rectangle.setOutlineThickness(1);
        int px = 1;
        int py = StateGetter::yRadius + 1;
        int x, y;
        //Loop through game "image"
        std::map<int, std::vector<int>> nodePos;
        for(int i = 0; i < StateGetter::xRange * StateGetter::yRange; i ++){
            //int page = i / (StateGetter::xRange * StateGetter::yRange);
            //int val = input[i] == 1 ? page : 0;
            int val = input[i];
            int iMod = i % (StateGetter::xRange * StateGetter::yRange);
            x = iMod / StateGetter::yRange;
            y = iMod % StateGetter::yRange;
            nodePos.insert(std::pair<int, std::vector<int>>(i, std::vector<int> {xPos + x * blockSize + blockSize / 2, yPos + y * blockSize + blockSize / 2}));
            sf::RectangleShape rectangle(sf::Vector2f(blockSize, blockSize));
            rectangle.setOutlineThickness(1);
            rectangle.setOutlineColor(sf::Color(0, 0, 0, 200));
            switch(val){
                case 0:
                break;
                case -1:
                    rectangle.setSize(sf::Vector2f(blockSize, blockSize));
                    rectangle.setPosition(sf::Vector2f(xPos + x * blockSize, yPos + y * blockSize));
                    rectangle.setFillColor(sf::Color(255, 0, 0, 150));
                    emulator.m_window.draw(rectangle);
                break;
                case 1:
                    rectangle.setSize(sf::Vector2f(blockSize, blockSize));
                    rectangle.setPosition(sf::Vector2f(xPos + x * blockSize, yPos + y * blockSize));
                    rectangle.setFillColor(sf::Color(100, 100, 100, 175));
                    emulator.m_window.draw(rectangle);
                break;
                default:
                    rectangle.setSize(sf::Vector2f(blockSize, blockSize));
                    rectangle.setPosition(sf::Vector2f(xPos + x * blockSize, yPos + y * blockSize));
                    rectangle.setFillColor(sf::Color(0, 0, 0, 150));
                    emulator.m_window.draw(rectangle);
                break;
            }
        }
        //Draw player pos
        rectangle.setSize(sf::Vector2f(blockSize, blockSize));
        rectangle.setPosition(sf::Vector2f(xPos + px * blockSize, yPos + py * blockSize));
        rectangle.setFillColor(sf::Color(0, 0, 255, 150));
        emulator.m_window.draw(rectangle);
        
        //Draw the rest of the input nodes
        for(int i = StateGetter::xRange * StateGetter::yRange; i < inSize; i ++){
            int iDiff = i - StateGetter::xRange * StateGetter::yRange;
            float y = StateGetter::yRange + iDiff * 1.5 + 0.5;
            int x = StateGetter::xRange - 1;
            int xVal = xPos + x * blockSize;
            int yVal = yPos + y * blockSize;
            nodePos.insert(std::pair<int, std::vector<int>>(i, std::vector<int> {xVal + blockSize / 2, yVal + blockSize / 2}));
            rectangle.setPosition(sf::Vector2f(xVal, yVal));
            int c = network.getValue(i) * 255;
            if(c > 0){
                if(c > 255){
                    c = 255;
                }
                rectangle.setFillColor(sf::Color(255 - c, 255, 255 - c, 150));
            } else {
                if(c < -255){
                    c = -255;
                }
                rectangle.setFillColor(sf::Color(255, 255 + c, 255 + c, 150));
            }
            emulator.m_window.draw(rectangle);
        }

        //Draw all the hidden nodes
        rectangle.setSize(sf::Vector2f(blockSize / 3, blockSize / 3));
        int sze = network.nodeIDs.size();
        for(int i = 0; i < sze; i ++){
            int id = network.nodeIDs[i];
            float y = i * 1.1;
            int x = StateGetter::xRange + 4;
            int xVal = xPos + x * blockSize;
            int yVal = yPos + y * blockSize / 3;
            nodePos.insert(std::pair<int, std::vector<int>>(id, std::vector<int> {xVal + blockSize / 6, yVal + blockSize / 6}));
            rectangle.setPosition(sf::Vector2f(xVal, yVal));
            int c = (network.getValue(id) - 0.5) * 255 * 2;
            if(c > 0){
                if(c > 255){
                    c = 255;
                }
                if(c < 50){
                    c = 50;
                }
                rectangle.setFillColor(sf::Color(255 - c, 255, 255 - c, c));
            } else {
                if(c < -255){
                    c = -255;
                }
                if(c > -50 && c != 0){
                    c = -50;
                }
                rectangle.setFillColor(sf::Color(255, 255 + c, 255 + c, -c));
            }
            emulator.m_window.draw(rectangle);
        }
        rectangle.setSize(sf::Vector2f(blockSize, blockSize));

        //Draw all the output nodes
        for(int i = inSize; i < inSize + outSize; i ++){
            int y = (i - inSize) * 2;
            int x = StateGetter::xRange + 9;
            nodePos.insert(std::pair<int, std::vector<int>>(i, std::vector<int> {xPos + x * blockSize + blockSize / 2, yPos + y * blockSize + blockSize / 2}));
            rectangle.setPosition(sf::Vector2f(xPos + x * blockSize, yPos + y * blockSize));
            int c = (network.getValue(i) - 0.5) * 255 * 2;
            if(c > 0){
                if(c > 255){
                    c = 255;
                }
                if(c < 50){
                    c = 50;
                }
                rectangle.setFillColor(sf::Color(255 - c, 255, 255 - c, 150));
            } else {
                if(c < -255){
                    c = -255;
                }
                if(c > -50 && c != 0){
                    c = -50;
                }
                rectangle.setFillColor(sf::Color(255, 255 + c, 255 + c, 150));
            }
            emulator.m_window.draw(rectangle);
        }

        //Draw all the connections
        for(int i = 0; i < network.connectionIDs.size(); i ++){
            Connection conn = network.connections.at(network.connectionIDs[i]);
            std::vector<int> from = nodePos.at(conn.from);
            std::vector<int> to = nodePos.at(conn.to);
            int dx = to[0] - from[0];
            int dy = to[1] - from[1];
            rectangle.setSize(sf::Vector2f(sqrt(dx * dx + dy * dy), 2));
            rectangle.setPosition(sf::Vector2f(from[0], from[1]));
            rectangle.setOutlineThickness(0);
            int c = conn.strength * 122;
            int a;
            if(!conn.enabled){
                a = 20;
            } else {
                if(conn.from >= inSize){
                    a = abs((network.getValue(conn.from) - 0.5) * 255 * 2);
                } else {
                    a = abs(network.getValue(conn.from) * 255);
                }
                if(a < 20){
                    a = 20;
                }
            }
            if(c > 0){
                if(c > 255){
                    c = 255;
                }
                rectangle.setFillColor(sf::Color(255 - c, 255, 255 - c, a));
            } else {
                if(c < -255){
                    c = -255;
                }
                rectangle.setFillColor(sf::Color(255, 255 + c, 255 + c, a));
            }
            rectangle.setRotation(atan2(dy, dx) * 180 / 3.1415);
            emulator.m_window.draw(rectangle);
        }
        rectangle.setRotation(0);
        
        if(!NeuralNet::playTest && frameNum % 3 == 0){
            sf::Texture texture;
            texture.create(emulator.m_window.getSize().x, emulator.m_window.getSize().y);
            texture.update(emulator.m_window);
            std::string numName = std::to_string(frameNum / 3);
            numName = std::string(5 - std::min(5, static_cast<int>(numName.length())), '0') + numName;
            texture.copyToImage().saveToFile(basePath + "/" + numName + ".jpg");
        }
        frameNum ++;
    });
}