#include "license.h"
#include <algorithm>
#include <iostream>

int main() {
    std::string machine_id = lic::GetMachineID();

    if (machine_id.empty()) {
        std::cout << "Failed getting machine ID" << std::endl;
        return -1;
    }

    std::cout << machine_id << std::endl;
    return 0;
}
