#include "lp_parser.h"
#include "gerber/gerber_layer.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"

#include "log/log.h"

LpParser::LpParser(GerberParserImpl& gerber, ParameterParser& param_parser)
    : gerber_parser_(gerber), param_parser_(param_parser) {}

bool LpParser::Run() {
  if (!gerber_parser_.start_of_layer_) {
    gerber_parser_.AddNewLayer();
  }
  gerber_parser_.start_of_layer_ = true;

  if (gerber_parser_.gerber_file_->SkipWhiteSpace()) {
    return false;
  }

  switch (gerber_parser_.gerber_file_->GetChar()) {
    case 'C':
      gerber_parser_.current_layer_->SetNegative(true);
      break;

    case 'D':
      gerber_parser_.current_layer_->SetNegative(false);
      break;

    default:
      Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                << " - Error: Unknown layer polarity: "
                << gerber_parser_.gerber_file_->PeekChar();
      return false;
  }

  return gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*');
}
