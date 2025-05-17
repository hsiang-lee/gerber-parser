#include "ycode_parser.h"
#include "gerber/plotter.h"
#include "gerber_file.h"
#include "gerber_parser_impl.h"

YCodeParser::YCodeParser(GerberParserImpl& gerber) : gerber_parser_(gerber) {}

bool YCodeParser::Run() {
  double d = 0.0;
  if (!gerber_parser_.gerber_file_->GetCoordinate(
          d, gerber_parser_.format_.YInteger, gerber_parser_.format_.YDecimal,
          gerber_parser_.format_.omit_trailing_zeroes_)) {
    return false;
  }

  gerber_parser_.PrepareEmptyLayer();
  gerber_parser_.plotter_->y_ = d;
  return true;
}

bool YCodeParser::EndOfFile() { return false; }
