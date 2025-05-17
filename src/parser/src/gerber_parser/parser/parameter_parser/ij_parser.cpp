#include "ij_parser.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"

#include "log/log.h"

IjParser::IjParser(GerberParserImpl& gerber, ParameterParser& param_parser)
    : gerber_parser_(gerber), param_parser_(param_parser) {}

bool IjParser::Run() {
  Log::log_ << "Line "
            << " - Warning: ImageJustify ignored"
            << gerber_parser_.gerber_file_->line_number_;
  gerber_parser_.start_of_layer_ = false;
  return gerber_parser_.gerber_file_->QueryCharUntilEnd('%');
}
