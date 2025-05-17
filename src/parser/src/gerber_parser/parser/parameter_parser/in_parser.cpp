#include "in_parser.h"
#include "gerber/gerber.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"
#include "log/log.h"

InParser::InParser(GerberParserImpl& gerber, ParameterParser& param_parser)
    : gerber_parser_(gerber), param_parser_(param_parser) {}

bool InParser::Run() {
  Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
            << " - Warning: Deprecated command: IN";
  gerber_parser_.start_of_layer_ = false;

  std::string name;
  if (!gerber_parser_.gerber_file_->GetString(name)) {
    return false;
  }

  gerber_parser_.gerber_->SetName(name);
  gerber_parser_.gerber_file_->SkipWhiteSpace();
  return gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*');
}
