#include "sf_parser.h"
#include "gerber/gerber_parser.h"

#include <glog/logging.h>


SfParser::SfParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool SfParser::Run()
{
	LOG(WARNING) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Warning: Deprecated command: SF";

	double d = 0.0;
	gerber_parser_.start_of_level_ = false;
	gerber_parser_.gerber_file_->SkipWhiteSpace();

	while (!gerber_parser_.gerber_file_->EndOfFile()) {
		switch (gerber_parser_.gerber_file_->GetChar()) {
		case 'A':
			if (!gerber_parser_.gerber_file_->GetFloat(d)) {
				return false;
			}
			scale_a_ = d;
			break;

		case 'B':
			if (!gerber_parser_.gerber_file_->GetFloat(d)) {
				return false;
			}
			scale_b_ = d;
			break;

		case '*':
			if (scale_b_ != 1.0 || scale_b_ != 1.0) {
				LOG(WARNING) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Warning: Scale Factor ignored";
			}
			return true;

		default:
			LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Unrecognised Scale Factor Modifier: " << gerber_parser_.gerber_file_->PeekChar();
			return false;
		}
		gerber_parser_.gerber_file_->SkipWhiteSpace();
	}

	return false;
}
