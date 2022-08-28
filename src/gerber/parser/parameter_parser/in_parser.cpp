#include "in_parser.h"
#include "gerber/gerber.h"
#include "gerber/gerber_parser.h"
#include <glog/logging.h>


InParser::InParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool InParser::Run()
{
	LOG(WARNING) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Warning: Deprecated command: IN";
	gerber_parser_.start_of_level_ = false;

	if (!gerber_parser_.gerber_file_->GetString(gerber_parser_.gerber_->name_)) {
		return false;
	}

	gerber_parser_.gerber_file_->SkipWhiteSpace();
	return gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*');
}
