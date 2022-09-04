#pragma once
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "parser.h"


class GerberParser;
class PParser;
class GerberMacro;
class Aperture;

class ParameterParser : public Parser {
public:
	ParameterParser(GerberParser& gerber);
	~ParameterParser();

	bool Run() override;
	bool EndOfFile() override;

private:
	void Add(const std::shared_ptr<GerberMacro>& macro);
	std::shared_ptr<GerberMacro> FindMacro(const std::string& name);

	friend class AdParser;
	friend class AmParser;

	GerberParser& gerber_parser_;
	std::vector<std::shared_ptr<GerberMacro>> macros_;
	std::map<std::string, std::shared_ptr<PParser>> parsers_;
};
