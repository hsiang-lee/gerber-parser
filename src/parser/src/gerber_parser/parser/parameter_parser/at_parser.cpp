#include "at_parser.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"

#include "log/log.h"

AtParser::AtParser(GerberParserImpl& gerber, ParameterParser& param_parser)
    : gerber_parser_(gerber), param_parser_(param_parser) {}

bool AtParser::Run() {
  while (!gerber_parser_.gerber_file_->EndOfFile()) {
    if (gerber_parser_.gerber_file_->GetChar() == '*') {
      return true;
    }
  }

  return false;
}
