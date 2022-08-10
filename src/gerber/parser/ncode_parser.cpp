#include "ncode_parser.h"
#include "gerber/gerber_parser.h"

#include <iostream>


NCodeParser::NCodeParser(GerberParser& gerber) :gerber_parser_(gerber) {

}

bool NCodeParser::Run() {
	gerber_parser_.start_of_level_ = false;
	std::cout << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: N Code not implemented" << std::endl;
	return false;
}

bool NCodeParser::EndOfFile()
{
	return false;
}
