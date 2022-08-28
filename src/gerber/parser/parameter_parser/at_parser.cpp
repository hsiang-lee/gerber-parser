#include "at_parser.h"
#include "gerber/gerber_parser.h"

#include <glog/logging.h>


AtParser::AtParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool AtParser::Run()
{
	while (!gerber_parser_.gerber_file_->EndOfFile()) {
		if (gerber_parser_.gerber_file_->GetChar() == '*') {
			return true;
		}
	}

	return false;
}
