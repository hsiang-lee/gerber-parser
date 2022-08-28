#include "ir_parser.h"
#include "gerber/gerber_parser.h"

#include <glog/logging.h>


IrParser::IrParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool IrParser::Run()
{
	LOG(WARNING) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Warning: Deprecated command: IR. ImageRotation not implemented";
	gerber_parser_.start_of_level_ = false;
	return gerber_parser_.gerber_file_->QueryCharUntilEnd('*');
}
