#include "gerber.h"
#include "parser/xcode_parser.h"
#include "parser/ycode_parser.h"
#include "parser/icode_parser.h"
#include "parser/jcode_parser.h"
#include "parser/star_parser.h"
#include "parser/gcode_parser.h"
#include "parser/dcode_parser.h"
#include "parser/ncode_parser.h"
#include "parser/mcode_parser.h"
#include "parser/parameter_parser.h"
#include <glog/logging.h>

#include <algorithm>


bool gerber_warnings = true;

Gerber::Gerber(const std::string& file_name) : file_name_(file_name) {
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

	parsers_['X'] = std::make_shared<XCodeParser>(*this);
	parsers_['Y'] = std::make_shared<YCodeParser>(*this);
	parsers_['I'] = std::make_shared<ICodeParser>(*this);
	parsers_['J'] = std::make_shared<JCodeParser>(*this);
	parsers_['D'] = std::make_shared<DCodeParser>(*this);
	parsers_['G'] = std::make_shared<GCodeParser>(*this);
	parsers_['N'] = std::make_shared<NCodeParser>(*this);
	parsers_['M'] = std::make_shared<MCodeParser>(*this);
	parsers_['*'] = std::make_shared<StarParser>(*this);
	parsers_['0'] = std::make_shared<ParameterParser>(*this);

	LoadGerber(file_name);
}


Gerber::~Gerber() {
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

std::string Gerber::FileName() const
{
	return file_name_;
}

std::vector<std::shared_ptr<GerberLevel>> Gerber::Levels() const
{
	return levels_;
}

bool Gerber::ParseGerber() {
	while (!gerber_file_.EndOfFile()) {
		gerber_file_.SkipWhiteSpace();

		auto parser = GetParser(gerber_file_.GetChar());
		if (!parser->Run()) {
			return false;
		}

		if (parser->EndOfFile()) {
			return true;
		}
	}

	LOG(ERROR) << "Line " << gerber_file_.line_number_ << " - Error: No end-of-file code";
	return false;
}

std::shared_ptr<Parser> Gerber::GetParser(char code)
{
	if (parsers_.find(code) != parsers_.end()) {
		return parsers_[code];
	}

	return parsers_['0'];
}

bool Gerber::LoadGerber(const std::string& file_name) {
	start_of_level_ = false;

	if (!gerber_file_.Load(file_name)) {
		return false;
	}

	return ParseGerber();
}
