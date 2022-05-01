#ifndef TRAINER_H
#define TRAINER_H
#include "Emulator.h"
#include <thread>
#include "NeuralNet.h"
#include "Cartridge.h"
#include <time.h>
#include "stateGetter.h"

//Should add species ids and staleness probably
class Trainer {
public:
    Trainer(int num);
    void runGeneration();
    void partialRun(int start, int stop);
private:
    const static int MEMBERS = 250;
    int generation;
    int retain;
    NeuralNet networks[250];
    sn::Cartridge cartridge;
    float maxFitness;
    std::vector<NeuralNet> species;
    std::vector<int> speciesAges;
    std::vector<std::vector<int>> speciesMembers;
    std::vector<int> speciesFittest;
    float mutationRate = 2.0;
    int fileNum = 0;
    int bestCount = 0;
};

#endif