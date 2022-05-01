#include "NeuralNet.h"

//Declare static members
//Idk if it's good practice to do this here or have so many of them
bool NeuralNet::initialized = false;
int NeuralNet::inSize = 0;
int NeuralNet::speciesID = 0;
int NeuralNet::outSize = 0;
int NeuralNet::historicalID = 0;
//was 0.6
//0.5
//0.35
//0.5
float NeuralNet::similarDistance = 0.5;
bool NeuralNet::playTest = false;
std::vector<std::vector<int>> NeuralNet::recentConnections(0);
std::map<int, Node> NeuralNet::nodeMap = std::map<int, Node>();
std::map<int, Connection> NeuralNet::connectionMap = std::map<int, Connection>();
std::map<int, speciesHolder> NeuralNet::speciesMap = std::map<int, speciesHolder>();
sn::Cartridge NeuralNet::cartridge = sn::Cartridge(); 

void NeuralNet::initialize(int inputSize, int outputSize){
    NeuralNet::inSize = inputSize;
    NeuralNet::outSize = outputSize;
    cartridge.loadFromFile(std::string("../../roms/mariobros.nes"));
    for(int i = 0; i < inputSize; i ++){
        int id = i;
        historicalID++;
        Node inputNode(nodeType::INPUT, id);
        NeuralNet::nodeMap.insert(std::pair<int, Node>(id, inputNode));
    }
    for(int i = 0; i < outputSize; i ++){
        int id = i + inputSize;
        historicalID++;
        Node outputNode(nodeType::OUTPUT, id);
        NeuralNet::nodeMap.insert(std::pair<int, Node>(id, outputNode));
    }
    initialized = true;
}

