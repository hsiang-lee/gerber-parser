#include "if_parser.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"

#include "log/log.h"

IfParser::IfParser(GerberParserImpl& gerber, ParameterParser& param_parser)
    : gerber_parser_(gerber), param_parser_(param_parser) {}

bool IfParser::Run() {
  Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
            << " - Error: IncludeFile not implemented";

  gerber_parser_.start_of_layer_ = false;
  return gerber_parser_.gerber_file_->QueryCharUntilEnd('%');
}
