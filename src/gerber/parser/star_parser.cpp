#include "star_parser.h"
#include "gerber.h"

StarParser::StarParser(Gerber& gerber) :gerber_(gerber)
{
}

bool StarParser::Run()
{
	if (gerber_.current_level_) {
		gerber_.current_level_->Do(gerber_.gerber_file_.line_number_);
	}

	return true;
}

bool StarParser::EndOfFile()
{
	return false;
}
