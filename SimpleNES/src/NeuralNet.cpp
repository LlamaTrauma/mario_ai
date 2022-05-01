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
//0.9 (multiplied coefficients by 2)
//0.9
float NeuralNet::similarDistance = 0.9;
float NeuralNet::pointMutation = 0.75;
bool NeuralNet::playTest = false;
std::vector<std::vector<int>> NeuralNet::recentConnections(0);
std::map<int, Node> NeuralNet::nodeMap = std::map<int, Node>();
std::map<int, Connection> NeuralNet::connectionMap = std::map<int, Connection>();
std::map<int, speciesHolder> NeuralNet::speciesMap = std::map<int, speciesHolder>();

void NeuralNet::initialize(int inputSize, int outputSize){
    NeuralNet::inSize = inputSize;
    NeuralNet::outSize = outputSize;
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
    // static std::vector<int> initialConnections;
    if(!initialized){
        initialize(StateGetter::SIZE, 4);
        // for(int i = 0; i < 100; i ++){
        //     initialConnections.push_back(rand() % inSize);
        //     initialConnections.push_back(inSize + rand() % outSize);
        //     initialConnections.push_back(rand() / static_cast<float>(RAND_MAX) - 0.5);
        //     initialConnections.push_back(historicalID++);
        // }
    }
    for(int i = 0; i < inSize + outSize; i ++){
        nodes.insert(std::pair<int, Node>(i, nodeMap.at(i)));
    }
    inputAvg.resize(inSize);
    std::fill(inputAvg.begin(), inputAvg.end(), 0);
    globalAvg.resize(inSize);
    std::fill(globalAvg.begin(), globalAvg.end(), 0);
    //Connect an always active node and the jump button
    //addConnection(inSize - 2, inSize, rand() / static_cast<float>(RAND_MAX) * 0.5 - 0.25, true, -2);
    //Connect grounded and a button nodes
    //addConnection(inSize - 1, inSize, rand() / static_cast<float>(RAND_MAX) * 0.5 - 0.25, true, -3);
    //Connect jump pressed and a button nodes
    //addConnection(inSize - 3, inSize, rand() / static_cast<float>(RAND_MAX) * -2, true, -5);
    int numToAdd = rand() % 3;
    while(numToAdd --> 0){
        // int addPos = rand() % initialConnections.size() / 4 * 4;
        // addConnection(initialConnections[addPos], initialConnections[addPos + 1], initialConnections[addPos + 2], true, initialConnections[addPos + 3]);
        addConnection(rand() % inSize, rand() % outSize + inSize, rand() / static_cast<float>(RAND_MAX) * 2.0 - 1.0, true, historicalID++);
    }
}

NeuralNet::NeuralNet(const NeuralNet* other) {
    nodes = other->nodes;
    connections = other->connections;
    nodeIDs = other->nodeIDs;
    connectionIDs = other->connectionIDs;
}

NeuralNet& NeuralNet::operator=(const NeuralNet* other) {
    nodes = other->nodes;
    connections = other->connections;
    nodeIDs = other->nodeIDs;
    connectionIDs = other->connectionIDs;
    return *this;
}

NeuralNet& NeuralNet::operator=(const NeuralNet& other) {
    nodes = other.nodes;
    connections = other.connections;
    nodeIDs = other.nodeIDs;
    connectionIDs = other.connectionIDs;
    return *this;
}

NeuralNet::NeuralNet(const NeuralNet& other) {
    nodes = other.nodes;
    connections = other.connections;
    nodeIDs = other.nodeIDs;
    connectionIDs = other.connectionIDs;
}

NeuralNet::NeuralNet(std::map<int, Node> nodes, std::map<int, Connection> connections, std::vector<int> nodeIDs, std::vector<int> connectionIDs) {
    this->nodes = nodes;
    this->connections = connections;
    this->nodeIDs = nodeIDs;
    this->connectionIDs = connectionIDs;
}

