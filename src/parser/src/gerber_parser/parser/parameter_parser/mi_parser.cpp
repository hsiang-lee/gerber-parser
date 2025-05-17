#include "mi_parser.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"

#include "log/log.h"

MiParser::MiParser(GerberParserImpl& gerber, ParameterParser& param_parser)
    : gerber_parser_(gerber), param_parser_(param_parser) {}

bool MiParser::Run() {
  Log::log_
      << "Line " << gerber_parser_.gerber_file_->line_number_
      << " - Warning: Deprecated command: MI. MirrorImage not implemented";
  gerber_parser_.start_of_layer_ = false;

  while (!gerber_parser_.gerber_file_->EndOfFile()) {
    if (gerber_parser_.gerber_file_->GetChar() == '*') {
      return true;
    }
  }

  return false;
}
