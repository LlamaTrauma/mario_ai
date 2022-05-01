#include "NeuralNet.h"
#include "Emulator.h"
#include "StateGetter.h"

class Species;

//Holds all the ugly code in case I want to reuse the NEAT network class
class NetworkWrapper {
    public:
        NetworkWrapper();
        NetworkWrapper(NeuralNet net);
        NetworkWrapper(const NetworkWrapper* other);
        NetworkWrapper(const NetworkWrapper& other);
        NetworkWrapper& operator=(const NetworkWrapper& other);
        NetworkWrapper& operator=(const NetworkWrapper* other);
        void show(int baseNum = 0, int genNum = 0);
        void reset();
        void run(int saveFile);
        void saveToFile(std::string fname);
        bool similar(NetworkWrapper& other);
        unsigned int feedForward(std::vector<float> input);
        NetworkWrapper merge(NetworkWrapper& other);
        void setCallbacks();
        static void init();
        float fitness;
        Species* species;
        int complexity = 0;
        int frameNum = 0;
        static sn::Cartridge cartridge;
        static int inSize;
        static int outSize;
        bool finished = false;
    private:
        NeuralNet network;
        int ticks = 0;
        int jumpTime;
        int jumpBuffer;
        int pos;
        int initialPos;
        int lastPos[2];
        sn::Emulator emulator;
        int times;
        int lastInputs[4] = {0, 0, 0, 0};
        int movement;
        std::string basePath;
        std::vector<float> input;
        static bool initialized;
};