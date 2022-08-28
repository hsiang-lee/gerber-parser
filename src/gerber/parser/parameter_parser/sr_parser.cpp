#include "sr_parser.h"
#include "gerber/gerber_parser.h"
#include "gerber/gerber_level.h"

#include <glog/logging.h>


SrParser::SrParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool SrParser::Run()
{
	if (!gerber_parser_.start_of_level_) {
		gerber_parser_.AddNewLevel();
	}
	gerber_parser_.start_of_level_ = true;

	int x = 1;
	int y = 1;
	double i = 0.0;
	double j = 0.0;

	gerber_parser_.gerber_file_->SkipWhiteSpace();
	while (!gerber_parser_.gerber_file_->EndOfFile()) {
		switch (gerber_parser_.gerber_file_->GetChar()) {
		case 'X':
			if (!gerber_parser_.gerber_file_->GetInteger(x)) {
				return false;
			}

			break;

		case 'Y':
			if (!gerber_parser_.gerber_file_->GetInteger(y)) {
				return false;
			}

			break;

		case 'I':
			if (!gerber_parser_.gerber_file_->GetFloat(i)) {
				return false;
			}

			break;

		case 'J':
			if (!gerber_parser_.gerber_file_->GetFloat(j)) {
				return false;
			}

			break;

		case '*':
			gerber_parser_.current_level_->count_x_ = x;
			gerber_parser_.current_level_->count_y_ = y;
			gerber_parser_.current_level_->step_x_ = UnitType::Get_mm(i, gerber_parser_.gerber_->unit_);
			gerber_parser_.current_level_->step_y_ = UnitType::Get_mm(j, gerber_parser_.gerber_->unit_);
			return true;

		default:
			LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Unrecognised Step-and-Repeat Modifier: " << gerber_parser_.gerber_file_->PeekChar();
			return false;
		}

		gerber_parser_.gerber_file_->SkipWhiteSpace();
	}

	return false;
}
