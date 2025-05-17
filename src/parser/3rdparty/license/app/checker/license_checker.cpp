#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "license.h"

int main(int argc, char *argv[]) {
    std::cout << "Current license version: "
              << lic::GetVersion() << std::endl;

    std::cout << "Enter application name:" << std::endl;
    std::string app_name;
    std::cin >> app_name;

    std::cout << "Enter public key (32 numbers from 0-255):" << std::endl;
    std::vector<unsigned char> pk(32);
    std::for_each(pk.begin(), pk.end(), [](unsigned char &c) { int a;std::cin >> a;c=a; });

    std::cout << "Enter license path" << std::endl;
    std::string path;
    std::cin >> path;

    std::ifstream license_file(path);
    if (!license_file.is_open()) {
        std::cout << "unable to open license file" << std::endl;
        return -1;
    }

    auto status = lic::CheckLicense(license_file, pk, app_name, lic::GetMachineID);
    if (status != lic::kSuccess) {
        std::cout << "invalid license, " << status << std::endl;
        return -1;
    }

    std::cout << "license passed" << std::endl;
    return 0;
}
