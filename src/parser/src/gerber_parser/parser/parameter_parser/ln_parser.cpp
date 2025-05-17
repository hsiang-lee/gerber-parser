#include "ln_parser.h"
#include "gerber/gerber_layer.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"

#include "log/log.h"

LnParser::LnParser(GerberParserImpl& gerber, ParameterParser& param_parser)
    : gerber_parser_(gerber), param_parser_(param_parser) {}

bool LnParser::Run() {
  Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
            << " - Warning: Deprecated command: LN";

  std::string name;
  if (!gerber_parser_.gerber_file_->GetString(name)) {
    return false;
  }

  if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*')) {
    return false;
  }

  if (!gerber_parser_.start_of_layer_) {
    gerber_parser_.AddNewLayer();
  }

  gerber_parser_.current_layer_->SetName(name);
  gerber_parser_.start_of_layer_ = true;

  return true;
}
