#include "ip_parser.h"
#include "gerber/gerber.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"

#include "log/log.h"

IpParser::IpParser(GerberParserImpl& gerber, ParameterParser& param_parser)
    : gerber_parser_(gerber), param_parser_(param_parser) {}

bool IpParser::Run() {
  Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
            << " - Warning: Deprecated command: IP";
  gerber_parser_.start_of_layer_ = false;

  gerber_parser_.gerber_file_->SkipWhiteSpace();
  if (gerber_parser_.gerber_file_->pointer_ >=
      gerber_parser_.gerber_file_->buffer_.size() - 2) {
    return false;
  }

  if (gerber_parser_.gerber_file_->PeekChar() == 'P' &&
      gerber_parser_.gerber_file_->PeekNextChar() == 'O' &&
      gerber_parser_.gerber_file_
              ->buffer_[gerber_parser_.gerber_file_->pointer_ + 2] == 'S') {
    gerber_parser_.gerber_->negative_ = false;
  } else if (gerber_parser_.gerber_file_->PeekChar() == 'N' &&
             gerber_parser_.gerber_file_->PeekNextChar() == 'E' &&
             gerber_parser_.gerber_file_
                     ->buffer_[gerber_parser_.gerber_file_->pointer_ + 2] ==
                 'G') {
    gerber_parser_.gerber_->negative_ = true;
  } else {
    Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
              << " - Error: Unknown Image Polarity";
    return false;
  }
  gerber_parser_.gerber_file_->pointer_ += 3;

  return gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*');
}
