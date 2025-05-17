#include "mcode_parser.h"
#include "gerber/plotter.h"
#include "gerber_file.h"
#include "gerber_parser_impl.h"
#include "log/log.h"

MCodeParser::MCodeParser(GerberParserImpl& gerber) : gerber_parser_(gerber) {}

bool MCodeParser::Run() {
  gerber_parser_.start_of_layer_ = false;

  int code = 0;
  if (!gerber_parser_.gerber_file_->GetInteger(code)) {
    return false;
  }

  switch (code) {
    case 0:  // Program stop
    case 1:  // Optional stop
      Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                << " - Warning: Deprecated code: M" << code;
    case 2:  // End of program
      if (gerber_parser_.current_layer_) {
        gerber_parser_.plotter_->SetExposure(geOff);
        gerber_parser_.plotter_->Do(gerber_parser_.gerber_file_->line_number_);
      }
      end_of_file_ = true;
      return true;

    default:  // Select aperture
      Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                << " - Error: Unknown M Code: " << code;
      return false;
  }
}

bool MCodeParser::EndOfFile() { return end_of_file_; }
