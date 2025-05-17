#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "parser.h"

class PParser;
class GerberMacro;
class GerberParserImpl;

class ParameterParser : public Parser {
public:
	ParameterParser(GerberParserImpl& gerber);
	~ParameterParser();

	bool Run() override;
	bool EndOfFile() override;

private:
	void Add(const std::shared_ptr<GerberMacro>& macro);
	std::shared_ptr<GerberMacro> FindMacro(const std::string& name);

	friend class AdParser;
	friend class AmParser;

	GerberParserImpl& gerber_parser_;
	std::vector<std::shared_ptr<GerberMacro>> macros_;
	std::map<std::string, std::shared_ptr<PParser>> parsers_;
};
