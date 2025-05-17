#include "permission/permission.h"

Permission::Permission() {
    
}

Permission::Permission(const std::string& license) {
}

Permission::~Permission() {
    
}

bool Permission::accept() {
    return true;
}
