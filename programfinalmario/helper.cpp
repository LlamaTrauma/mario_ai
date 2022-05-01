#ifndef NESEMU_HELPER_CPP
#define NESEMU_HELPER_CPP

#include "helper.h"
using namespace std::chrono;

#define lottanops \
(void)0;

sbyte byteToSbyte(byte in){
    sbyte out = 0xff;
    return static_cast<sbyte>(out) & static_cast<byte>(in);
}

void preciseSleep(double millis){
    double startTime = currentTimeDouble();
    double endTime = startTime + millis;
    double timeLeft = endTime - currentTimeDouble();
    while(timeLeft > 4){
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1)));
        timeLeft = endTime - currentTimeDouble();
    }
    while(timeLeft > 0){
        timeLeft = endTime - currentTimeDouble();
        //lottanops
    }
}

double currentTimeDouble(){
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    auto value = now_ms.time_since_epoch();
    long duration = value.count();
    return static_cast<double>(duration);
} 

#endif