#include "xcode_parser.h"
#include "gerber/plotter.h"
#include "gerber_file.h"
#include "gerber_parser_impl.h"

XCodeParser::XCodeParser(GerberParserImpl& gerber) : gerber_parser_(gerber) {}

bool XCodeParser::Run() {
  double d = 0.0;
  if (!gerber_parser_.gerber_file_->GetCoordinate(
          d, gerber_parser_.format_.XInteger, gerber_parser_.format_.XDecimal,
          gerber_parser_.format_.omit_trailing_zeroes_)) {
    return false;
  }

  gerber_parser_.PrepareEmptyLayer();
  gerber_parser_.plotter_->x_ = d;
  return true;
}

bool XCodeParser::EndOfFile() { return false; }
