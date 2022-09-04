#include "fs_parser.h"
#include "gerber/gerber_parser.h"

#include <glog/logging.h>


FsParser::FsParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool FsParser::Run()
{
	gerber_parser_.start_of_level_ = false;

	gerber_parser_.gerber_file_->SkipWhiteSpace();

	while (gerber_parser_.gerber_file_->pointer_ < gerber_parser_.gerber_file_->buffer_.size() - 1) {
		switch (gerber_parser_.gerber_file_->PeekChar()) {
		case 'L':
			gerber_parser_.format_.omit_trailing_zeroes_ = false;
			break;

		case 'T':
			gerber_parser_.format_.omit_trailing_zeroes_ = true;
			break;

		case 'A':
			incremental_ = false;
			break;

		case 'I':
			incremental_ = true;
			break;

		case 'N':
		case 'G':
		case 'D':
		case 'M':
			gerber_parser_.gerber_file_->pointer_++;
			break;

		case 'X':
			gerber_parser_.gerber_file_->pointer_++;
			gerber_parser_.format_.XInteger = gerber_parser_.gerber_file_->GetChar() - '0';
			gerber_parser_.format_.XDecimal = gerber_parser_.gerber_file_->PeekChar() - '0';
			break;

		case 'Y':
			gerber_parser_.gerber_file_->pointer_++;
			gerber_parser_.format_.YInteger = gerber_parser_.gerber_file_->GetChar() - '0';
			gerber_parser_.format_.YDecimal = gerber_parser_.gerber_file_->PeekChar() - '0';
			break;

		case '*':
			gerber_parser_.gerber_file_->pointer_++;
			return true;

		default:
			LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Unrecognised FS modifier: " << gerber_parser_.gerber_file_->PeekChar();
			return false;
		}
		gerber_parser_.gerber_file_->pointer_++;
		gerber_parser_.gerber_file_->SkipWhiteSpace();
	}

	return false;
}
