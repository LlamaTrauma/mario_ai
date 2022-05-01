#include "Emulator.h"
#include "Log.h"

#include <thread>
#include <chrono>

std::vector<sn::state> sn::Emulator::states = std::vector<sn::state>();

namespace sn
{
    Emulator::Emulator() :
        m_cpu(m_bus),
        m_ppu(m_pictureBus, m_emulatorScreen),
        m_screenScale(2.0f),
        m_cycleTimer(),
        m_cpuCycleDuration(std::chrono::nanoseconds(559)),
        stateGetter(&m_bus)
    {
        if(!m_bus.setReadCallback(PPUSTATUS, [&](void) {return m_ppu.getStatus();}) ||
            !m_bus.setReadCallback(PPUDATA, [&](void) {return m_ppu.getData();}) ||
            !m_bus.setReadCallback(JOY1, [&](void) {return m_controller1.read();}) ||
            !m_bus.setReadCallback(JOY2, [&](void) {return m_controller2.read();}) ||
            !m_bus.setReadCallback(OAMDATA, [&](void) {return m_ppu.getOAMData();}))
        {
            LOG(Error) << "Critical error: Failed to set I/O callbacks" << std::endl;
        }


        if(!m_bus.setWriteCallback(PPUCTRL, [&](Byte b) {m_ppu.control(b);}) ||
            !m_bus.setWriteCallback(PPUMASK, [&](Byte b) {m_ppu.setMask(b);}) ||
            !m_bus.setWriteCallback(OAMADDR, [&](Byte b) {m_ppu.setOAMAddress(b);}) ||
            !m_bus.setWriteCallback(PPUADDR, [&](Byte b) {m_ppu.setDataAddress(b);}) ||
            !m_bus.setWriteCallback(PPUSCROL, [&](Byte b) {m_ppu.setScroll(b);}) ||
            !m_bus.setWriteCallback(PPUDATA, [&](Byte b) {m_ppu.setData(b);}) ||
            !m_bus.setWriteCallback(OAMDMA, [&](Byte b) {DMA(b);}) ||
            !m_bus.setWriteCallback(JOY1, [&](Byte b) {m_controller1.strobe(b); m_controller2.strobe(b);
                if(b == 0 && inputCallback){
                    inputCallback();
                }
                //stateGetter.getState();
            }) ||
            !m_bus.setWriteCallback(OAMDATA, [&](Byte b) {m_ppu.setOAMData(b);})
            //Detect when CurrentColumnPos increases
            //|| !m_bus.setWriteCallback(CURRENTCOLUMNPOS, [&](Byte b) {stateGetter.getState();})
            )
        {
            LOG(Error) << "Critical error: Failed to set I/O callbacks" << std::endl;
        }

        m_ppu.setInterruptCallback([&](){ m_cpu.interrupt(CPU::NMI); });
    }

    void Emulator::stopRun(){
        stop = true;
    }

