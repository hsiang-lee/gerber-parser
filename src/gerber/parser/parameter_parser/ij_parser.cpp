#include "ij_parser.h"
#include "gerber/gerber_parser.h"

#include <glog/logging.h>


IjParser::IjParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool IjParser::Run()
{
	LOG(WARNING) << "Line " << " - Warning: ImageJustify ignored" << gerber_parser_.gerber_file_->line_number_;
	gerber_parser_.start_of_level_ = false;
	return gerber_parser_.gerber_file_->QueryCharUntilEnd('%');
}
