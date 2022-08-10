#include "ic_parser.h"
#include "gerber/gerber_parser.h"

#include <glog/logging.h>


IcParser::IcParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool IcParser::Run()
{
	LOG(WARNING) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Warning: IC Parameter ignored";

	gerber_parser_.start_of_level_ = false;
	return gerber_parser_.gerber_file_->QueryCharUntilEnd('%');
}
