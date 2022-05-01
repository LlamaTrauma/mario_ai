#include "Trainer.h"

//const int Trainer::MEMBERS = 150;

Trainer::Trainer(int num){
    generation = 0;
    maxFitness = 0;
    retain = MEMBERS * 0.4;
    fileNum = num;
    bestCount = 0;
}

void Trainer::partialRun(int start, int stop){
    for(int i = start; i < stop; i ++){
        networks[i].run();
        if(networks[i].fitness > maxFitness * 0.3 && networks[i].fitness > 1){
            std::cout << "Finished " << generation << ':' << i << " > " << networks[i].fitness << '\n';
        }
    }
}

void Trainer::runGeneration(){
    //Clear the list of recent connections
    NeuralNet::recentConnections.resize(0);
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

    float maxval = 0;
    int maxind = 0;
    for(int i = 0; i < MEMBERS; i ++){
        if(networks[i].fitness > maxval){
            maxind = i;
            maxval = networks[i].fitness;
        }
    }

    //Deal with variables
    generation += 1;
    float avgFitness = 0;
    float topVals[retain];
    int topInds[retain];
    //std::map<int, int> speciesCount;
    float fitnessStore[MEMBERS];

    //Prime the top arrays
    //Store each network's fitness
    for(int i = 0; i < MEMBERS; i ++){
        if(i < retain){
            topVals[i] = 0;
            topInds[i] = -1;
        }
        fitnessStore[i] = networks[i].fitness;
    }

    //Group the agents into species
    speciesMembers.resize(species.size());
    speciesFittest.resize(species.size());
    std::fill(speciesMembers.begin(), speciesMembers.end(), std::vector<int>(0));
    std::fill(speciesFittest.begin(), speciesFittest.end(), -1);
    for(int i = 0; i < MEMBERS; i ++){
        int j = 0;
        for(; j < species.size(); j ++){
            if(networks[i].similar(species[j])){
                networks[i].species = j;
                speciesMembers[j].push_back(i);
                if(speciesFittest[j] == -1 || networks[i].fitness > networks[speciesFittest[j]].fitness){
                    speciesFittest[j] = i;
                }
                break;
            }
        }
        if(j == species.size()){
            species.push_back(networks[i]);
            speciesAges.push_back(0);
            speciesMembers.push_back(std::vector<int> {i});
            speciesFittest.push_back(i);
            networks[i].species = j;
        }
    }

    int bestCount = 0;

    //Generate the list of most fit species, flag any that won't be carried over
    bool carriedOver[MEMBERS];
    //Restore the fitnesses
    for(int i = 0; i < MEMBERS; i ++){
        //I was using the fitness of the network instead of the fitnessStore value so new bests sometimes would just not be kept
        int fittest = speciesFittest[networks[i].species];
        //Keep the best ones in, but in case of ties limit it a bit
        if(networks[i].fitness < maxval){
            //Added sqrt to not punish large species as much
            networks[i].fitness /= speciesMembers[networks[i].species].size();
        } else if(bestCount ++ >= 3){
            networks[i].fitness /= speciesMembers[networks[i].species].size();
        }
        float prop = fitnessStore[fittest] / (maxFitness == 0 ? 0.01 : maxFitness);
        // if(prop < 0.6){
        //     networks[i].fitness *= prop + 0.4;
        // }
        //Being kind of harsh here because small unfit species will remain in the population and crowd out better ones 
        int specAge = speciesAges[networks[i].species];
        int specSize = speciesMembers[networks[i].species].size();
        if(specAge >= 3 && specSize <= 2){
            networks[i].fitness *= 0.01;
        }
        if(specAge >= 10 && (specSize <= 15 || prop < 0.6)){
            networks[i].fitness *= 0.01;//prop * pow(0.9, static_cast<float>(specAge - 10));
        }
        if(specAge > 20 && prop < 0.9){
            networks[i].fitness *= pow(0.98, specAge - 20);
        }
        //avgFitness += fitness[i];
        int ind = retain;
        while(ind > 0 && topVals[ind - 1] < networks[i].fitness){
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
            topVals[ind] = networks[i].fitness;
            topInds[ind] = i;
        } else {
            carriedOver[i] = false;
        }
    }

    for(int i = 0; i < MEMBERS; i ++){
        networks[i].fitness = fitnessStore[i];
    }

    //Find the maximum fitness
    //Generate a table to choose a network weighted by its fitness later
    float randRange[MEMBERS + 1];
    randRange[0] = 0;
    for(int i = 0; i < MEMBERS; i ++){
        randRange[i + 1] = networks[i].fitness + randRange[i];
    }

    //Print the best fitness, save and show it if conditions met
    std::ofstream logger("framesOut/" + std::to_string(fileNum) + '/' + std::to_string(generation) + ".txt");
    std::cout << "Best fitness of " << generation << ": " << maxval << '\n';
    std::cout << "Number of species: " << species.size() << '\n';
    std::cout << "Max fitnesses: \n";
    logger << "Best fitness of " << generation << ": " << maxval << '\n';
    logger << "Number of species: " << species.size() << '\n';
    logger << "Max fitnesses: \n";
    for(int i = 0; i < 10; i ++){
        std::string msg = std::to_string(fitnessStore[topInds[i]]) + " | species: " + std::to_string(networks[topInds[i]].species) + " | species size: " + std::to_string(speciesMembers[networks[topInds[i]].species].size()) + " | species age: " + std::to_string(speciesAges[networks[topInds[i]].species]) + " | connections: " + std::to_string(networks[topInds[i]].connectionIDs.size()) + " | nodes: " + std::to_string(networks[topInds[i]].nodeIDs.size());
        std::cout << msg;
        logger << msg;
        if(speciesFittest[networks[topInds[i]].species] == topInds[i]){
            std::cout << " (best in species)";
            logger << " (best in species)";
        }
        std::cout << '\n';
        logger << '\n';
    }
    for(int i = 10; i < retain; i ++){
        std::string msg = std::to_string(fitnessStore[topInds[i]]) + " | species: " + std::to_string(networks[topInds[i]].species) + " | species size: " + std::to_string(speciesMembers[networks[topInds[i]].species].size()) + " | species age: " + std::to_string(speciesAges[networks[topInds[i]].species]) + " | connections: " + std::to_string(networks[topInds[i]].connectionIDs.size()) + " | nodes: " + std::to_string(networks[topInds[i]].nodeIDs.size());
        logger << msg;
        if(speciesFittest[networks[topInds[i]].species] == topInds[i]){
            logger << " (best in species)";
        }
        logger << '\n';
    }
    logger.close();
    //Only show if best network is a new one
    if((maxval > (maxFitness + 1))){
        //networks[maxind].saveToFile("bestRuns7/gen_" + std::to_string(generation) + "_" + std::to_string(int(maxval)) + ".txt");
        networks[maxind].show(fileNum, generation);
        networks[maxind].fitness = maxval;
        //Move to folder, make video from images, delete images
        // && find . -name \"*.jpg\" -delete
        if(system(("cd " + ("framesOut/" + std::to_string(fileNum) + "/" + std::to_string(generation)) + " && ffmpeg -framerate 60 -i %05d.jpg -start_number 2 -c:v libx264 -pix_fmt yuv420p -q:v 5 fitness_" + std::to_string(static_cast<int>(maxval)) + ".mp4").c_str()) < 0){
            std::cout << "\nError making video\n";
        }
        //Save all the networks
        for(int i = 0; i < MEMBERS; i ++){
            networks[i].saveToFile("framesOut/" + std::to_string(fileNum) + '/' + std::to_string(generation) + '/' + std::to_string(i) + ".txt");
        }
    }

    if(maxval > maxFitness){
        maxFitness = maxval;
    }
    
    //Make the list of new networks
    NeuralNet newNetworks[MEMBERS];

    //merge existing networks
    for(int i = retain; i < MEMBERS; i ++){
        float choice = rand() / static_cast<float>(RAND_MAX) * randRange[MEMBERS - 1];
        int ind = 0;
        while(ind < MEMBERS - 1 && choice > randRange[ind + 1]){
            ind ++;
        }
        int ind2 = 0;
        int attempts = 0;
        do{
            float choice2 = rand() / static_cast<float>(RAND_MAX) * randRange[MEMBERS - 1];
            ind2 = 0;
            while(ind2 < MEMBERS - 1 && choice2 > randRange[ind2 + 1]){
                ind2 ++;
            }
        } while(networks[ind].species != networks[ind2].species && attempts++ < 5);
        if(networks[ind].fitness > networks[ind2].fitness){
            networks[ind].merge(networks[ind2], mutationRate, newNetworks[i]);
        } else {
            networks[ind2].merge(networks[ind], mutationRate, newNetworks[i]);
        }
    }

    for(int i = 0; i < retain; i ++){
        newNetworks[i] = networks[topInds[i]];
    }

    //Update species array, randomly assign agent to represent species
    for(int i = 0; i < species.size(); i ++){
        for(int j = 0; j < speciesMembers[i].size(); j ++){
            if(!carriedOver[speciesMembers[i][j]]){
                speciesMembers[i].erase(speciesMembers[i].begin() + j);
                j --;
            }
        }
        if(speciesMembers[i].size() == 0){
            speciesMembers.erase(speciesMembers.begin() + i);
            species.erase(species.begin() + i);
            speciesAges.erase(speciesAges.begin() + i);
            i --;
        } else {
            species[i] = networks[speciesMembers[i][rand() % speciesMembers[i].size()]];
            speciesAges[i] += 1;
        }
        //If species has no members or its fittest member leaves it
        // if(speciesFittest[i] == -1 || !carriedOver[speciesFittest[i]]){
        //     species.erase(species.begin() + i);
        //     i --;
        // } else {
        //     species[i] = networks[speciesFittest[i]];
        // }        
    }

    //Just added, might help idk
    if(species.size() > 15){
        NeuralNet::similarDistance += 0.005;
    } else if(species.size() < 5){
        NeuralNet::similarDistance -= 0.005;
    }

    std::cout << '\n';
    for(int i = 0; i < MEMBERS; i ++){
        networks[i] = newNetworks[i];
    }
}