#ifndef NEURALNET_H
#define NEURALNET_H

#include <vector>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include "helper.h"
#include "Log.h"
#include "Cartridge.h"
#include "StateGetter.h"
//std::find
#include <algorithm>

struct Connection {
    Connection(){
        this->from = -100;
        this->to = -100;
        this->strength = 100;
        this->id = -100;
        this->enabled = true;
    };
    Connection(int from, int to, float strength, int id){
        this->from = from;
        this->to = to;
        this->strength = strength;
        this->id = id;
        this->enabled = true;
    }
    // void operator=(Connection& other){
    //     from = other.from;
    //     to = other.to;
    //     strength = other.strength;
    //     id = other.id;
    //     enabled = other.enabled;
    // }
    int from;
    int to;
    int id;
    float strength;
    bool enabled;
};

enum nodeType {INPUT, HIDDEN, OUTPUT};

struct Node {
    Node(nodeType type, int id) : in(0), out(0) {
        this->type = type;
        this->id = id;
        this->enabled = true;
        this->value = 0;
        this->bias = 0;
    }
    Node(nodeType type, float bias, int id) : in(0), out(0) {
        this->type = type;
        this->id = id;
        this->enabled = true;
        this->value = 0;
        this->bias = bias;
    }
    Node(int in, int out, float bias, int id) : in(0), out(0) {
        (this->in).push_back(in);
        (this->out).push_back(out);
        this->id = id;
        this->enabled = true;
        this->value = 0;
        this->bias = 0;
        this->type = nodeType::HIDDEN;
    }
    // void operator=(Node& other){
    //     in = other.in;
    //     out = other.out;
    //     id = other.id;
    //     enabled = other.enabled;
    //     value = other.value;
    //     type = other.type;
    //     bias = other.bias;
    // }
    int id;
    float bias = 0;
    std::vector<int> in;
    std::vector<int> out;
    float value;
    bool enabled;
    nodeType type;
};

struct speciesHolder {
    speciesHolder() : nodes(0), connections(0) {
        count = 0;
    }
    speciesHolder(std::vector<int> nodes, std::vector<int> connections){
        this->nodes = nodes;
        this->connections = connections;
        count = 0;
    }
    void operator=(speciesHolder& other){
        nodes = other.nodes;
        connections = other.connections;
        count = other.count;
    }
    std::vector<int> nodes;
    std::vector<int> connections;
    int count = 0;
};

class NeuralNet {
    public:
        NeuralNet();
        NeuralNet(const NeuralNet* other);
        NeuralNet(const NeuralNet& other);
        NeuralNet& operator=(const NeuralNet& other);
        NeuralNet& operator=(const NeuralNet* other);
        NeuralNet(std::vector<int> layerSizesInput);
        NeuralNet(std::map<int, Node> nodes, std::map<int, Connection> connections, std::vector<int> nodeIDs, std::vector<int> connectionIDs);
        NeuralNet merge(NeuralNet& other);
        std::vector<float> feedForward(std::vector<float> input);
        static std::map<int, Node> nodeMap;
        static std::map<int, Connection> connectionMap;
        static void initialize(int inputSize, int outputSize);
        int addConnection(int from, int to, float strength);
        bool addConnection(int from, int to, float strength, bool enabled, int id);
        int addNode();
        void addNode(float bias, int id);
        float getValue(int node);
        void saveToFile(std::string fname); 
        void remove();
        void reset();
        void run();
        int chooseFromNode();
        bool similar(NeuralNet& other);
        void show(int baseNum = 0, int genNum = 0);
        void removeRandomConnection();
        void removeConnection(int id);
        NeuralNet crossOver(NeuralNet& other);
        static int historicalID;
        static int inSize;
        static int outSize;
        static bool initialized;
        static float pointMutation;
        static float similarDistance;
        static std::map<int, speciesHolder> speciesMap;
        static int speciesID;
        int species;
        static bool playTest;
        std::vector<int> nodeIDs;
        std::vector<int> connectionIDs;
        static std::vector<std::vector<int>> recentConnections;
        std::map<int, Connection> connections;
    private:
        std::map<int, Node> nodes;
        std::vector<float> inputAvg;
        std::vector<float> globalAvg;
        std::vector<float> input;
        int ticks = 0;
};

#endif