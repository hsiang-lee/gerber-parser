#pragma once
#include "pparser.h"
#include <string>


class GerberParserImpl;
class ParameterParser;

class FsParser : public PParser {
public:
	FsParser(GerberParserImpl& gerber, ParameterParser& param_parser);
	bool Run() override;

private:
	GerberParserImpl& gerber_parser_;
	ParameterParser& param_parser_;

	bool incremental_{ false };
};
