#include "jcode_parser.h"
#include "gerber.h"

JCodeParser::JCodeParser(Gerber& gerber) :gerber_(gerber)
{
}

bool JCodeParser::Run()
{
	double d = 0;
	if (!gerber_.gerber_file_.GetCoordinate(d, gerber_.format_.YInteger, gerber_.format_.YDecimal, gerber_.format_.omit_trailing_zeroes_)) {
		return false;
	}

	if (!gerber_.current_level_) {
		gerber_.Add(std::make_shared<GerberLevel>(nullptr, gerber_.units_));
	}

	gerber_.current_level_->J = d;
	return true;
}

bool JCodeParser::EndOfFile()
{
	return false;
}
