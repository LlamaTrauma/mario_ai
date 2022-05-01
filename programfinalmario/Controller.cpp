#include "Controller.h"

void Controller::setInputs(){
    a = key_is_pressed('Z');
    b = key_is_pressed('X');
    start = key_is_pressed('C');
    select = key_is_pressed('V');
    up = key_is_pressed(VK_UP);
    down = key_is_pressed(VK_DOWN);
    left = key_is_pressed(VK_LEFT);
    right = key_is_pressed(VK_RIGHT);
}

bool Controller::key_is_pressed(/*Linux: KeySym ks*/ char key) {
    /* Linux: Display *dpy = XOpenDisplay(":0");
    char keys_return[32];
    XQueryKeymap(dpy, keys_return);
    KeyCode kc2 = XKeysymToKeycode(dpy, ks);
    bool isPressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
    XCloseDisplay(dpy);
    //std::cout << "Reads: " << readNum << '\n';
    return isPressed; */
    if(GetKeyState(key) & 0x8000/*Check if high-order bit is set (1 << 15) apparently I guess*/){
        return true;
    }
    return false;

}

void Controller::resetRead(){
    readNum = 0;
}

byte Controller::returnButton(){
    //Because on bus and top 5 bits not driven top 5 bytes are usually 0x40/41 (from controller port address) and games like Paperboy rely on this.
    readNum += 1;
    switch(readNum){
        case 1:
            a = key_is_pressed('Z');
            return a;
            break;
        case 2:
            b = key_is_pressed('X');
            return b;
            break;
        case 3:
            select = key_is_pressed('V');
            return select;
            break;
        case 4:
            start = key_is_pressed('C');
            if(start){
                slctpressed = true;
            }
            return start;
            break;
        case 5:
            up = key_is_pressed(VK_UP);
            return up;
            break;
        case 6:
            down = key_is_pressed(VK_DOWN);
            return down;
            break;
        case 7:
            left = key_is_pressed(VK_LEFT);
            return left;
            break;
        case 8:
            right = key_is_pressed(VK_RIGHT);
            return right;
            break;
        default:
            return 1;
    }
}