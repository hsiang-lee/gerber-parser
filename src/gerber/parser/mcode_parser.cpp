#include "mcode_parser.h"
#include "gerber.h"
#include <glog/logging.h>


MCodeParser::MCodeParser(Gerber& gerber) : gerber_(gerber)
{
}

bool MCodeParser::Run()
{
	gerber_.start_of_level_ = false;

	int code = 0;
	if (!gerber_.gerber_file_.GetInteger(code))
		return false;

	switch (code) {
	case 0: // Program stop
	case 1: // Optional stop
		if (gerber_warnings) {
			LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated code: M" << code;
		}
	case 2: // End of program
		if (gerber_.current_level_) {
			gerber_.current_level_->exposure_ = geOff;
			gerber_.current_level_->Do(gerber_.gerber_file_.line_number_);
		}
		end_of_file_ = true;
		return true;

	default: // Select aperture
		LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Unknown M Code: " << code;
		return false;
	}
}

bool MCodeParser::EndOfFile()
{
	return end_of_file_;
}
