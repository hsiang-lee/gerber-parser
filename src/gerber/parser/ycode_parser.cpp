#include "ycode_parser.h"
#include "gerber.h"


YCodeParser::YCodeParser(Gerber& gerber) : gerber_(gerber)
{
}

bool YCodeParser::Run()
{
	double d;
	if (!gerber_.gerber_file_.GetCoordinate(d, gerber_.format_.YInteger, gerber_.format_.YDecimal, gerber_.format_.omit_trailing_zeroes_)) {
		return false;
	}

	if (!gerber_.current_level_) {
		gerber_.Add(std::make_shared<GerberLevel>(nullptr, gerber_.units_));
	}

	gerber_.current_level_->Y = d;
	return true;
}

bool YCodeParser::EndOfFile()
{
	return false;
}
