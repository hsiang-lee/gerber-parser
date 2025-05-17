#include "gcode_parser.h"
#include "gerber/gerber_layer.h"
#include "gerber/plotter.h"
#include "gerber_file.h"
#include "gerber_parser_impl.h"
#include "log/log.h"

GCodeParser::GCodeParser(GerberParserImpl& gerber) : gerber_parser_(gerber) {}

bool GCodeParser::Run() {
  gerber_parser_.start_of_layer_ = false;

  int code = 0;
  if (!gerber_parser_.gerber_file_->GetInteger(code)) {
    return false;
  }

  if (code != 4) {
    gerber_parser_.PrepareLayer();
  }

  switch (code) {
    case 0:  // Move
      gerber_parser_.plotter_->SetExposure(geOff);
      gerber_parser_.plotter_->SetInterpolation(giLinear);
      return true;

    case 1:  // Linear interpolation 1X scale
      gerber_parser_.plotter_->SetInterpolation(giLinear);
      return true;

    case 2:  // Clockwise circular
      gerber_parser_.plotter_->SetInterpolation(giClockwiseCircular);
      return true;

    case 3:  // Counterclockwise circular
      gerber_parser_.plotter_->SetInterpolation(giCounterclockwiseCircular);
      return true;

    case 4:  // Ignore block
      while (!gerber_parser_.gerber_file_->EndOfFile() &&
             gerber_parser_.gerber_file_->GetChar() != '*')
        ;
      gerber_parser_.gerber_file_->pointer_++;
      return !gerber_parser_.gerber_file_->EndOfFile();

    case 10:  // NOLINT
      gerber_parser_.plotter_->SetInterpolation(giLinear10X);
      return true;

    case 11:  // NOLINT
      gerber_parser_.plotter_->SetInterpolation(giLinear0_1X);
      return true;

    case 12:  // NOLINT
      gerber_parser_.plotter_->SetInterpolation(giLinear0_01X);
      return true;

    case 36:  // NOLINT
      gerber_parser_.plotter_->OutlineBegin(
          gerber_parser_.gerber_file_->line_number_);
      return true;

    case 37:  // NOLINT
      gerber_parser_.plotter_->OutlineEnd(
          gerber_parser_.gerber_file_->line_number_);
      return true;

    case 54:  // NOLINT
      Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                << " - Warning: Deprecated code: G54";
      gerber_parser_.plotter_->SetExposure(geOff);
      return true;

    case 55:  // NOLINT
      Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                << " - Warning: Deprecated code: G55";
      gerber_parser_.plotter_->SetExposure(geOff);
      return true;

    case 70:  // NOLINT
      Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                << " - Warning: Deprecated code: G70";
      gerber_parser_.gerber_->unit_ = UnitType::guInches;
      gerber_parser_.current_layer_->SetUnit(UnitType::guInches);
      return true;

    case 71:  // NOLINT
      Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                << " - Warning: Deprecated code: G71";
      gerber_parser_.gerber_->unit_ = UnitType::guMillimeters;
      gerber_parser_.current_layer_->SetUnit(UnitType::guMillimeters);
      return true;

    case 74:  // NOLINT
      gerber_parser_.plotter_->multi_quadrant_ = false;
      return true;

    case 75:  // NOLINT
      gerber_parser_.plotter_->multi_quadrant_ = true;
      return true;

    case 90:  // NOLINT
      Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                << " - Warning: Deprecated code: G90";
      return true;

    case 91:  // NOLINT
      Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                << " - Warning: Deprecated code: G91";
      return true;

    default:
      Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                << " - Error: Unknown G Code: " << code;
      break;
  }

  return false;
}

bool GCodeParser::EndOfFile() { return false; }
