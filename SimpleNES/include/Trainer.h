#ifndef TRAINER_H
#define TRAINER_H
#include "Emulator.h"
#include <thread>
#include "Species.h"
#include "Cartridge.h"
#include <time.h>
#include "stateGetter.h"

//Should add species ids and staleness probably
class Trainer {
public:
    Trainer(int num);
    void runGeneration();
    void partialRun(int start, int stop);
    void prepareGen();
    void updateSpecies();
    void adjustFitnesses();
    void logStuff();
    void prepareNext();
private:
    const static int MEMBERS = 250;
    int generation;
    const static int retain = 1;
    NetworkWrapper networks[250];
    NetworkWrapper newNetworks[MEMBERS];
    sn::Cartridge cartridge;
    float maxFitness;
    std::vector<Species*> speciesArr;
    const static int NUMSAVES = Species::NUMSAVES;
    float bestScores[NUMSAVES];
    int fileNum = 0;
    int bestCount = 0;
    float lastShow;
    float avgFitness = 0;
    float topVals[retain];
    int topInds[retain];
    float randRange[MEMBERS + 1];
    float fitnessStore[MEMBERS];
    bool carriedOver[MEMBERS];
    int maxind;
    float maxval;
};

#endif