    void Emulator::run(Cartridge& m_cartridge, int saveFile)
    {
        bool printAll = false;
        //int numDraws = 0;
        //int numSteps = 0;
        
        m_mapper = Mapper::createMapper(static_cast<Mapper::Type>(m_cartridge.getMapper()),
                                        m_cartridge,
                                        [&](){ m_pictureBus.updateMirroring(); });
        if (!m_mapper)
        {
            LOG(Error) << "Creating Mapper failed. Probably unsupported." << std::endl;
            return;
        }

        if (!m_bus.setMapper(m_mapper.get()) ||
            !m_pictureBus.setMapper(m_mapper.get()))
            return;

        m_cpu.reset();
        m_ppu.reset();

        if(show){
            m_window.create(sf::VideoMode(NESVideoWidth * m_screenScale, NESVideoHeight * m_screenScale),
                            "SimpleNES", sf::Style::Titlebar | sf::Style::Close);
            m_window.setVerticalSyncEnabled(true);
            m_emulatorScreen.create(NESVideoWidth, NESVideoHeight, m_screenScale, sf::Color::White);
        }

        m_cycleTimer = std::chrono::high_resolution_clock::now();
        m_elapsedTime = m_cycleTimer - m_cycleTimer;
        loadState(saveFile);
        sf::Event event;
        bool focus = true, pause = false;
        //while (m_window.isOpen())
        while(1)
        {
            while (m_window.pollEvent(event)) {
            //     if (event.type == sf::Event::Closed ||
            //     (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            //     {
            //         m_window.close();
            //         return;
            //     }
            //     else if (event.type == sf::Event::GainedFocus)
            //     {
            //         focus = true;
            //         m_cycleTimer = std::chrono::high_resolution_clock::now();
            //     }
            //     else if (event.type == sf::Event::LostFocus)
            //         focus = false;
            //     else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2)
            //     {
            //         pause = !pause;
            //         if (!pause)
            //             m_cycleTimer = std::chrono::high_resolution_clock::now();
            //     }
            //     else if (pause && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F3)
            //     {
            //         for (int i = 0; i < 29781; ++i) //Around one frame
            //         {
            //             //PPU
            //             m_ppu.step();
            //             m_ppu.step();
            //             m_ppu.step();
            //             //CPU
            //             m_cpu.step();
            //             numSteps += 1;
            //         }
            //     }
            //     else if (focus && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F4)
            //     {
            //         Log::get().setLevel(Info);
            //     }
            //     else if (focus && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F5)
            //     {
            //         Log::get().setLevel(InfoVerbose);
            //     }
            //     else if (focus && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F6)
            //     {
            //         Log::get().setLevel(CpuTrace);
            //     }
                // if (focus && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F7)
                // {
                //     LOG(Info) << "save";
                //     saveState();
                // }
                // if (focus && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F8)
                // {
                //     LOG(Info) << "load";
                //     loadState();
                // }
            //     else if (focus && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F9)
            //     {
            //         printAll = true;
            //     }
            }

            // if (focus && !pause)
            // {
                m_elapsedTime += std::chrono::high_resolution_clock::now() - m_cycleTimer;
                m_cycleTimer = std::chrono::high_resolution_clock::now();
                //If in losing state or y position is below screen or stop flag set
                if(m_bus.read(0x0E) == 0x0B || (/*m_bus.read(0x03B8) / 16 > 14 && */m_bus.read(0xB5) > 0x01) || stop){
                    //LOG(sn::Info) << int(m_bus.read(0x075A)) << '\n';
                    m_window.close();
                    return;
                }
            //    while (m_elapsedTime > m_cpuCycleDuration)
                for(int i = 0; i < 29781; i ++)
                {
                        //PPU
                        if(show){
                            m_ppu.step();
                            m_ppu.step();
                            m_ppu.step();
                        } else {
                            m_ppu.fastStep();
                            m_ppu.fastStep();
                            m_ppu.fastStep();
                        }
                        //CPU
                        // if(printAll){
                        //     LOG(Info) << static_cast<int>(m_bus.read(m_cpu.r_PC)) << '\n';
                        // }
                        m_cpu.step();
                        //numSteps += 1;
                    m_elapsedTime -= m_cpuCycleDuration;    
                }
                //numDraws += 1;
                //if(1){//1700000 / 30){
                    //numDraws = 0;
                    if(show){
                        m_window.draw(m_emulatorScreen);
                        //stateGetter.drawToScreen(m_window);
                        if(drawCallback){
                            drawCallback();
                        }
                        //m_window.display();
                    }
                //}
            // }
            // else
            // {
            //     //sf::sleep(sf::milliseconds(1000/60));
            //     //std::this_thread::sleep_for(std::chrono::milliseconds(1000/60)); //1/60 second
            // }
        }
    }

    void Emulator::DMA(Byte page)
    {
        m_cpu.skipDMACycles();
        auto page_ptr = m_bus.getPagePtr(page);
        m_ppu.doDMA(page_ptr);
    }

    void Emulator::setVideoHeight(int height)
    {
        m_screenScale = height / float(NESVideoHeight);
        LOG(Info) << "Scale: " << m_screenScale << " set. Screen: "
                  << int(NESVideoWidth * m_screenScale) << "x" << int(NESVideoHeight * m_screenScale) << std::endl;
    }

    void Emulator::setVideoWidth(int width)
    {
        m_screenScale = width / float(NESVideoWidth);
        LOG(Info) << "Scale: " << m_screenScale << " set. Screen: "
                  << int(NESVideoWidth * m_screenScale) << "x" << int(NESVideoHeight * m_screenScale) << std::endl;

    }
    void Emulator::setVideoScale(float scale)
    {
        m_screenScale = scale;
        LOG(Info) << "Scale: " << m_screenScale << " set. Screen: "
                  << int(NESVideoWidth * m_screenScale) << "x" << int(NESVideoHeight * m_screenScale) << std::endl;
    }

    void Emulator::setKeys(std::vector<sf::Keyboard::Key>& p1, std::vector<sf::Keyboard::Key>& p2)
    {
        m_controller1.setKeyBindings(p1);
        m_controller2.setKeyBindings(p2);
    }

    void Emulator::saveState(){
        // savedState.load(m_bus, m_pictureBus, m_cpu, m_ppu);
        // savedState.toFile();
        m_cpu.toFile("state/cpu.txt");
        m_ppu.toFile("state/ppu.txt");
        m_bus.toFile("state/mbus.txt");
        m_pictureBus.toFile("state/pbus.txt");
    }

    void Emulator::loadState(int saveFile){
        // std::cout << "loaded: "  << saveFile << '\n';
        // savedState.fromFile();
        m_bus = states[saveFile].bus;
        m_pictureBus = states[saveFile].pictureBus;
        m_cpu = states[saveFile].cpu;
        m_ppu = states[saveFile].ppu;
        // m_cpu.fromFile("state/cpu.txt");
        // m_ppu.fromFile("state/ppu.txt");
        // m_bus.fromFile("state/mbus.txt");
        // m_pictureBus.fromFile("state/pbus.txt");
    }

    void Emulator::setInputCallback(std::function<void(void)> cb){
        inputCallback = cb;
    }

    void Emulator::setDrawCallback(std::function<void(void)> cb){
        drawCallback = cb;
    }

    void Emulator::reset(){
        stop = false;
        stateGetter.reset();
    }

}