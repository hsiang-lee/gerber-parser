#include "mi_parser.h"
#include "gerber/gerber_parser.h"

#include <glog/logging.h>


MiParser::MiParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool MiParser::Run()
{
	LOG(WARNING) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Warning: Deprecated command: MI. MirrorImage not implemented";
	gerber_parser_.start_of_level_ = false;

	while (!gerber_parser_.gerber_file_->EndOfFile()) {
		if (gerber_parser_.gerber_file_->GetChar() == '*') {
			return true;
		}
	}

	return false;
}
