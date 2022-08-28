#include "star_parser.h"
#include "gerber_parser.h"
#include "gerber/plotter.h"


StarParser::StarParser(GerberParser& gerber) :gerber_parser_(gerber)
{
}

bool StarParser::Run()
{
	if (gerber_parser_.current_level_) {
		gerber_parser_.plotter_->Do(gerber_parser_.gerber_file_->line_number_);
	}

	return true;
}

bool StarParser::EndOfFile()
{
	return false;
}
