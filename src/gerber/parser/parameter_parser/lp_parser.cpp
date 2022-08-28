#include "lp_parser.h"
#include "gerber/gerber_parser.h"
#include "gerber/gerber_level.h"

#include <glog/logging.h>


LpParser::LpParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool LpParser::Run()
{
	if (!gerber_parser_.start_of_level_) {
		gerber_parser_.AddNewLevel();
	}
	gerber_parser_.start_of_level_ = true;

	if (gerber_parser_.gerber_file_->SkipWhiteSpace()) {
		return false;
	}

	switch (gerber_parser_.gerber_file_->GetChar()) {
	case 'C':
		gerber_parser_.current_level_->SetNegative(true);
		break;

	case 'D':
		gerber_parser_.current_level_->SetNegative(false);
		break;

	default:
		LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Unknown level polarity: " << gerber_parser_.gerber_file_->PeekChar();
		return false;
	}

	return gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*');
}
