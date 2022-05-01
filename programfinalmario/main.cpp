#include "Emulator.h"
#include "Memoryppu.h"

int main(int argc, char** argv) {
    std::cout << "dsfw23sda32r";
    Emulator emulator;
    emulator.start();
    emulator.step();
    return 0;
}