#include "if_parser.h"
#include "gerber/gerber_parser.h"

#include <glog/logging.h>


IfParser::IfParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool IfParser::Run()
{
	LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: IncludeFile not implemented";

	gerber_parser_.start_of_level_ = false;
	return gerber_parser_.gerber_file_->QueryCharUntilEnd('%');
}
