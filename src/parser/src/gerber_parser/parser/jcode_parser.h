#pragma once
#include "parser.h"

class GerberParserImpl;

class JCodeParser : public Parser {
 public:
  JCodeParser(GerberParserImpl& gerber);

  bool Run() override;
  bool EndOfFile() override;

 private:
  GerberParserImpl& gerber_parser_;
};