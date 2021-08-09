#include "xcode_parser.h"
#include "gerber.h"

XCodeParser::XCodeParser(Gerber& gerber) :gerber_(gerber)
{
}

bool XCodeParser::Run()
{
	double d;
	if (!gerber_.gerber_file_.GetCoordinate(d, gerber_.format_.XInteger, gerber_.format_.XDecimal, gerber_.format_.omit_trailing_zeroes_)) {
		return false;
	}

	if (!gerber_.current_level_) {
		gerber_.Add(std::make_shared<GerberLevel>(nullptr, gerber_.units_));
	}

	gerber_.current_level_->X = d;
	return true;
}

bool XCodeParser::EndOfFile()
{
	return false;
}
