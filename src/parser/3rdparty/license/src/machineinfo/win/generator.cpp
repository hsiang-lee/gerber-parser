#include "generator.h"

#define _WIN32_DCOM
#define GUID_LEN 64

std::string WString2String(const std::wstring &input) {
    size_t size;
    std::string cur_locale = setlocale(LC_ALL, nullptr);
    setlocale(LC_ALL, "chs");
    const wchar_t *source = input.c_str();
    size_t d_size = 2 * input.size() + 1;
    char *d_est = new char[d_size];
    memset(d_est, 0x0, d_size);
    wcstombs_s(&size, d_est, d_size, source, d_size);
    std::string result = d_est;
    delete[] d_est;
    setlocale(LC_ALL, cur_locale.c_str());
    return result;
}

std::string RemoveDash(const std::string &input) {
    std::string res = "";
    for (const auto &ch : input) {
        if (ch != '-') {
            res += ch;
        }
    }
    return res;
}

int CpuIdGenerator::GetID(std::string &input) {
    std::array<int, 4> temp_cpu_id;

    // __cpuid(int CPUInfo[4], int InfoType):
    // Generates the cpuid instruction available on x86 and x64,
    // which queries the processor for information
    // about the supported features and CPU type.
    __cpuid(temp_cpu_id.data(), 0x0);
    int nIds_ = temp_cpu_id[0];

    // save all traversal values of temp_cpu_id
    std::vector<std::array<int, 4>> data_;
    for (int i = 0; i <= nIds_; ++i) {
        __cpuidex(temp_cpu_id.data(), i, 0);
        data_.emplace_back(temp_cpu_id);
    }

    // save the cpu id
    char vendor_serialnumber[0x14] = {0};
    sprintf_s(vendor_serialnumber, sizeof(vendor_serialnumber),
              "%08X%08X", data_[1][3], data_[1][0]);
    input = std::move(vendor_serialnumber);
    return 0;
}

int MacIdGenerator::GetID(std::string &input) {
    int ret = -1;
    int alloc_type = 0;

    ULONG ul_out_buf_len = sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO p_adapter_info;
    try {
        p_adapter_info = new IP_ADAPTER_INFO;
        alloc_type = 1;
    }
    // catch error with const reference
    catch (const std::bad_alloc &) {
        return -1;
    }

    // make an initial call to GetAdaptersInfo to get
    // the necessary size into the ul_out_buf_len variable
    if (GetAdaptersInfo(p_adapter_info, &ul_out_buf_len) == ERROR_BUFFER_OVERFLOW) {
        delete p_adapter_info;
        p_adapter_info = (IP_ADAPTER_INFO *)malloc(ul_out_buf_len);
        if (p_adapter_info == nullptr) {
            return -1;
        }
        alloc_type = 2;
    }

    if (GetAdaptersInfo(p_adapter_info, &ul_out_buf_len) == NO_ERROR) {
        for (PIP_ADAPTER_INFO p_adapter = p_adapter_info;
             p_adapter != nullptr; p_adapter = p_adapter->Next) {
            // make sure that env is Ethernet
            if (p_adapter->Type != MIB_IF_TYPE_ETHERNET) {
                continue;
            }
            // Ensure the length of MAC address
            if (p_adapter->AddressLength != 6) {
                continue;
            }
            char ac_mac[32];
            // sprintf_s(ac_mac, "%02X-%02X-%02X-%02X-%02X-%02X",
            sprintf_s(ac_mac, "%02X%02X%02X%02X%02X%02X",
                      int(p_adapter->Address[0]),
                      int(p_adapter->Address[1]),
                      int(p_adapter->Address[2]),
                      int(p_adapter->Address[3]),
                      int(p_adapter->Address[4]),
                      int(p_adapter->Address[5]));
            input = ac_mac;
            ret = 0;
            break;
        }
    }

    if (alloc_type == 2) {
        free(p_adapter_info);
    } else if (alloc_type == 1) {
        delete p_adapter_info;
    }

    return ret;
}
