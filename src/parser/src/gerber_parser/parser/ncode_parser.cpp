#include "ncode_parser.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"

#include <iostream>

NCodeParser::NCodeParser(GerberParserImpl& gerber) : gerber_parser_(gerber) {}

bool NCodeParser::Run() {
  gerber_parser_.start_of_layer_ = false;
  std::cout << "Line " << gerber_parser_.gerber_file_->line_number_
            << " - Error: N Code not implemented" << std::endl;
  return false;
}

bool NCodeParser::EndOfFile() { return false; }
