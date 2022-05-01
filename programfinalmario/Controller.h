#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "helper.h"
//#include <X11/Xlib.h>
#include <iostream>
//#include "X11/keysym.h"

class Controller {
public:
    byte a = 0;
    byte b = 0;
    byte start = 0;
    byte select = 0;
    byte up = 0;
    byte down = 0;
    byte left = 0;
    byte right = 0;
    bool slctpressed = false;
    bool zpressed;
    char pressed[256];
    int readNum = 0;
    void setInputs();
    void resetRead();
    byte returnButton();
    //Linux: bool key_is_pressed(KeySym ks);
    bool key_is_pressed(char key);
    void getKeyPresses();
};

#endif