NeuralNet::NeuralNet(): nodeIDs(0), connectionIDs(0) {
    if(!initialized){
        initialize(StateGetter::SIZE, 4);
    }
    for(int i = 0; i < inSize + outSize; i ++){
        nodes.insert(std::pair<int, Node>(i, nodeMap.at(i)));
    }
    inputAvg.resize(inSize);
    std::fill(inputAvg.begin(), inputAvg.end(), 0);
    globalAvg.resize(inSize);
    std::fill(globalAvg.begin(), globalAvg.end(), 0);
    //Connect an always active node and the jump button
    //addConnection(inSize - 2, inSize, rand() / static_cast<float>(RAND_MAX) - 0.5, true, -2);
    //Connect grounded and a button nodes
    //addConnection(inSize - 1, inSize, rand() / static_cast<float>(RAND_MAX) - 0.5, true, -3);
    int numToAdd = rand() % 3;
    while(numToAdd --> 0){
        int from = chooseFromNode();
        int to = rand() % outSize + inSize;
        if(rand() % 10 == 0 && nodeIDs.size() > 0 && from < inSize){
            to = nodeIDs[rand() % nodeIDs.size()];
        }
        float strength = rand() / static_cast<float>(RAND_MAX) * 2 - 1.0;
        addConnection(from, to, strength);
    }
    emulator.setInputCallback(
        [&](){
            std::vector<float> state = emulator.stateGetter.getState();
            if(!playTest){
                int size = state.size();
                unsigned int inputs = feedForward(state);
                emulator.m_controller1.setKeystates(inputs);
            } else {
                input = state;
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
        int px = 3;
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
            int c = getValue(i) * 255;
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
        int sze = nodeIDs.size();
        for(int i = 0; i < sze; i ++){
            int id = nodeIDs[i];
            float y = i * 1.1;
            int x = StateGetter::xRange + 4;
            int xVal = xPos + x * blockSize;
            int yVal = yPos + y * blockSize / 3;
            nodePos.insert(std::pair<int, std::vector<int>>(id, std::vector<int> {xVal + blockSize / 6, yVal + blockSize / 6}));
            rectangle.setPosition(sf::Vector2f(xVal, yVal));
            int c = (getValue(id) - 0.5) * 255 * 2;
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
                if(c > -50){
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
            int c = (getValue(i) - 0.5) * 255 * 2;
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
                if(c > -50){
                    c = -50;
                }
                rectangle.setFillColor(sf::Color(255, 255 + c, 255 + c, 150));
            }
            emulator.m_window.draw(rectangle);
        }

        //Draw all the connections
        for(int i = 0; i < connectionIDs.size(); i ++){
            Connection conn = connections.at(connectionIDs[i]);
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
                    a = abs((getValue(conn.from) - 0.5) * 255 * 2);
                } else {
                    a = abs(getValue(conn.from) * 255);
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
        
        if(!playTest){
            sf::Texture texture;
            texture.create(emulator.m_window.getSize().x, emulator.m_window.getSize().y);
            texture.update(emulator.m_window);
            std::string numName = std::to_string(frameNum);
            numName = std::string(5 - std::min(5, static_cast<int>(numName.length())), '0') + numName;
            texture.copyToImage().saveToFile(basePath + "/" + numName + ".jpg");
        }
        frameNum ++;
    });
}

NeuralNet::NeuralNet(const NeuralNet* other) {
    nodes = other->nodes;
    connections = other->connections;
    nodeIDs = other->nodeIDs;
    connectionIDs = other->connectionIDs;
    pos = other->pos;
    movement = other->movement;
    lastPos[0] = other->lastPos[0];
    lastPos[1] = other->lastPos[1];
    fitness = other->fitness;
    ticks = other->ticks;
    jumpTime = other->jumpTime;
    species = other->species;
}

NeuralNet& NeuralNet::operator=(const NeuralNet* other) {
    nodes = other->nodes;
    connections = other->connections;
    nodeIDs = other->nodeIDs;
    connectionIDs = other->connectionIDs;
    pos = other->pos;
    movement = other->movement;
    lastPos[0] = other->lastPos[0];
    lastPos[1] = other->lastPos[1];
    fitness = other->fitness;
    ticks = other->ticks;
    jumpTime = other->jumpTime;
    species = other->species;
    return *this;
}

NeuralNet& NeuralNet::operator=(const NeuralNet& other) {
    nodes = other.nodes;
    connections = other.connections;
    nodeIDs = other.nodeIDs;
    connectionIDs = other.connectionIDs;
    pos = other.pos;
    movement = other.movement;
    lastPos[0] = other.lastPos[0];
    lastPos[1] = other.lastPos[1];
    fitness = other.fitness;
    ticks = other.ticks;
    jumpTime = other.jumpTime;
    species = other.species;
    return *this;
}

NeuralNet::NeuralNet(const NeuralNet& other) {
    nodes = other.nodes;
    connections = other.connections;
    nodeIDs = other.nodeIDs;
    connectionIDs = other.connectionIDs;
    pos = other.pos;
    movement = other.movement;
    lastPos[0] = other.lastPos[0];
    lastPos[1] = other.lastPos[1];
    fitness = other.fitness;
    ticks = other.ticks;
    jumpTime = other.jumpTime;
    species = other.species;
}

NeuralNet::NeuralNet(std::map<int, Node> nodes, std::map<int, Connection> connections, std::vector<int> nodeIDs, std::vector<int> connectionIDs) {
    this->nodes = nodes;
    this->connections = connections;
    this->nodeIDs = nodeIDs;
    this->connectionIDs = connectionIDs;
}

unsigned int NeuralNet::feedForward(std::vector<float> input){
    const int size = StateGetter::SIZE;
    times += 1;
    std::vector<int> thisPos = emulator.stateGetter.getXY();
    int move = thisPos[0] - lastPos[0];
    lastPos[0] = thisPos[0];
    if(thisPos[0] > pos){
        pos = thisPos[0];
    }
    //std::cout << lastPos[i][0] << '\n';
    lastPos[1] = thisPos[1];
    if(move < 1){
        movement += 1;
        int playerState = emulator.m_bus.read(0x0E);
        if((playerState >= 0x02 && playerState <= 0x05) || playerState == 0x07){
            movement = 0;
        } else if(movement > 150){
            emulator.stopRun();
        }
    } else {
        movement = 0;
    }
    this->input = input;
    const int LOCALRANGE = 60;
    for(int i = 0; i < inSize; i ++){
        //Arbitrary, a second seems good
        if(ticks < LOCALRANGE){
            inputAvg[i] = (inputAvg[i] * ticks + input[i]) / (ticks + 1);
        } else {
            inputAvg[i] = (inputAvg[i] * (LOCALRANGE - 1) + input[i]) / LOCALRANGE;
        }
        globalAvg[i] = (globalAvg[i] * ticks + input[i]) / (ticks + 1);
    }
    ticks += 1;
    for(int i = 0; i < inSize + outSize; i ++){
        nodes.at(i).value = 0;
    }
    for(int i = 0; i < nodeIDs.size(); i ++){
        nodes.at(nodeIDs[i]).value = 0;
    }
    
    std::vector<float> output(outSize, 0);
    for(int i = 0; i < outSize; i ++){
        output[i] = getValue(inSize + i);
    }
    unsigned int inputs = 0;
    //jump
    if(output[0] > 0.5){
        inputs += 1;
    }
    if(output[1] > 0.5){
        inputs += 2;
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
    }
    //right
    if(output[3] > 0.5){
        inputs += 128;
        //fitness[i] += 0.01;
    }
    return inputs;
}

float NeuralNet::getValue(int node){
    Node* n = &(nodes.at(node));
    if(abs(n->value) > 0.001){
        //std::cout << (n->value) << '\n';
        return n->value;
    }
    if(n->type == nodeType::INPUT){
        n->value = input[n->id];
        return n->value;
    }
    if(n->type == nodeType::HIDDEN && n->value != 0){
        return n->value;
    }
    int numInputs = (n->in).size();
    n->value = n->bias;
    for(int i = 0; i < numInputs; i ++){
        Connection conn = connections.at(n->in[i]);
        if(conn.enabled){
            float connectionVal = conn.strength;
            float nodeVal = getValue(conn.from);
            n->value += nodeVal * connectionVal;
        }
    }
    //sigmoid
    n->value = 1.0 / (1 + pow(2.71828, -n->value));
    return n->value;
}

int NeuralNet::addConnection(int from, int to, float strength){
    int id = historicalID++;
    Connection c(from, to, strength, id);
    connections.insert(std::pair<int, Connection>(id, c));
    connectionIDs.push_back(id);
    //connectionMap.insert(std::pair<int, Connection>(id, c));
    nodes.at(from).out.push_back(id);
    nodes.at(to).in.push_back(id);
    return id;
}

bool NeuralNet::addConnection(int from, int to, float strength, bool enabled, int id){
    if(connections.find(id) != connections.end()){
        //std::cout << "adsfad\n";
        return false;
    }
    if(nodes.find(from) == nodes.end()){
        //std::cout << "adsfad\n";
        return false;
    }
    if(nodes.find(to) == nodes.end()){
        //std::cout << "adsfad\n";
        return false;
    }
    Connection c(from, to, strength, id);
    c.enabled = enabled;
    connections.insert(std::pair<int, Connection>(id, c));
    connectionIDs.push_back(id);
    nodes.at(from).out.push_back(id);
    nodes.at(to).in.push_back(id);
    return true;
}

int NeuralNet::addNode(){
    int id = historicalID++;
    Node n(nodeType::HIDDEN, id);
    nodes.insert(std::pair<int, Node>(id, n));;
    nodeIDs.push_back(id);
    //nodeMap.insert(std::pair<int, Node>(id, n));
    return id;
}

void NeuralNet::addNode(float bias, int id){
    if(nodes.find(id) != nodes.end()){
        return;
    }
    Node n(nodeType::HIDDEN, bias, id);
    nodes.insert(std::pair<int, Node>(id, n));;
    nodeIDs.push_back(id);
}

void NeuralNet::saveToFile(std::string fname){
    std::ofstream outFile(fname);
    outFile << inSize << ' ' << outSize << '\n';
    outFile << nodeIDs.size() << ' ' << connectionIDs.size() << ' ' << species << "\n\n";
    int sze = nodeIDs.size();
    for(int i = 0; i < sze; i ++){
        Node* n = &nodes.at(nodeIDs[i]);
        outFile << n->in.size() << ' ' << n->out.size() << ' ';
        for(int j = 0; j < n->in.size(); j ++){
            outFile << n->in[j] << ' ';
        }
        for(int j = 0; j < n->out.size(); j ++){
            outFile << n->out[j] << ' ';
        }
        outFile << n->id << ' ' << n->enabled << ' ' << static_cast<int>(n->type) << ' ' << n->value << ' ' << n->bias;
        outFile << '\n';
    }
    outFile << '\n';
    sze = connectionIDs.size();
    for(int i = 0; i < sze; i ++){
        Connection* c = &connections.at(connectionIDs[i]);
        outFile << c->id << ' ' << c->strength << ' ' << c->enabled << ' ' << c->from << ' ' << c->to << ' ';
        outFile << '\n';
    }
    outFile.close();
}

void NeuralNet::differentSpecies(){
    return;
    int bestMatch = 0;
    float bestScore = 100;
    for(auto it = speciesMap.begin(); it != speciesMap.end(); ++it){
        std::vector<int> speciesNodes = it->second.nodes;
        std::vector<int> speciesConnections = it->second.connections;
        int numGenes = speciesConnections.size() + speciesNodes.size();
        int found = 0;
        int notFound = 0;
        int sze = nodeIDs.size();
        
        for(int i = 0; i < sze; i ++){
            auto loc = std::find(speciesNodes.begin(), speciesNodes.end(), nodeIDs[i]);
            if(loc != speciesNodes.end()){
                found += 1;
                speciesNodes.erase(loc);
            } else {
                notFound += 1;
            }
        }
        sze = connectionIDs.size();
        for(int i = 0; i < sze; i ++){
            auto loc = std::find(speciesConnections.begin(), speciesConnections.end(), connectionIDs[i]);
            if(loc != speciesConnections.end()){
                found += 1;
                speciesConnections.erase(loc);
            } else {
                notFound += 1;
            }
        }
        float variation = notFound + speciesNodes.size() / 3.0 + speciesConnections.size() / 2.0;
        if(variation < bestScore){
            bestScore = variation;
            bestMatch = it->first;
        }
    }
    if(bestScore >= 5.0){
        int id = speciesID++;
        speciesMap.insert(std::pair<int, speciesHolder>(id, speciesHolder(nodeIDs, connectionIDs)));
        std::cout << "New species " << speciesID << " Species count = " << speciesMap.size() << '\n';
        species = id;
    } else {
        species = bestMatch;
    }
    speciesMap.at(species).count += 1;
}

bool NeuralNet::similar(NeuralNet& other){
    std::vector<int> otherNodes = other.nodeIDs;
    std::vector<int> otherConnections = other.connectionIDs;
    int numGenes = otherConnections.size() + otherNodes.size();
    int found = 0;
    float connectionDiffs = 0;
    int notFound = 0;
    int sameConnCount = 0;
    int N = nodeIDs.size() + connectionIDs.size();
    int otherN = otherNodes.size() + otherConnections.size();
    //Dunno if I shold take max, min, or average?
    N = otherN < N ? N : otherN;
    N += N == 0;
    int sze = nodeIDs.size();
    for(int i = 0; i < sze; i ++){
        auto loc = std::find(otherNodes.begin(), otherNodes.end(), nodeIDs[i]);
        if(loc != otherNodes.end()){
            found += 1;
            otherNodes.erase(loc);
        } else {
            notFound += 1;
        }
    }
    sze = connectionIDs.size();
    for(int i = 0; i < sze; i ++){
        auto loc = std::find(otherConnections.begin(), otherConnections.end(), connectionIDs[i]);
        if(loc != otherConnections.end()){
            found += 1;
            connectionDiffs += abs(other.connections.at(*loc).strength - connections.at(connectionIDs[i]).strength);
            sameConnCount += 1;
            otherConnections.erase(loc);
        } else {
            notFound += 1;
        }
    }
    //Number of genes not in other + number of genes not in this + differences in connections
    sameConnCount += sameConnCount == 0;
    float variation = (notFound + otherNodes.size() + otherConnections.size()) / (N * 2.0) + connectionDiffs / (sameConnCount * 10.0);
    //Think network might be too small for dividing by N
    //variation /= N;
    return variation < similarDistance;
}

void NeuralNet::remove(){
    // speciesMap.at(species).count -= 1;
    // if(speciesMap.at(species).count == 0){
    //     speciesMap.erase(species);
    // }
}

void NeuralNet::reset(){
    ticks = 0;
    inputAvg.resize(inSize);
    globalAvg.resize(inSize);
    std::fill(inputAvg.begin(), inputAvg.end(), 0);
    std::fill(globalAvg.begin(), globalAvg.end(), 0);
    times = 0;
    movement = 0;
    lastPos[0] = 0;
    lastPos[1] = 0;
    jumpTime = -1;
    jumpBuffer = 0;
    fitness = 0;
    pos = 0;
    emulator.reset();
}

int NeuralNet::chooseFromNode(){
    //Choose a random node based on the inputs
    //If an input node is different from what it normally is at the end of a run, it's more likely to be chosen when a connection is created
    std::vector<float> chances(inSize + nodeIDs.size() + 1);
    chances[0] = 0;
    int chancePos = 1;
    int sze = inSize;
    for(int i = 0; i < sze; i ++){
        float diff = abs(inputAvg[i] - globalAvg[i]);
        if(diff >= 0 && diff <= 2){
            chances[chancePos] = 1 + 3 * sqrt(diff) + chances[chancePos - 1];
        } else {
            chances[chancePos] = 1 + chances[chancePos - 1];
        }
        chancePos ++;
    }
    sze = nodeIDs.size();
    for(int i = 0; i < sze; i ++){
        chances[chancePos] = 5 + chances[chancePos - 1];
        chancePos ++;
    }
    int choice = rand() / static_cast<float>(RAND_MAX) * chances[chances.size() - 1];
    int ind = 0;
    while(ind < chances.size() - 2 && choice > chances[ind + 1]){
        ind++;
    }
    if(ind < inSize){
        return ind;
    } else {
        return nodeIDs[ind - inSize];
    }
}

void NeuralNet::run(){
    // if(playTest){
    //     show();
    // }
    fitness = 0;
    reset();
    emulator.run(cartridge);
    if(times == 0){
        fitness = 0; 
    } else {
        //fitness[i] = (static_cast<float>(pos[i]) * pos[i]) / static_cast<float>(times[i]) * 20.0;
        //fitness = pos * pos * 20.0 / times;
        //This is distance + speed
        fitness = static_cast<float>(pos) * (1 + 1.0 / times);
    }
    int numConnections = 0;
    for(int i = 0; i < connectionIDs.size(); i ++){
        if(connections.at(connectionIDs[i]).enabled){
            numConnections ++;
        }
    }
    //fitness -= numConnections / 3.0;
    fitness += emulator.m_bus.read(0x0760) * 1000;
    if(fitness <= 0){
        fitness = 0.001;
    }
}

//Both arguments default to 0
void NeuralNet::show(int baseNum, int genNum){
    basePath = "";
    frameNum = 0;
    if(!playTest){
        basePath += "framesOut/" + std::to_string(baseNum) + '/' + std::to_string(genNum);
        mkdir(basePath.c_str(), 0);
    }
    emulator.stop = false;
    movement = 0;
    emulator.show = true;
    float fitnessHold = fitness;
    emulator.reset();
    emulator.run(cartridge);
    fitness = fitnessHold;
    emulator.show = false;
}

void NeuralNet::removeRandomConnection(){
    if(connectionIDs.size() > 0){
        Connection conn = connections.at(connectionIDs[rand() % connectionIDs.size()]);
        Node* from = &(nodes.at(conn.from));
        Node* to = &(nodes.at(conn.to));
        from->out.erase(std::find(from->out.begin(), from->out.end(), conn.id));
        to->in.erase(std::find(to->in.begin(), to->in.end(), conn.id));
        connectionIDs.erase(std::find(connectionIDs.begin(), connectionIDs.end(), conn.id));
        connections.erase(conn.id);
    }
}

void NeuralNet::removeConnection(int id){
    Connection conn = connections.at(id);
    Node* from = &(nodes.at(conn.from));
    Node* to = &(nodes.at(conn.to));
    from->out.erase(std::find(from->out.begin(), from->out.end(), conn.id));
    to->in.erase(std::find(to->in.begin(), to->in.end(), conn.id));
    connectionIDs.erase(std::find(connectionIDs.begin(), connectionIDs.end(), conn.id));
    connections.erase(conn.id);
}

NeuralNet NeuralNet::crossOver(NeuralNet& other){
    NeuralNet out;
    std::vector<int> nodeList;
    std::vector<int> connectionList;
    bool same = similar(other);
    if((same && rand() % 10 <= 7) || rand() % 10 <= 1){
        int sze = nodeIDs.size();
        for(int i = 0; i < sze; i ++){
            if(rand() % 100 <= 97){
                out.addNode(nodes.at(nodeIDs[i]).bias + rand() / static_cast<float>(RAND_MAX) * 0.25 - 0.125, nodeIDs[i]);
            }
        }
        sze = other.nodeIDs.size();
        for(int i = 0; i < sze; i ++){
            if(rand() % 100 <= 97){
                out.addNode(other.nodes.at(other.nodeIDs[i]).bias + rand() / static_cast<float>(RAND_MAX) * 0.25 - 0.125, other.nodeIDs[i]);
            }
        }
        std::vector<int> otherConnections = other.connectionIDs;
        sze = connectionIDs.size();
        for(int i = 0; i < sze; i ++){
            Connection conn;
            //Random chance choosing between the two if both have gene with slight bias towards more fit
            auto otherHas = std::find(otherConnections.begin(), otherConnections.end(), connectionIDs[i]);
            if(otherHas != otherConnections.end() && rand() % 10 <= 2){
                conn = other.connections.at(connectionIDs[i]);
            } else {
                conn = connections.at(connectionIDs[i]);
            }
            if(otherHas != otherConnections.end()){
                otherConnections.erase(otherHas);
            }
            if(conn.enabled || rand() % 4 == 0){
                float strengthAdd = rand() / static_cast<float>(RAND_MAX) * 0.5 - 0.25;
                out.addConnection(conn.from, conn.to, conn.strength + strengthAdd, conn.enabled, conn.id);
            }
        }
        sze = otherConnections.size();
        for(int i = 0; i < sze; i ++){
            //Already looped through this, so get the nodes only other has
            Connection conn = other.connections.at(otherConnections[i]);
            if(conn.enabled || rand() % 4 == 0){
                float strengthAdd = rand() / static_cast<float>(RAND_MAX) * 0.5 - 0.25;
                out.addConnection(conn.from, conn.to, conn.strength + strengthAdd, conn.enabled, conn.id);
            }
        }
    } else {
        out = this;
    }
    return out;
}

void NeuralNet::merge(NeuralNet& other, float mutationRate, NeuralNet& out){
    out = crossOver(other);
    if(rand() % 10 <= 1){
        return;
    }
    while(mutationRate > 0){
        if(rand() / static_cast<float>(RAND_MAX) < mutationRate){
            int randNum = rand() % 20;
            if(randNum <= 9){
                //Insert a new connection
                //std::cout << "Add connection"<< '\n';
                int from = chooseFromNode();
                int to = rand() % outSize + inSize;
                if(rand() % (outSize + out.nodeIDs.size()) > outSize && out.nodeIDs.size() > 0 && from < inSize){
                    to = out.nodeIDs[rand() % out.nodeIDs.size()];
                }
                bool foundId = -1;
                //Ought to use a tree here or something
                for(int i = 0; i < recentConnections.size(); i ++){
                    if(recentConnections[i][0] == from && recentConnections[i][1] == to){
                        foundId = recentConnections[i][2];
                        break;
                    }
                }
                float strength = rand() / static_cast<float>(RAND_MAX) * 2 - 1.0;
                if(foundId == -1){
                    recentConnections.push_back(std::vector<int> {from, to, out.addConnection(from, to, strength)});
                } else {
                    out.addConnection(from, to, strength, true, foundId);
                }
            } else if(randNum <= 11){
                //std::cout << "Change connection"<< '\n';
                //Change a connection
                if(out.connectionIDs.size() > 0){
                    out.connections.at(out.connectionIDs[rand() % out.connectionIDs.size()]).strength += rand() / static_cast<float>(RAND_MAX) * 2 - 1;
                }
            } else if(randNum <= 12){
                if(rand() % 3 == 0){
                    //std::cout << "Add node"<< '\n';
                    if(out.connectionIDs.size() > 0){
                        Connection conn = out.connections.at(out.connectionIDs[rand() % out.connectionIDs.size()]);
                        if(conn.enabled){
                            conn.enabled = false;
                            int n = out.addNode();
                            out.nodes.at(n).bias = rand() / static_cast<float>(RAND_MAX) * 0.5 - 0.25;
                            out.addConnection(conn.from, n, conn.strength);
                            out.addConnection(n, conn.to, 1);
                        }
                    }
                }
            } else if(randNum <= 16) {
                //std::cout << "Disable connection"<< '\n';
                if(out.connectionIDs.size() > 0){
                    out.connections.at(out.connectionIDs[rand() % out.connectionIDs.size()]).enabled = !out.connections.at(out.connectionIDs[rand() % out.connectionIDs.size()]).enabled;
                }
            } else if(randNum <= 17){
                int n = 0;
                int r = rand() % (outSize + out.nodeIDs.size());
                if(r < outSize){
                    n = r + inSize;
                } else {
                    n = out.nodeIDs[r - outSize]; 
                }
                out.nodes.at(n).bias += rand() / static_cast<float>(RAND_MAX) * 1 - 0.5;
            } else {
                //Remove connection
                out.removeRandomConnection();
            }
        }
        mutationRate -= 1;
    }
    //out.differentSpecies();
}