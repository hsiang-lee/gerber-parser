#pragma once
#include <string>

class Permission {
public:
    Permission();
    explicit Permission(const std::string& license);
    ~Permission();
    
    bool accept();
    
private:
    bool accepted_{false};
};
