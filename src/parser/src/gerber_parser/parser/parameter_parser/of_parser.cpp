#include "of_parser.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"

#include "log/log.h"

OfParser::OfParser(GerberParserImpl& gerber, ParameterParser& param_parser)
    : gerber_parser_(gerber), param_parser_(param_parser) {}

bool OfParser::Run() {
  Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
            << " - Warning: Deprecated command: OF";

  double d = 0.0;
  gerber_parser_.start_of_layer_ = false;
  gerber_parser_.gerber_file_->SkipWhiteSpace();
  while (!gerber_parser_.gerber_file_->EndOfFile()) {
    switch (gerber_parser_.gerber_file_->GetChar()) {
      case 'A':
        if (!gerber_parser_.gerber_file_->GetCoordinate(
                d, 5, 5,
                gerber_parser_.format_.omit_trailing_zeroes_)) {  // NOLINT
          return false;
        }
        offset_a_ = d;
        break;

      case 'B':
        if (!gerber_parser_.gerber_file_->GetCoordinate(
                d, 5, 5,
                gerber_parser_.format_.omit_trailing_zeroes_)) {  // NOLINT
          return false;
        }
        offset_b_ = d;
        break;

      case '*':
        if (offset_a_ != 0.0 || offset_b_ != 0.0) {
          Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                    << " - Warning: Offsets ignored";
        }
        return true;

      default:
        Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                  << " - Error: Unrecognised Offset Modifier: "
                  << gerber_parser_.gerber_file_->PeekChar();
        return false;
    }
    gerber_parser_.gerber_file_->SkipWhiteSpace();
  }

  return false;
}
