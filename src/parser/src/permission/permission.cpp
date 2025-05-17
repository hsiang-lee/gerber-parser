#include "permission.h"
#include "license.h"

#include <cstdlib>
#include <fstream>

#ifdef WIN32
Permission::Permission()
    : Permission(std::string(std::getenv("USERPROFILE")) +
                 "\\gerber_parser\\license.txt") {}
#else
Permission::Permission()
    : Permission(std::string(std::getenv("HOME")) +
                 "/gerber_parser/license.txt") {}
#endif

Permission::Permission(const std::string& license) {
  std::ifstream license_file(license);
  if (!license_file.is_open()) {
    accepted_ = false;
    return;
  }

  std::vector<unsigned char> pk{
      50, 92, 45, 58, 57, 8, 160, 74, 78, 79, 93, 17, 243, 3, 135, 10, 147, 70, 161, 233, 167, 209, 236, 190, 206, 86, 161, 73, 152, 153, 89, 81};
  const auto status =
      lic::CheckLicense(license_file, pk, "gerber_parser", lic::GetMachineID);
  accepted_ = (status == lic::kSuccess);
}

Permission::~Permission() {}

bool Permission::accept() {
  return accepted_;
}
