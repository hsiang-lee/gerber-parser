#pragma once
#include <string>
#include <vector>
#include <memory>

class Gerber;
class GerberParserImpl;

class GerberParser
{
private:
    GerberParserImpl *impl_;

public:
    GerberParser(const std::string &file_name);
    GerberParser(const std::vector<char> &gerber_data);
    ~GerberParser();

    std::string FileName() const;
    std::shared_ptr<Gerber> GetGerber() const;
};