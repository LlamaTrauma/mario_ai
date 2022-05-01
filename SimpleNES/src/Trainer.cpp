#include "Trainer.h"

//const int Trainer::MEMBERS = 150;

Trainer::Trainer(int num){
    generation = 0;
    maxFitness = 0;
    //retain = MEMBERS * 0.01;
    fileNum = num;
    bestCount = 0;
    lastShow = 0;
    for(int i = 0; i < NUMSAVES; i ++){
        bestScores[i] = 0.01;
    }
    for(int i = 0; i < MEMBERS; i ++){
        networks[i] = new NetworkWrapper();
    }
    speciesArr.resize(0);
}

void Trainer::partialRun(int start, int stop){
    for(int i = start; i < stop; i ++){
        networks[i].run(networks[i].species->saveFile);
        // networks[i].run(0);
        if(networks[i].fitness > maxFitness * 0.3 && networks[i].fitness > 1){
            std::cout << "Finished " << generation << ':' << i << " > " << networks[i].fitness << '\n';
        }
    }
}

void Trainer::runGeneration(){
    prepareGen();

    if(NeuralNet::playTest){
        networks[0].show();
    } else {
    //Run each network
        std::thread th1(&Trainer::partialRun, this, generation == 0 ? 0 : retain, retain + (MEMBERS - retain) / 4);
        std::thread th2(&Trainer::partialRun, this, retain + (MEMBERS - retain) / 4, retain + (MEMBERS - retain) / 2);
        std::thread th3(&Trainer::partialRun, this, retain + (MEMBERS - retain) / 2, retain + (MEMBERS - retain) * 3 / 4);
        std::thread th4(&Trainer::partialRun, this, retain + (MEMBERS - retain) * 3 / 4, MEMBERS);
        th1.join();
        th2.join();
        th3.join();
        th4.join();
    }

    updateSpecies();

    adjustFitnesses();

    logStuff();

    prepareNext();
}

void Trainer::prepareGen(){
    //Group the agents into species
    for(int i = 0; i < MEMBERS; i ++){
        int j = 0;
        for(; j < speciesArr.size(); j ++){
            if(networks[i].similar(speciesArr[j]->base)){
                networks[i].species = speciesArr[j];
                speciesArr[j]->members.push_back(&(networks[i]));
                break;
            }
        }
        if(j == speciesArr.size()){
            Species* a = new Species();
            speciesArr.push_back(a);
            a->base = networks[i];
            networks[i].species = a;
            a->members.push_back(&(networks[i]));
        }
    }

    //Clear the list of recent connections
    NeuralNet::recentConnections.resize(0);
    //Choose which save state to load
    for(int j = 0; j < speciesArr.size(); j ++){
        speciesArr[j]->chooseSavefile();
    }
}

void Trainer::updateSpecies(){
    maxval = 0.001;
    maxind = 0;
    for(int i = 0; i < MEMBERS; i ++){
        if(networks[i].fitness > maxval){
            maxind = i;
            maxval = networks[i].fitness;
        }
        if(networks[i].fitness > bestScores[networks[i].species->saveFile]){
            bestScores[networks[i].species->saveFile] = networks[i].fitness;
        }
    }

    for(int i = 0; i < speciesArr.size(); i ++){
        speciesArr[i]->updateBests();
    }

    //Deal with variables
    generation += 1;

    //Prime the top arrays
    //Store each network's fitness
    for(int i = 0; i < MEMBERS; i ++){
        if(i < retain){
            topVals[i] = 0;
            topInds[i] = -1;
        }
        fitnessStore[i] = networks[i].fitness;
    }

    //Complexity of the best network 
    float maxComplexity = networks[maxind].complexity;
    maxComplexity += maxComplexity < 0.5;
}

void Trainer::adjustFitnesses(){
    for(int i = 0; i < speciesArr.size(); i ++){
        speciesArr[i]->distributeFitnesses();
    }
    for(int i = 0; i < MEMBERS; i ++){
        if(fitnessStore[i] <= 0){
            networks[i].fitness = 0.01;
        }
        //They start on different savestates, so normalize the fitness
        //networks[i].fitness /= bestScores[networks[i].species->saveFile];
        int ind = retain;
        while(ind > 0 && topVals[ind - 1] < fitnessStore[i]){
            ind --;
        }
        if(ind < retain){
            carriedOver[i] = true;
            //If this member remains, make it less likely for others in the species to remain
            //speciesCount.at(spec) += 1;
            if(topInds[retain - 1] != -1){
                carriedOver[topInds[retain - 1]] = false;
            }
            for(int j = retain - 1; j > ind; j --){
                topVals[j] = topVals[j - 1];
                topInds[j] = topInds[j - 1];
            }
            topVals[ind] = fitnessStore[i];
            topInds[ind] = i;
        } else {
            carriedOver[i] = false;
        }
    }

    //Refind the maximum values
    for(int j = 0; j < speciesArr.size(); j ++){
        speciesArr[j]->refindMax(); 
    }
    //Find the maximum fitness
    //Generate a table to choose a network weighted by its fitness later
    randRange[0] = 0;
    for(int i = 0; i < MEMBERS; i ++){
        randRange[i + 1] = networks[i].fitness + randRange[i];
    }
}

