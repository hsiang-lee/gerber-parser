#include "generator.h"

#define MAXINTERFACES 16

int CpuIdGenerator::GetID(std::string &input) {
    input = std::to_string(gethostid());
    return 0;
}

int MacIdGenerator::GetID(std::string &input) {
    std::vector<std::string> vnet_types{"eth", "em", "oct", "eno", "ib"};
    for (const auto &vnet_type : vnet_types) {
        for (char temp_ch = '0'; temp_ch <= '9'; temp_ch++) {
            std::string str_device = vnet_type + temp_ch;
            int fd;
            struct ifreq ifr;
            fd = socket(AF_INET, SOCK_DGRAM, 0);
            strcpy(ifr.ifr_name, str_device.c_str());
            if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) {
                close(fd);
                continue;
            }
            unsigned char macaddr[6];
            char temp_str[80];
            memcpy(macaddr, ifr.ifr_hwaddr.sa_data, 6);
            sprintf(temp_str, "%02X%02X%02X%02X%02X%02X",
                    macaddr[0], macaddr[1], macaddr[2],
                    macaddr[3], macaddr[4], macaddr[5]);
            input = temp_str;
            return 0;
        }
    }
    return -1;
}

