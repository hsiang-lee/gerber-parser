#include "dcode_parser.h"
#include "gerber/gerber_level.h"
#include "gerber.h"

#include <glog/logging.h>


DCodeParser::DCodeParser(Gerber& gerber) :gerber_(gerber) {

}

bool DCodeParser::Run() {
	if (!gerber_.current_level_) {
		auto level = std::make_shared<GerberLevel>(gerber_.current_level_, gerber_.units_);
		gerber_.Add(level);
	}

	gerber_.start_of_level_ = false;

	int code;
	if (!gerber_.gerber_file_.GetInteger(code))
		return false;

	switch (code) {
	case 1: // Draw line, exposure on
		gerber_.current_level_->exposure_ = geOn;
		return true;

	case 2: // Exposure off
		gerber_.current_level_->exposure_ = geOff;
		return true;

	case 3: // Flash aperture
		gerber_.current_level_->exposure_ = geFlash;
		return true;

	default: // Select aperture
		if (code >= 1000) {
			LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Aperture code out of range: D" << code;
			return false;
		}

		auto aperture = gerber_.apertures_[code];
		if (!aperture) {
			LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Aperture not defined: D" << code;
			return false;
		}
		gerber_.current_level_->ApertureSelect(aperture, gerber_.gerber_file_.line_number_);
		return true;
	}

	return false;
}