void Trainer::logStuff(){
    Species* printSpec = speciesArr[0];
    for(int i = 1; i < speciesArr.size(); i ++){
        if(speciesArr[i]->closeness > printSpec->closeness){
            printSpec = speciesArr[i];
        }
    }

    //Print the best fitness, save and show it if conditions met
    std::ofstream logger("framesOut/" + std::to_string(fileNum) + '/' + std::to_string(generation) + ".txt");
    std::cout << "Best fitness of " << generation << ": " << maxval << '\n';
    std::cout << "Number of species: " << speciesArr.size() << '\n';
    std::cout << "Max fitnesses: \n";
    logger << "Best fitness of " << generation << ": " << maxval << '\n';
    logger << "Number of species: " << speciesArr.size() << '\n';
    
    std::cout << "Savefile fitnesses: " << '\n';
    logger << "Savefile fitnesses: " << '\n';
    for(int i = 0; i < NUMSAVES; i ++){
        std::cout << printSpec->bestScores[i] << ' ';
        logger << printSpec->bestScores[i] << ' ';
    }
    std::cout << '\n';
    logger << '\n';

    // logger << "Max fitnesses: \n";
    // for(int i = 0; i < retain; i ++){
    //     std::string msg = std::to_string(fitnessStore[topInds[i]]) + " | species: " + std::to_string(networks[topInds[i]]->species) + " | species size: " + std::to_string(speciesArr[networks[topInds[i]].species].members.size()) + " | species age: " + std::to_string(speciesArr[networks[topInds[i]].species].age) + " | species staleness: " + std::to_string(speciesArr[networks[topInds[i]].species].lastInnovation) + " | connections: " + std::to_string(networks[topInds[i]].complexity);
    //     std::cout << msg;
    //     logger << msg;
    //     if(speciesArr[networks[topInds[i]].species].fittest == topInds[i]){
    //         std::cout << " (best in species)";
    //         logger << " (best in species)";
    //     }
    //     std::cout << "\n";
    //     logger << "\n";
    // }
    std::cout << "\n";
    logger << "\n";
    for(int i = 0; i < speciesArr.size(); i ++){
        std::string msg = "species: " + std::to_string(i) + " | species size: " + std::to_string(speciesArr[i]->members.size()) + " | species age: " + std::to_string(speciesArr[i]->age) + " | species staleness: " + std::to_string(speciesArr[i]->lastInnovation) + " | best ever fitness " + std::to_string(speciesArr[i]->bestFitness) + " | complexity " + std::to_string(speciesArr[i]->complexity) + '\n';
        //std::cout << msg;
        logger << msg;
    }
    std::cout << '\n';
    logger.close();
    //Only show if best network is a new one
    if(generation % 5 == 0){
        float fitnessHold = networks[maxind].fitness;
        networks[maxind].show(fileNum, generation);
        networks[maxind].fitness = fitnessHold;
        //Move to folder, make video from images, delete images
        // && find . -name \"*.jpg\" -delete
        if(system(("cd " + ("framesOut/" + std::to_string(fileNum) + "/" + std::to_string(generation)) + " && ffmpeg -framerate 20 -i %05d.jpg -start_number 2 -c:v libx264 -pix_fmt yuv420p -q:v 5 fitness_" + std::to_string(static_cast<int>(maxval)) + ".mp4").c_str()) < 0){
            std::cout << "\nError making video\n";
        }
        //Save all the networks
        for(int i = 0; i < MEMBERS; i ++){
            networks[i].saveToFile("framesOut/" + std::to_string(fileNum) + '/' + std::to_string(generation) + '/' + std::to_string(i) + ".txt");
        }
    }
}

void Trainer::prepareNext(){
    if(maxval > maxFitness){
        maxFitness = maxval;
    }
    //merge existing networks
    for(int i = 0; i < retain; i ++){
        newNetworks[i] = networks[topInds[i]];
        newNetworks[i].fitness = fitnessStore[topInds[i]];
    }

    for(int i = retain; i < MEMBERS; i ++){
        float choice = rand() / static_cast<float>(RAND_MAX) * randRange[MEMBERS - 1];
        int ind = 0;
        while(ind < MEMBERS - 1 && choice > randRange[ind + 1]){
            ind ++;
        }
        NetworkWrapper* p2;
        if(rand() % 100 <= 97){
            p2 = networks[ind].species->chooseRandomMember();
        } else {
            p2 = &(networks[rand() % MEMBERS]);
        }
        if(networks[ind].fitness > p2->fitness){
            newNetworks[i] = networks[ind].merge(*p2);
        } else {
            newNetworks[i] = (*p2).merge(networks[ind]);
        }
    }

    //Update species array, randomly assign agent to represent species
    for(int i = 0; i < speciesArr.size(); i ++){
        if(speciesArr[i]->members.size() == 0){
            delete speciesArr[i];
            speciesArr.erase(speciesArr.begin() + i);
            i -= 1;
        } else {
            speciesArr[i]->update();
        }
    }

    std::cout << '\n';
    for(int i = 0; i < MEMBERS; i ++){
        networks[i] = newNetworks[i];
    }
}