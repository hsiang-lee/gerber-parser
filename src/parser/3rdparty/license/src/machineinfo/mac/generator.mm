#include "generator.h"
#include <sstream>

#import <Foundation/NSString.h>
#import <IOKit/IOKitLib.h>

std::string get_platform_uuid()
{
    io_registry_entry_t ioRegistryRoot = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/");
    CFStringRef uuidCf = (CFStringRef) IORegistryEntryCreateCFProperty(ioRegistryRoot, CFSTR(kIOPlatformUUIDKey), kCFAllocatorDefault, 0);
    IOObjectRelease(ioRegistryRoot);
    
    constexpr int buf_size = 128;
    char buf[buf_size] = "";
    CFStringGetCString(uuidCf, buf, buf_size, kCFStringEncodingMacRoman);
    CFRelease(uuidCf);
    
    return std::string(buf);
}

inline void native_cpuid(unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx)
{
    // ecx is often an input as well as an output
    asm volatile("cpuid"
        : "=a" (*eax),
          "=b" (*ebx),
          "=c" (*ecx),
          "=d" (*edx)
        : "0" (*eax), "2" (*ecx));
}

std::string get_cpuid()
{
    // Capture vendor string
    char vendor[0x20];
    memset(vendor, 0, sizeof(vendor));

    unsigned eax, ebx, ecx, edx;
 
    eax = 0; // processor info and feature bits
    native_cpuid(&eax, &ebx, &ecx, &edx);

    *reinterpret_cast<int*>(vendor) = ebx;
    *reinterpret_cast<int*>(vendor + 4) = edx;
    *reinterpret_cast<int*>(vendor + 8) = ecx;

    eax = 1; // processor serial number
    native_cpuid(&eax, &ebx, &ecx, &edx);
    // see the CPUID Wikipedia article on which models return the serial number in which registers
    
    std::ostringstream ostr;
    ostr << vendor << std::hex <<std::uppercase << std::setw(8) <<  std::setfill('0') << std::right << edx << std::setw(8) << eax << std::flush;
 
    return ostr.str();
}


int CpuIdGenerator::GetID(std::string &input) {
    input = get_cpuid();
    return 0;
}

int MacIdGenerator::GetID(std::string &input) {
    input = get_platform_uuid();
    return 0;
}
