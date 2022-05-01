#ifndef STATEGETTER_H
#define STATEGETTER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "MainBus.h"
#include "Log.h"

class StateGetter {
    public:
        StateGetter(sn::MainBus*);
        std::vector<float> getState();
        void drawToScreen(sf::RenderWindow&);
        void addColumn();
        void reset();
        const static int xRadius = 4;
        const static int yRadius = 3;
        const static int xRange = xRadius * 2 + 1;
        const static int yRange = yRadius * 2 + 1;
        const static int SIZE = xRange * yRange + 2;
        std::vector<int> getXY();
    private:
        sn::MainBus* ram;
        std::list<std::vector<int>> tiles;
        std::list<std::vector<int>> state;
        std::vector<std::vector<int>> state2;
        const int COLSIZE = 13;
        const int NUMCOLS = 16;
        const int STATESIZE = COLSIZE * NUMCOLS;
        int lastBlockScroll, blockScrollOffset;
        int xVal;
        int lastScroll;
        int scrollPage;
};

#endif