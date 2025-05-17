#include "star_parser.h"
#include "gerber/plotter.h"
#include "gerber_file.h"
#include "gerber_parser_impl.h"

StarParser::StarParser(GerberParserImpl& gerber) : gerber_parser_(gerber) {}

bool StarParser::Run() {
  if (gerber_parser_.current_layer_) {
    gerber_parser_.plotter_->Do(gerber_parser_.gerber_file_->line_number_);
  }

  return true;
}

bool StarParser::EndOfFile() { return false; }
