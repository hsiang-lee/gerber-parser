#include "icode_parser.h"
#include "gerber.h"

ICodeParser::ICodeParser(Gerber& gerber) :gerber_(gerber)
{
}

bool ICodeParser::Run()
{
	double d = 0;
	if (!gerber_.gerber_file_.GetCoordinate(d, gerber_.format_.XInteger, gerber_.format_.XDecimal, gerber_.format_.omit_trailing_zeroes_)) {
		return false;
	}

	if (!gerber_.current_level_) {
		gerber_.Add(std::make_shared<GerberLevel>(nullptr, gerber_.units_));
	}

	gerber_.current_level_->I = d;
	return true;
}

bool ICodeParser::EndOfFile()
{
	return false;
}
