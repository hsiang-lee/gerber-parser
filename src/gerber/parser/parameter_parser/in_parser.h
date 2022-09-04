#pragma once
#include "pparser.h"
#include <string>
#include <memory>


class GerberParser;
class ParameterParser;

class InParser : public PParser {
public:
	InParser(GerberParser& gerber, ParameterParser& param_parser);
	bool Run() override;

private:
	GerberParser& gerber_parser_;
	ParameterParser& param_parser_;
};
