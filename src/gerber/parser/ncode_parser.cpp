#include "ncode_parser.h"
#include "gerber.h"



NCodeParser::NCodeParser(Gerber& gerber) :gerber_(gerber) {

}

bool NCodeParser::Run() {
	gerber_.start_of_level_ = false;
	printf("Line %d - Error: N Code not implemented\n", gerber_.gerber_file_.line_number_);
	return false;
}
