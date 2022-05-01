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
        bool inVision(int x, int y, std::vector<float>& out);
        const static int xRadius = 3;
        const static int yRadius = 2;
        const static int xRange = xRadius * 2 + 1;
        const static int yRange = yRadius * 2 + 1;
        const static int left = -3;
        const static int top = -yRadius - 1;
        const static int SIZE = xRange * yRange + 2 + 1;
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