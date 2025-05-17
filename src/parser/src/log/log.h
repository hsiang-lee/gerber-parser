#pragma once
#include <string>

class Log {
public:
    Log& operator << (const std::string& str);
    Log& operator << (int num);
    
    static void turnOn();
    static void turnOff();
    
    static Log log_;
    
private:
    static bool enable_;
};
