#include "am_parser.h"
#include "gerber/gerber.h"
#include "gerber/gerber_parser.h"
#include "parser/parameter_parser.h"
#include "gerber/gerber_macro.h"

#include <glog/logging.h>


AmParser::AmParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool AmParser::Run()
{
	gerber_parser_.start_of_level_ = false;

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
			auto macro = std::make_shared<GerberMacro>(name, std::string(gerber_parser_.gerber_file_->buffer_.data() + j, gerber_parser_.gerber_file_->pointer_ - j), (gerber_parser_.gerber_->unit_ == UnitType::guInches));

			if (macro->Valid()) {
				param_parser_.Add(macro);
			}
			else {
				LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Invalid aperture macro";
			}
			return true;
		}
		gerber_parser_.gerber_file_->pointer_++;
		gerber_parser_.gerber_file_->SkipWhiteSpace();
	}

	return false;
}
