#pragma once
#include <vector>
#include "NetworkWrapper.h"

class Species {
    public:
        Species();
        //Species(NetworkWrapper* base);
        void chooseSavefile();
        void updateBests();
        void distributeFitnesses();
        void refindMax();
        NetworkWrapper* chooseRandomMember();
        void update();
        void getCloseness();
        float avg = 0.01;
        int id;
        NetworkWrapper* fittest;
        int age;
        int lastInnovation;
        float bestFitness;
        int complexity;
        int saveFile = 1;
        const static int NUMSAVES = 4;
        float bestScores[NUMSAVES];
        bool finished[NUMSAVES];
        std::vector<NetworkWrapper*> members;
        int count = 0;
        NetworkWrapper base;
        //How close the species is to having a good fitness in all savestates
        float closeness = 0;
        float fittestFitness = 0;
        float baseline = 0;
};