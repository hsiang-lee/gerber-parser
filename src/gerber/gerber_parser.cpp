#include "gerber_parser.h"
#include <glog/logging.h>

#include <fstream>
#include <iostream>

#include "gerber/plotter.h"
#include "gerber/gerber_level.h"
#include "parser/star_parser.h"
#include "parser/dcode_parser.h"
#include "parser/gcode_parser.h"
#include "parser/icode_parser.h"
#include "parser/jcode_parser.h"
#include "parser/mcode_parser.h"
#include "parser/ncode_parser.h"
#include "parser/xcode_parser.h"
#include "parser/ycode_parser.h"
#include "parser/parameter_parser.h"


GerberParser::GerberParser(const std::string& file_name) : file_name_(file_name) {
	Init();
	LoadGerber(file_name);
}

GerberParser::GerberParser(const std::vector<char>& gerber_data)
{
	Init();
	LoadGerber(gerber_data);
}

void GerberParser::Init()
{
	format_.omit_trailing_zeroes_ = false;
	format_.XInteger = 6;// NOLINT
	format_.XDecimal = 6;// NOLINT
	format_.YInteger = 6;// NOLINT
	format_.YDecimal = 6;// NOLINT

	current_level_ = nullptr;

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
}

std::string GerberParser::FileName() const
{
	return file_name_;
}

std::shared_ptr<Gerber> GerberParser::GetGerber() const
{
	return gerber_;
}

bool GerberParser::ParseGerber() {
	while (!gerber_file_->EndOfFile()) {
		gerber_file_->SkipWhiteSpace();

		auto parser = GetParser(gerber_file_->GetChar());
		if (!parser->Run()) {
			return false;
		}

		if (parser->EndOfFile()) {
			return true;
		}
	}

	LOG(ERROR) << "Line " << gerber_file_->line_number_ << " - Error: No end-of-file code";
	return false;
}

std::shared_ptr<Parser> GerberParser::GetParser(char code)
{
	if (parsers_.find(code) != parsers_.end()) {
		return parsers_[code];
	}

	return parsers_['0'];
}

std::shared_ptr<Aperture> GerberParser::GetAperture(int code) const
{
	return gerber_->apertures_[code];
}

void GerberParser::SetAperture(int code, std::shared_ptr<Aperture> aperture)
{
	gerber_->apertures_[code] = aperture;
}

void GerberParser::AddNewLevel()
{
	auto level = std::make_shared<GerberLevel>(current_level_, gerber_->unit_);
	gerber_->levels_.push_back(level);
	current_level_ = level;

	if (plotter_) {
		plotter_->SetExposure(geOff);
		plotter_->Do(gerber_file_->line_number_);
	}
	plotter_ = std::make_shared<Plotter>(*level, plotter_);
}

void GerberParser::PrepareEmptyLevel()
{
	if (!current_level_) {
		auto level = std::make_shared<GerberLevel>(nullptr, gerber_->unit_);
		gerber_->levels_.push_back(level);
		current_level_ = level;

		plotter_ = std::make_shared<Plotter>(*level, plotter_);
	}
}

void GerberParser::PrepareLevel()
{
	if (!current_level_) {
		AddNewLevel();
	}
}

bool GerberParser::LoadGerber(const std::string& file_name) {
	std::ifstream file(file_name, std::ios::in);
	if (!file) {
		std::cout << "failed to open gerber file." << std::endl;
		return false;
	}

	return LoadGerber(std::vector<char>{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() });
}

bool GerberParser::LoadGerber(const std::vector<char>& gerber_data)
{
	start_of_level_ = false;
	gerber_ = std::make_shared<Gerber>();
	gerber_file_ = std::make_unique<GerberFile>(gerber_data);
	return ParseGerber();
}
