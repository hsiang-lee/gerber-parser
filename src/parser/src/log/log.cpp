#include "log.h"
#include <iostream>

Log Log::log_;
bool Log::enable_ = true;

Log& Log::operator << (const std::string& str) {
    if (enable_) {
        std::cout << str;
    }
    
    return *this;
}

Log& Log::operator << (int num) {
    if (enable_) {
        std::cout << num;
    }
    
    return *this;
}

void Log::turnOn() {
    enable_ = true;
}

void Log::turnOff() {
    enable_ = false;
}
