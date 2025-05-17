#include <string>
#include <vector>

#include <sodium.h>
#include <nlohmann/json.hpp>

#include "base64/base64lib.h"
#include "generator.h"
#include "md5/md5.h"
#include "license.h"

namespace lic {
// NOLINTNEXTLINE
void to_json(nlohmann::json &j, const lic::License &lic) {
  j = nlohmann::json{
      {"AppName", lic.app_name}, {"MachineIDs", lic.machine_id_list}, {"ExpirationTime", lic.expiration_time},
      {"Issuer", lic.issuer},    {"Payloads", lic.payloads},
  };
}

// NOLINTNEXTLINE
void from_json(const nlohmann::json &j, lic::License &lic) {
  j.at("AppName").get_to(lic.app_name);
  j.at("MachineIDs").get_to(lic.machine_id_list);
  j.at("ExpirationTime").get_to(lic.expiration_time);
  j.at("Issuer").get_to(lic.issuer);
  j.at("Payloads").get_to(lic.payloads);
}
}  // namespace lic

static std::vector<char> EncodeBase64(const std::vector<unsigned char> &input) {
  auto rst = Base64Encode(std::string(input.begin(), input.end()));
  return std::vector<char>(rst.begin(), rst.end());
}

static std::vector<unsigned char> DecodeBase64(const std::vector<char> &input) {
  auto rst = Base64Decode(std::string(input.begin(), input.end()));
  return std::vector<unsigned char>(rst.begin(), rst.end());
}

std::string lic::GetVersion() { return LICENSE_VERSION; }

std::string lic::GetMachineID() {
  MacIdGenerator mac_id_gen;
  CpuIdGenerator cpu_id_gen;
  std::string cpu_id;
  std::string mac_id;
  mac_id_gen.GetID(mac_id);
  cpu_id_gen.GetID(cpu_id);

  return cpu_id + mac_id;
}

void lic::GenerateKeyPair(std::vector<unsigned char> &pk, std::vector<unsigned char> &sk) {
  std::vector<unsigned char>(crypto_sign_PUBLICKEYBYTES).swap(pk);
  std::vector<unsigned char>(crypto_sign_SECRETKEYBYTES).swap(sk);
  crypto_sign_keypair(pk.data(), sk.data());
  return;
}

void lic::GenerateLicense(const License &lic, const std::vector<unsigned char> &sk, std::ostream &stream) {
  nlohmann::json lic_json = lic;
  std::string lic_serialized = lic_json.dump();

  std::vector<unsigned char> lic_signed(crypto_sign_BYTES + lic_serialized.size());
  unsigned long long signed_message_len;  // NOLINT

  crypto_sign(lic_signed.data(), &signed_message_len, reinterpret_cast<const unsigned char *>(lic_serialized.data()), lic_serialized.size(),
              sk.data());

  auto lic_base64 = EncodeBase64(lic_signed);

  stream.write(lic_base64.data(), lic_base64.size());
}

lic::Status lic::CheckLicense(std::istream &is, const std::vector<unsigned char> &pk, const std::string &app_name,
                              std::function<std::string()> machine_id_func) {
  Parser lp(is);

  auto sig = lp.Signature();
  auto mes = lp.Message();

  if (crypto_sign_verify_detached(sig.data(), mes.data(), mes.size(), pk.data()) != 0) {
    return kInvalidLicense;
  }

  // validate the license
  if (app_name != lp.AppName()) {
    return kInvalidAppName;
  }

  // TODO(huijie): check expiration time
  auto time_now = time(nullptr);
  if (time_now > lp.ExpirationTime()) {
    return kLicenseExpired;
  }

  for (const auto &id : lp.MachineIDs()) {
    if (id == machine_id_func()) {
      return kSuccess;
    }
  }

  return kInvalidMachineId;
}

lic::Parser::Parser(std::istream &is) {
  std::vector<char> license_base64(std::istream_iterator<char>(is), {});
  std::vector<unsigned char> license_data = DecodeBase64(license_base64);

  auto sig_end = license_data.begin() + crypto_sign_BYTES;

  signature_ = std::vector<unsigned char>(license_data.begin(), sig_end);

  message_ = std::vector<unsigned char>(sig_end, license_data.end());

  // deserialize license
  std::string license_serialized(sig_end, license_data.end());
  nlohmann::json license_json = nlohmann::json::parse(license_serialized);
  license_ = license_json.get<License>();
}

std::vector<unsigned char> lic::Parser::Signature() const { return signature_; }

std::vector<unsigned char> lic::Parser::Message() const { return message_; }

std::string lic::Parser::AppName() const { return license_.app_name; }

std::string lic::Parser::Issuer() const { return license_.issuer; }

time_t lic::Parser::ExpirationTime() const { return license_.expiration_time; }

std::vector<std::string> lic::Parser::MachineIDs() const { return license_.machine_id_list; }

std::string lic::Parser::Payload(const std::string &key) { return license_.payloads[key]; }
