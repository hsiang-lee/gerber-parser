#pragma once
#include "parser.h"

class GerberParserImpl;

class ICodeParser : public Parser {
 public:
  ICodeParser(GerberParserImpl& gerber);

  bool Run() override;
  bool EndOfFile() override;

 private:
  GerberParserImpl& gerber_parser_;
};