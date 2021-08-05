#include "gcode_parser.h"
#include "gerber/gerber_level.h"
#include "gerber.h"

#include <glog/logging.h>


GCodeParser::GCodeParser(Gerber& gerber) :gerber_(gerber) {
}

bool GCodeParser::Run() {
	std::shared_ptr<GerberLevel> level;

	gerber_.start_of_level_ = false;

	int code;
	if (!gerber_.gerber_file_.GetInteger(code)) {
		return false;
	}

	if (!gerber_.current_level_ && code != 4) {
		level = std::make_shared<GerberLevel>(gerber_.current_level_, gerber_.units_);
		gerber_.Add(level);
	}

	switch (code) {
	case 0: // Move
		gerber_.current_level_->exposure_ = geOff;
		gerber_.current_level_->interpolation_ = giLinear;
		return true;

	case 1: // Linear interpolation 1X scale
		gerber_.current_level_->interpolation_ = giLinear;
		return true;

	case 2: // Clockwise circular
		gerber_.current_level_->interpolation_ = giClockwiseCircular;
		return true;

	case 3: // Counterclockwise circular
		gerber_.current_level_->interpolation_ = giCounterclockwiseCircular;
		return true;

	case 4: // Ignore block
		while (!gerber_.gerber_file_.EndOfFile() && gerber_.gerber_file_.GetChar() != '*');
		gerber_.gerber_file_.index_++;
		return !gerber_.gerber_file_.EndOfFile();

	case 10: // Linear interpolation 10X scale
		gerber_.current_level_->interpolation_ = giLinear10X;
		return true;

	case 11: // Linear interpolation 0.1X scale
		gerber_.current_level_->interpolation_ = giLinear0_1X;
		return true;

	case 12: // Linear interpolation 0.01X scale
		gerber_.current_level_->interpolation_ = giLinear0_01X;
		return true;

	case 36: // Turn on Outline Area Fill
		gerber_.current_level_->OutlineBegin(gerber_.gerber_file_.line_number_);
		return true;

	case 37: // Turn off Outline Area Fill
		gerber_.current_level_->OutlineEnd(gerber_.gerber_file_.line_number_);
		return true;

	case 54: // Tool prepare
		if (gerber_warnings) {
			LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated code: G54";
		}
		gerber_.current_level_->exposure_ = geOff;
		return true;

	case 55: // Flash prepare
		if (gerber_warnings) {
			LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated code: G55";
		}
		gerber_.current_level_->exposure_ = geOff;
		return true;

	case 70: // Specify inches
		if (gerber_warnings) {
			LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated code: G70";
		}
		gerber_.units_ = guInches;
		gerber_.current_level_->units_ = guInches;
		return true;

	case 71: // Specify millimeters
		if (gerber_warnings) {
			LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated code: G71";
		}
		gerber_.units_ = guMillimeters;
		gerber_.current_level_->units_ = guMillimeters;
		return true;

	case 74: // Disable 360 deg circular interpolation
		gerber_.current_level_->multi_quadrant_ = false;
		return true;

	case 75: // Enable 360 deg circular interpolation
		gerber_.current_level_->multi_quadrant_ = true;
		return true;

	case 90: // Specify absolute format
		if (gerber_warnings) {
			LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated code: G90";
		}
		gerber_.current_level_->incremental_ = false;
		return true;

	case 91: // Specify incremental format
		if (gerber_warnings) {
			LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated code: G91";
		}

		return true;

	default:
		LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Unknown G Code: " << code;
		break;
	}

	return false;
}
