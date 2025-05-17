#include "ic_parser.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"

#include "log/log.h"

IcParser::IcParser(GerberParserImpl& gerber, ParameterParser& param_parser)
    : gerber_parser_(gerber), param_parser_(param_parser) {}

bool IcParser::Run() {
  Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
            << " - Warning: IC Parameter ignored";

  gerber_parser_.start_of_layer_ = false;
  return gerber_parser_.gerber_file_->QueryCharUntilEnd('%');
}
