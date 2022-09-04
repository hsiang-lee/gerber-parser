#pragma once
#include "pparser.h"
#include <string>
#include <memory>


class GerberParser;
class ParameterParser;

class IfParser : public PParser {
public:
	IfParser(GerberParser& gerber, ParameterParser& param_parser);
	bool Run() override;

private:
	GerberParser& gerber_parser_;
	ParameterParser& param_parser_;
};
