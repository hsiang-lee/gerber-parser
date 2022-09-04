#include "dcode_parser.h"
#include "gerber_parser.h"
#include "gerber/plotter.h"

#include <glog/logging.h>


DCodeParser::DCodeParser(GerberParser& gerber) :gerber_parser_(gerber) {

}

bool DCodeParser::Run() {
	gerber_parser_.PrepareLevel();
	gerber_parser_.start_of_level_ = false;

	int code = 0;
	if (!gerber_parser_.gerber_file_->GetInteger(code)) {
		return false;
	}

	switch (code) {
	case 1: // Draw line, exposure on
		gerber_parser_.plotter_->SetExposure(geOn);
		return true;

	case 2: // Exposure off
		gerber_parser_.plotter_->SetExposure(geOff);
		return true;

	case 3: // Flash aperture
		gerber_parser_.plotter_->SetExposure(geFlash);
		return true;

	default: // Select aperture
		if (code >= 1000) {// NOLINT
			LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Aperture code out of range: D" << code;
			return false;
		}

		auto aperture = gerber_parser_.GetAperture(code);
		if (!aperture) {
			LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Aperture not defined: D" << code;
			return false;
		}
		gerber_parser_.plotter_->ApertureSelect(aperture, gerber_parser_.gerber_file_->line_number_);
		return true;
	}

	return false;
}

bool DCodeParser::EndOfFile()
{
	return false;
}
