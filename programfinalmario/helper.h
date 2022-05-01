#ifndef NESEMU_HELPER_H
#define NESEMU_HELPER_H

#include <iostream>
#include <iomanip>
#include <assert.h>
#include <fstream>
#include <stdlib.h>
#include <climits>
#include <thread>
#include <chrono>
#include <time.h>
#include <cmath>
#include <string>
#include <windows.h>

using byte = uint8_t;
using sbyte = signed char;
using word = unsigned short;
using dword = unsigned int;

sbyte byteToSbyte(byte in);
bool bitState(byte val, int bit);
void preciseSleep(double millis);
double currentTimeDouble();

#endif