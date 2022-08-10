#include "icode_parser.h"
#include "gerber_parser.h"
#include "gerber/plotter.h"


ICodeParser::ICodeParser(GerberParser& gerber) :gerber_parser_(gerber)
{
}

bool ICodeParser::Run()
{
	double d = 0;
	if (!gerber_parser_.gerber_file_->GetCoordinate(d, gerber_parser_.format_.XInteger, gerber_parser_.format_.XDecimal, gerber_parser_.format_.omit_trailing_zeroes_)) {
		return false;
	}

	gerber_parser_.PrepareEmptyLevel();
	gerber_parser_.plotter_->i_ = d;
	return true;
}

bool ICodeParser::EndOfFile()
{
	return false;
}
