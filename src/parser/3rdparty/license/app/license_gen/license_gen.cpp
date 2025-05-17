
#include <algorithm>
#include <fstream>
#include <iostream>

#include "license.h"

int main(int argc, char *argv[]) {
    lic::License license;

    std::cout << "Current license version: "
              << lic::GetVersion() << std::endl;

    std::cout << "Enter application name:" << std::endl;
    std::cin >> license.app_name;

    std::cout << "Enter secret key (64 numbers from 0-255):" << std::endl;
    std::vector<unsigned char> sk(64);
    std::for_each(sk.begin(), sk.end(),
                  [](unsigned char &c) { int a;std::cin >> a;c=a; });

    std::cout << "Enter number of machine ID's :" << std::endl;
    int machine_id_num;
    std::cin >> machine_id_num;

    for (int i = 0; i < machine_id_num; i++) {
        std::cout << "Enter machine ID: " << std::endl;
        std::string machine_id;
        std::cin >> machine_id;

        license.machine_id_list.push_back(machine_id);
    }

    auto expiration_time_start = time(nullptr);
    std::cout << "Enter expiration time (day):" << std::endl;
    int expiration_day = 1;
    std::cin >> expiration_day;
    license.expiration_time = expiration_time_start + expiration_day * 24 * 60 * 60;

    std::cout << "Enter issuer" << std::endl;
    std::cin >> license.issuer;

    std::cout << "Enter license path" << std::endl;
    std::string path;
    std::cin >> path;
    std::ofstream fout(path);

    lic::GenerateLicense(license, sk, fout);

    fout.close();
}
