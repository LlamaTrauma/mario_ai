#ifndef EMULATOR_H
#define EMULATOR_H
#include <SFML/Graphics.hpp>
#include <chrono>

#include "CPU.h"
#include "PPU.h"
#include "MainBus.h"
#include "PictureBus.h"
#include "Controller.h"
#include "StateGetter.h"
#include "fstream"
#include <string.h>
#include <filesystem>
#include <sys/stat.h>

namespace sn
{
    struct state {
        state(MainBus& mBus, PictureBus& pBus, VirtualScreen& screen): cpu(mBus), ppu(pBus, screen), bus(), pictureBus() {};
        state(MainBus& mBus, PictureBus& pBus, VirtualScreen& screen, int overLoad): cpu(mBus), ppu(pBus, screen), bus(), pictureBus() {
            fromFile();
        }
        state(MainBus& mBus, PictureBus& pBus, VirtualScreen& screen, int overLoad, std::string folder): cpu(mBus), ppu(pBus, screen), bus(), pictureBus() {
            fromFile(folder);
        }
        void load(const MainBus& bus, const PictureBus& pictureBus, const CPU& cpu, const PPU& ppu) {
            this->bus = bus;
            this->pictureBus = pictureBus;
            this->cpu = cpu;
            this->ppu = ppu;
        };
        void toFile(){
            cpu.toFile("state/cpu.txt");
            ppu.toFile("state/ppu.txt");
            bus.toFile("state/mbus.txt");
            pictureBus.toFile("state/pbus.txt");
        }
        void fromFile(){
            cpu.fromFile("state/cpu.txt");
            ppu.fromFile("state/ppu.txt");
            bus.fromFile("state/mbus.txt");
            pictureBus.fromFile("state/pbus.txt");
        }
        void fromFile(std::string folder){
            cpu.fromFile(folder + "/cpu.txt");
            ppu.fromFile(folder + "/ppu.txt");
            bus.fromFile(folder + "/mbus.txt");
            pictureBus.fromFile(folder + "/pbus.txt");
        }
        MainBus bus;
        PictureBus pictureBus;
        CPU cpu;
        PPU ppu;
    };
    using TimePoint = std::chrono::high_resolution_clock::time_point;

    const int NESVideoWidth = ScanlineVisibleDots;
    const int NESVideoHeight = VisibleScanlines;

    class Emulator
    {
    public:
        Emulator();
        void run(Cartridge& m_cartidge, int saveFile = 0);
        void setVideoWidth(int width);
        void setVideoHeight(int height);
        void setVideoScale(float scale);
        void setKeys(std::vector<sf::Keyboard::Key>& p1, std::vector<sf::Keyboard::Key>& p2);
        void saveState();
        void loadState(int saveFile = 0);
        void stopRun();
        void reset();
        void setInputCallback(std::function<void(void)> cb);
        void setDrawCallback(std::function<void(void)> cb);
        StateGetter stateGetter;
        Controller m_controller1, m_controller2;
        bool stop = false;
        bool show = false;
        std::function<void(void)> drawCallback = NULL;
        sf::RenderWindow m_window;
        MainBus m_bus;
        static std::vector<state> states;

    private:
        void DMA(Byte page);
        std::function<void(void)> inputCallback = NULL;

        PictureBus m_pictureBus;
        CPU m_cpu;
        PPU m_ppu;
        Cartridge m_cartridge;
        std::unique_ptr<Mapper> m_mapper;

        VirtualScreen m_emulatorScreen;
        float m_screenScale;

        TimePoint m_cycleTimer;

        std::chrono::high_resolution_clock::duration m_elapsedTime;
        std::chrono::nanoseconds m_cpuCycleDuration;
    };
}

#endif // EMULATOR_H
