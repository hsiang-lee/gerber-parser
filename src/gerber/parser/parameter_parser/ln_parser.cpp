#include "ln_parser.h"
#include "gerber/gerber_parser.h"
#include "gerber/gerber_level.h"

#include <glog/logging.h>


LnParser::LnParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool LnParser::Run()
{
	LOG(WARNING) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Warning: Deprecated command: LN";

	std::string name;
	if (!gerber_parser_.gerber_file_->GetString(name)) {
		return false;
	}

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	if (!gerber_parser_.start_of_level_) {
		gerber_parser_.AddNewLevel();
	}

	gerber_parser_.current_level_->SetName(name);
	gerber_parser_.start_of_level_ = true;

	return true;
}
