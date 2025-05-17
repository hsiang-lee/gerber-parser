#pragma once
#include "parser.h"

class GerberParserImpl;

class GCodeParser : public Parser {
 public:
  GCodeParser(GerberParserImpl& gerber);

  bool Run() override;
  bool EndOfFile() override;

 private:
  GerberParserImpl& gerber_parser_;
};