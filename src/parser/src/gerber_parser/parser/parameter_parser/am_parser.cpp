#include "am_parser.h"
#include "aperture/gerber_macro.h"
#include "gerber/gerber.h"
#include "gerber_file.h"
#include "gerber_parser/gerber_parser_impl.h"
#include "parser/macro_parser/gerber_macro_parser.h"
#include "parser/parameter_parser.h"

#include "log/log.h"

AmParser::AmParser(GerberParserImpl& gerber, ParameterParser& param_parser)
    : gerber_parser_(gerber), param_parser_(param_parser) {}

bool AmParser::Run() {
  gerber_parser_.start_of_layer_ = false;

  std::string name;
  if (!gerber_parser_.gerber_file_->GetString(name)) {
    return false;
  }

  if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*')) {
    return false;
  }
  gerber_parser_.gerber_file_->SkipWhiteSpace();

  auto j = gerber_parser_.gerber_file_->pointer_;
  while (!gerber_parser_.gerber_file_->EndOfFile()) {
    if (gerber_parser_.gerber_file_->PeekChar() == '%') {
      auto parser = std::make_unique<GerberMacroParser>(
          std::string(gerber_parser_.gerber_file_->buffer_.data() + j,
                      gerber_parser_.gerber_file_->pointer_ - j),
          name, (gerber_parser_.gerber_->unit_ == UnitType::guInches));
      auto macro = parser->Parse();

      if (macro) {
        param_parser_.Add(macro);
      } else {
        Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                  << " - Error: Invalid aperture macro";
      }
      return true;
    }
    gerber_parser_.gerber_file_->pointer_++;
    gerber_parser_.gerber_file_->SkipWhiteSpace();
  }

  return false;
}