std::vector<float> NeuralNet::feedForward(std::vector<float> input){
    this->input = input;
    const int LOCALRANGE = 1;
    for(int i = 0; i < inSize; i ++){
        //Arbitrary, a second seems good
        //Actually this seems like it's called more than 60 times per second Is hould find out why probably
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
    return output;
}

float NeuralNet::getValue(int node){
    Node* n = &(nodes.at(node));
    if(abs(n->value) > 0.001){
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
    N = otherN > N ? otherN : N;
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
    float variation = (notFound + otherNodes.size() + otherConnections.size()) / (static_cast<float>(N)) + connectionDiffs / (sameConnCount * 5.0);
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
    inputAvg.resize(inSize);
    globalAvg.resize(inSize);
    std::fill(inputAvg.begin(), inputAvg.end(), 0);
    std::fill(globalAvg.begin(), globalAvg.end(), 0);
    ticks = 0;
}

int NeuralNet::chooseFromNode(){
    //Choose a random node based on the inputs
    //If an input node is different from what it normally is at the end of a run, it's more likely to be chosen when a connection is created
    std::vector<float> chances(inSize + nodeIDs.size() + 1);
    chances[0] = 0;
    int chancePos = 1;
    int sze = inSize;
    for(int i = 0; i < sze; i ++){
        //float diff = abs(inputAvg[i] - globalAvg[i]);
        float diff = abs(inputAvg[i]);
        //I disabled this during run 336 (so disabled 337 onward) idk if I should reenable
        if(0 && diff >= 0 && diff <= 2){
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
    if((same && rand() % 10 <= 7) || rand() % 100 <= 2){
        int sze = nodeIDs.size();
        for(int i = 0; i < sze; i ++){
            if(rand() % 100 <= 97){
                out.addNode(nodes.at(nodeIDs[i]).bias, nodeIDs[i]);
            }
        }
        sze = other.nodeIDs.size();
        for(int i = 0; i < sze; i ++){
            if(rand() % 100 <= 97){
                out.addNode(other.nodes.at(other.nodeIDs[i]).bias, other.nodeIDs[i]);
            }
        }
        std::vector<int> otherConnections = other.connectionIDs;
        sze = connectionIDs.size();
        for(int i = 0; i < sze; i ++){
            Connection conn;
            //Random chance choosing between the two if both have gene with slight bias towards more fit
            auto otherHas = std::find(otherConnections.begin(), otherConnections.end(), connectionIDs[i]);
            if(otherHas != otherConnections.end() && rand() % 2){
                conn = other.connections.at(connectionIDs[i]);
            } else {
                conn = connections.at(connectionIDs[i]);
            }
            if(otherHas != otherConnections.end()){
                otherConnections.erase(otherHas);
            }
            if(conn.enabled || rand() % 4 == 0){
                out.addConnection(conn.from, conn.to, conn.strength, conn.enabled, conn.id);
            }
        }
        sze = otherConnections.size();
        for(int i = 0; i < sze; i ++){
            //Already looped through this, so get the nodes only other has
            Connection conn = other.connections.at(otherConnections[i]);
            if(conn.enabled || rand() % 4 == 0){
                out.addConnection(conn.from, conn.to, conn.strength, conn.enabled, conn.id);
            }
        }
    } else {
        out = this;
    }
    return out;
}

NeuralNet NeuralNet::merge(NeuralNet& other){
    NeuralNet out = crossOver(other);
    // if(rand() % 10 <= 0){
    //     return;
    // }
    if(rand() % 1000 <= 10){
        //Insert a new connection
        //std::cout << "Add connection"<< '\n';
        int from = out.chooseFromNode();
        int to = rand() % outSize + inSize;
        if(rand() % (outSize + out.nodeIDs.size()) > outSize && out.nodeIDs.size() > 0 && from < inSize){
            to = out.nodeIDs[rand() % out.nodeIDs.size()];
        }
        bool foundId = -1;
        //Ought to use a tree here or something
        for(int i = 0; i < recentConnections.size(); i += 1){
            if(recentConnections[i][0] == from && recentConnections[i][1] == to){
                foundId = recentConnections[i][2];
                //std::cout << "ADSfads\n";
                break;
            }
        }
        float strength = rand() / static_cast<float>(RAND_MAX) * 2 - 1.0;
        if(foundId == -1){
            recentConnections.push_back(std::vector<int> {from, to, out.addConnection(from, to, strength)});
        } else {
            out.addConnection(from, to, strength, true, foundId);
        }
    }
    if(rand() % 100 <= 50){
        //std::cout << "Change connection"<< '\n';
        //Change a connection
        float strengthAdd = 0;
        if(rand() / static_cast<float>(RAND_MAX) < pointMutation){
            strengthAdd = rand() / static_cast<float>(RAND_MAX) * 0.5 - 0.25;
        } else {
            strengthAdd = rand() / static_cast<float>(RAND_MAX) * 2.0 - 1.0;
        }
        if(out.connectionIDs.size() > 0){
            out.connections.at(out.connectionIDs[rand() % out.connectionIDs.size()]).strength += strengthAdd;
        }
    }
    if(rand() % 100 <= 1){
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
    if(rand() % 100 <= 40) {
        //std::cout << "Toggle connection"<< '\n';
        if(out.connectionIDs.size() > 0){
            out.connections.at(out.connectionIDs[rand() % out.connectionIDs.size()]).enabled = false;
        }
    }
    if(rand() % 100 <= 20) {
        //std::cout << "Toggle connection"<< '\n';
        if(out.connectionIDs.size() > 0){
            out.connections.at(out.connectionIDs[rand() % out.connectionIDs.size()]).enabled = true;
        }
    }
    if(rand() % 100 <= 50){
        if(out.nodeIDs.size() > 0){
            int n = 0;
            int r = rand() % out.nodeIDs.size();
            out.nodes.at(out.nodeIDs[r]).bias += rand() / static_cast<float>(RAND_MAX) * 2.0 - 1.0;
        }
    }
    return out;
}