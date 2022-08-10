#include "ko_parser.h"
#include "gerber/gerber_parser.h"

#include <glog/logging.h>


KoParser::KoParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool KoParser::Run()
{
	LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Knockout not implemented";
	return false;
}
