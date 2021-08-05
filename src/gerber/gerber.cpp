#include "gerber.h"
#include "parser/gcode_parser.h"
#include "parser/dcode_parser.h"
#include "parser/ncode_parser.h"
#include "parser/parameter_parser.h"
#include <glog/logging.h>

#include <algorithm>


bool gerber_warnings = true;

Gerber::Gerber(const const std::string& file_name) {
	gerber_file_.buffer_.clear();
	gerber_file_.index_ = 0;

	units_ = guInches;

	format_.omit_trailing_zeroes_ = false;
	format_.XInteger = 6;
	format_.XDecimal = 6;
	format_.YInteger = 6;
	format_.YDecimal = 6;

	current_level_ = 0;
	apertures_.resize(1000);

	parsers_['D'] = std::make_shared<DCodeParser>(*this);
	parsers_['G'] = std::make_shared<GCodeParser>(*this);
	parsers_['N'] = std::make_shared<NCodeParser>(*this);
	parsers_['0'] = std::make_shared<ParameterParser>(*this);

	LoadGerber(file_name);
}


Gerber::~Gerber() {
}

bool Gerber::MCode(bool& end_of_file) {

	start_of_level_ = false;

	int code = 0;
	if (!gerber_file_.GetInteger(code))
		return false;

	switch (code) {
	case 0: // Program stop
	case 1: // Optional stop
		if (gerber_warnings) {
			LOG(WARNING) << "Line " << gerber_file_.line_number_ << " - Warning: Deprecated code: M" << code;
		}
	case 2: // End of program
		if (current_level_) {
			current_level_->exposure_ = geOff;
			current_level_->Do(gerber_file_.line_number_);
		}
		end_of_file = true;
		return true;

	default: // Select aperture
		LOG(ERROR) << "Line " << gerber_file_.line_number_ << " - Error: Unknown M Code: " << code;
		return false;
	}
}

void Gerber::Add(std::shared_ptr<GerberLevel> level) {
	if (current_level_) {
		current_level_->exposure_ = geOff;
		current_level_->Do(gerber_file_.line_number_);
	}

	levels_.push_back(level);

	current_level_ = level;
}

bool Gerber::IsNegative() const
{
	return negative_;
}

BoundBox Gerber::GetBBox() const {
	if (levels_.empty()) {
		return BoundBox(0.0, 0.0, 0.0, 0.0);
	}

	BoundBox bound_box(1e3, -1e3, -1e3, 1e3);
	std::for_each(levels_.begin(), levels_.end(), [&bound_box](std::shared_ptr<GerberLevel> level) {
		bound_box.UpdateBox(level->bound_box_.Left(), level->GetRight(), level->GetTop(), level->bound_box_.Bottom());
	}
	);

	return bound_box;
}

GERBER_UNIT Gerber::Unit() const
{
	return units_;
}

std::string Gerber::Name() const
{
	return name_;
}

std::vector<std::shared_ptr<GerberLevel>> Gerber::Levels() const
{
	return levels_;
}

bool Gerber::ParseGerber() {
	bool   end_of_file;
	double d;

	while (!gerber_file_.EndOfFile()) {
		gerber_file_.SkipWhiteSpace();

		switch (gerber_file_.GetChar()) {
		case 'N': {
			auto parser = parsers_['N'];
			if (!parser->Run()) {
				return false;
			}
			break;
		}

		case 'G': {
			auto parser = parsers_['G'];
			if (!parser->Run()) {
				return false;
			}

			break;
		}

		case 'D': {
			auto parser = parsers_['D'];
			if (!parser->Run()) {
				return false;
			}

			break;
		}

		case 'M':
			if (!MCode(end_of_file))
				return false;

			if (end_of_file) {
				return true;
			}
			break;

		case 'X':
			if (!gerber_file_.GetCoordinate(d, format_.XInteger, format_.XDecimal, format_.omit_trailing_zeroes_))
				return false;

			if (!current_level_)
				Add(std::make_shared<GerberLevel>(nullptr, units_));

			current_level_->X = d;
			break;

		case 'Y':
			if (!gerber_file_.GetCoordinate(d, format_.YInteger, format_.YDecimal, format_.omit_trailing_zeroes_))
				return false;

			if (!current_level_)
				Add(std::make_shared<GerberLevel>(nullptr, units_));

			current_level_->Y = d;
			break;

		case 'I':
			if (!gerber_file_.GetCoordinate(d, format_.XInteger, format_.XDecimal, format_.omit_trailing_zeroes_))
				return false;
			if (!current_level_)
				Add(std::make_shared<GerberLevel>(nullptr, units_));
			current_level_->I = d;
			break;

		case 'J':
			if (!gerber_file_.GetCoordinate(d, format_.YInteger, format_.YDecimal, format_.omit_trailing_zeroes_))
				return false;
			if (!current_level_)
				Add(std::make_shared<GerberLevel>(nullptr, units_));
			current_level_->J = d;
			break;

		case '*':
			if (current_level_)
				current_level_->Do(gerber_file_.line_number_);
			break;

		default: {
			auto parser = parsers_['0'];
			if (!parser->Run()) {
				return false;
			}

			break;
		}
		}
	}

	LOG(ERROR) << "Line " << gerber_file_.line_number_ << " - Error: No end-of-file code";
	return false;
}

bool Gerber::LoadGerber(const std::string& file_name) {
	start_of_level_ = false;

	if (!gerber_file_.Load(file_name)) {
		return false;
	}

	return ParseGerber();
}
