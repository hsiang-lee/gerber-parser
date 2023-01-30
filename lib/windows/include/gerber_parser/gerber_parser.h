#pragma once
#include <memory>
#include <string>
#include <vector>

class Gerber;
class GerberParserImpl;

class GerberApi GerberParser {
 private:
  std::unique_ptr<GerberParserImpl> impl_;

 public:
  GerberParser(const std::string& file_name);
  GerberParser(const std::vector<char>& gerber_data);
  ~GerberParser();

  std::string FileName() const;
  std::shared_ptr<Gerber> GetGerber() const;
};
