#pragma once
#include "pparser.h"
#include <string>
#include <memory>


class GerberParser;
class ParameterParser;

class FsParser : public PParser {
public:
	FsParser(GerberParser& gerber, ParameterParser& param_parser);
	bool Run() override;

private:
	GerberParser& gerber_parser_;
	ParameterParser& param_parser_;

	bool incremental_{ false };
};
