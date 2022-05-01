#include "Species.h"

Species::Species(){
    id = 0;
    age = 0;
    fittest = nullptr;
    bestFitness = 0;
    lastInnovation = 0;
    complexity = 1;
    count = 0;
    for(int i = 0; i < NUMSAVES; i ++){
        bestScores[i] = 0;
        finished[i] = false;
    }
}

// Species::Species(NetworkWrapper* base){
//     id = 0;
//     age = 0;
//     fittest = base;
//     bestFitness = 0;
//     lastInnovation = 0;
//     complexity = 1;
//     count = 0;
//     this->base = base;
//     for(int i = 0; i < NUMSAVES; i ++){
//         bestScores[i] = 0;
//         finished[i] = false;
//     }
//     base->species = this;
//     members.resize();
//     members.push_back(base);
// }

void Species::chooseSavefile(){
    //Try to get it to be less complex by making it not change scenarios so often?
    if(bestScores[saveFile] - baseline < 0.5){
        bestScores[saveFile] = 0;
        return;
    }
    int lowInd = 1;
    float lowScore = bestScores[1];
    for(int i = 2; i < NUMSAVES; i ++){
        if(bestScores[i] < lowScore && !finished[i]){
            lowScore = bestScores[i];
            lowInd = i;
        }
    }
    baseline = lowScore;
    if(lowScore < 1.75){
        saveFile = lowInd;
        bestScores[saveFile] = 0;
    } else {
        saveFile = 0;
        bestScores[0] = 0;
    }
}

void Species::updateBests(){
    fittest = members[0];
    for(int i = 0; i < members.size(); i ++){
        if(members[i]->fitness > fittest->fitness){
            fittest = members[i];
            fittestFitness = fittest->fitness;
        }
        if(members[i]->fitness > bestFitness){
            bestFitness = members[i]->fitness;
            complexity = members[i]->complexity;
            lastInnovation = 0;
        }
        if(members[i]->fitness > bestScores[saveFile]){
            bestScores[saveFile] = members[i]->fitness;
            //Dunno if this works right, disabled for now
            //finished[saveFile] = members[i]->finished;
        }
    }
    getCloseness();
}

void Species::getCloseness(){
    closeness = 0;
    for(int i = 0; i < NUMSAVES; i ++){
        avg += bestScores[i];
        if(i > 0){
            closeness += (bestScores[i] > 1.75) ? 1.75 : bestScores[i];
        } else {
            closeness += bestScores[i];
        }
    }
    avg /= NUMSAVES;
}

void Species::distributeFitnesses(){
    float otherBests = 0;
    for(int i = 0; i < NUMSAVES; i ++){
        if(i != saveFile){
            otherBests += bestScores[i];
        }
    }
    int numMembers = members.size();
    for(int i = 0; i < members.size(); i ++){
        //Even if a species is bad at a savefile it's rewarded for being good at other ones
        members[i]->fitness = (otherBests * (NUMSAVES - 1) + members[i]->fitness) / NUMSAVES;
        members[i]->fitness /= numMembers;
    }
}

void Species::refindMax(){
    getCloseness();
    if(fittest == nullptr || saveFile == 0){
        return;
    }
    int numMembers = members.size();
    float fitnessStore = fittest->fitness;
    for(int i = 0; i < NUMSAVES; i ++){
        fittest->run(i);
        bestScores[i] = fittest->fitness;
    }
    fittest->fitness = fitnessStore;
}

NetworkWrapper* Species::chooseRandomMember(){
    return members[rand() % members.size()];
}

void Species::update(){
    base = members[rand() % members.size()];
    age += 1;
    lastInnovation += 1;
    fittest = nullptr;
    members.resize(0);
}