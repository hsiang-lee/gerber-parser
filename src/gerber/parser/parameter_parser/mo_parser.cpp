#include "mo_parser.h"
#include "gerber/gerber_parser.h"
#include "gerber/gerber_level.h"

#include <glog/logging.h>


MoParser::MoParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool MoParser::Run()
{
	gerber_parser_.start_of_level_ = false;
	gerber_parser_.gerber_file_->SkipWhiteSpace();

	while (gerber_parser_.gerber_file_->pointer_ < gerber_parser_.gerber_file_->buffer_.size() - 1) {
		if (gerber_parser_.gerber_file_->PeekChar() == 'I' && gerber_parser_.gerber_file_->PeekNextChar() == 'N') {
			gerber_parser_.gerber_->unit_ = UnitType::guInches;
			gerber_parser_.gerber_file_->pointer_ += 2;
		}
		else if (gerber_parser_.gerber_file_->PeekChar() == 'M' && gerber_parser_.gerber_file_->PeekNextChar() == 'M') {
			gerber_parser_.gerber_->unit_ = UnitType::guMillimeters;
			gerber_parser_.gerber_file_->pointer_ += 2;
		}
		else if (gerber_parser_.gerber_file_->PeekChar() == '*') {
			if (gerber_parser_.current_level_) {
				gerber_parser_.current_level_->SetUnit(gerber_parser_.gerber_->unit_);
			}

			gerber_parser_.gerber_file_->pointer_++;
			return true;
		}
		else {
			LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Unrecognised MO modifier: " << gerber_parser_.gerber_file_->PeekChar();
			return false;
		}
		gerber_parser_.gerber_file_->SkipWhiteSpace();
	}

	return false;
}
