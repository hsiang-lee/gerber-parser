#ifndef LICENSE_H
#define LICENSE_H

#include <map>
#include <memory>
#include <string>
#include <ostream>
#include <vector>
#include <functional>

namespace lic {
enum Status {
    kSuccess,
    kInvalidLicensePath,
    kInvalidLicense,
    kInvalidAppName,
    kInvalidMachineId,
    kLicenseExpired,
};

struct License {
    std::string app_name;
    std::string issuer;

    std::vector<std::string> machine_id_list;
    time_t expiration_time = 0;

    std::map<std::string, std::string> payloads;
};

std::string GetVersion();

// GetMachineID returns a unique machine identifier using its hardware
// information
std::string GetMachineID();

// Generate crypto_sign_key_pair
void GenerateKeyPair(std::vector<unsigned char> &pk,
                     std::vector<unsigned char> &sk);

// GenerateLicense generates license using secret key
void GenerateLicense(const License &info,
                     const std::vector<unsigned char> &sk,
                     std::ostream &stream);

// CheckLicense checks whether the license is valid
Status CheckLicense(std::istream &is,
                    const std::vector<unsigned char> &pk,
                    const std::string &app_name,
                    std::function<std::string()> machine_id_func);

class Parser {
   public:
    explicit Parser(std::istream &is);

    std::vector<unsigned char> Signature() const;

    std::vector<unsigned char> Message() const;

    std::string AppName() const;

    std::string Issuer() const;

    time_t ExpirationTime() const;

    std::vector<std::string> MachineIDs() const;

    std::string Payload(const std::string &key);

   private:
    std::vector<unsigned char> signature_;
    std::vector<unsigned char> message_;
    License license_;
};
}  // namespace lic

#endif
