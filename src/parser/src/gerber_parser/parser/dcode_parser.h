#pragma once
#include "parser.h"

class GerberParserImpl;

class DCodeParser : public Parser {
 public:
  DCodeParser(GerberParserImpl& gerber);

  bool Run() override;
  bool EndOfFile() override;

 private:
  GerberParserImpl& gerber_parser_;
};