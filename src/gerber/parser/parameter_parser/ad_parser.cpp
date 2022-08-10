#include "ad_parser.h"
#include "gerber/gerber.h"
#include "gerber/gerber_parser.h"
#include "parser/parameter_parser.h"
#include "gerber/aperture/macro_aperture.h"
#include "gerber/aperture/circle_aperture.h"
#include "gerber/aperture/obround_aperture.h"
#include "gerber/aperture/rectangle_aperture.h"
#include "gerber/aperture/polygon_aperture.h"

#include <glog/logging.h>


AdParser::AdParser(GerberParser& gerber, ParameterParser& param_parser) : gerber_parser_(gerber), param_parser_(param_parser)
{
}

bool AdParser::Run()
{
	gerber_parser_.start_of_level_ = false;

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('D')) {
		return false;
	}

	int code = 0;
	if (!gerber_parser_.gerber_file_->GetInteger(code)) {
		return false;
	}

	std::string aperture_type;
	if (!gerber_parser_.gerber_file_->GetString(aperture_type)) {
		return false;
	}

	if (aperture_type.size() > 1) {
		return ApertureMacro(code, aperture_type);
	}

	switch (aperture_type[0]) {
	case 'C':
		return ApertureCircle(code);

	case 'R':
		return ApertureRectangle(code);

	case 'O':
		return ApertureObround(code);

	case 'P':
		return AperturePolygon(code);

	default:
		return ApertureMacro(code, aperture_type);
	}

	return false;
}

bool AdParser::ApertureMacro(int code, const std::string& name) {
	auto macro = param_parser_.FindMacro(name);
	if (!macro) {
		LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Macro not defined: " << name;
		return false;
	}

	if (gerber_parser_.gerber_file_->EndOfFile()) {
		return false;
	}

	gerber_parser_.gerber_file_->SkipWhiteSpace();
	if (gerber_parser_.gerber_file_->PeekChar() != ',') {
		if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*')) {
			return false;
		}

		auto aperture = std::make_shared<MacroAperture>(code, macro, std::vector<double>{});
		return Add(aperture);
	}
	gerber_parser_.gerber_file_->pointer_++;

	std::vector<double> modifiers;
	double tmp = 0.0;
	if (!gerber_parser_.gerber_file_->GetFloat(tmp)) {
		return false;
	}
	modifiers.push_back(tmp);

	while (!gerber_parser_.gerber_file_->EndOfFile()) {
		if (gerber_parser_.gerber_file_->PeekChar() == 'X') {
			gerber_parser_.gerber_file_->pointer_++;

			if (!gerber_parser_.gerber_file_->GetFloat(tmp)) {
				return false;
			}
			modifiers.push_back(tmp);
		}
		else {
			break;
		}
		gerber_parser_.gerber_file_->SkipWhiteSpace();
	}

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	auto aperture = std::make_shared<MacroAperture>(code, macro, modifiers);
	return Add(aperture);
}

bool AdParser::ApertureCircle(int code) {
	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace(',')) {
		return false;
	}

	double d = 0.0;
	if (!gerber_parser_.gerber_file_->GetFloat(d)) {
		return false;
	}

	if (gerber_parser_.gerber_file_->SkipWhiteSpace()) {
		return false;
	}

	double x = -1.0;
	double y = -1.0;
	if (gerber_parser_.gerber_file_->PeekChar() == 'X') {
		gerber_parser_.gerber_file_->pointer_++;
		if (!gerber_parser_.gerber_file_->GetFloat(x)) {
			return false;
		}

		if (gerber_parser_.gerber_file_->SkipWhiteSpace()) {
			return false;
		}

		if (gerber_parser_.gerber_file_->PeekChar() == 'Y') {
			gerber_parser_.gerber_file_->pointer_++;
			if (!gerber_parser_.gerber_file_->GetFloat(y)) {
				return false;
			}
		}
	}

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	return Add(std::make_shared<CircleAperture>(code, UnitType::Get_mm(d, gerber_parser_.gerber_->unit_),
		UnitType::Get_mm(x, gerber_parser_.gerber_->unit_), UnitType::Get_mm(y, gerber_parser_.gerber_->unit_)));
}

bool AdParser::ApertureRectangle(int code) {
	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace(',')) {
		return false;
	}

	double w = 0.0;
	double h = 0.0;
	double x = 0.0;
	double y = -1.0;

	if (!gerber_parser_.gerber_file_->GetFloat(w)) {
		return false;
	}

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('X')) {
		return false;
	}

	if (!gerber_parser_.gerber_file_->GetFloat(h)) {
		return false;
	}

	if (gerber_parser_.gerber_file_->SkipWhiteSpace()) {
		return false;
	}

	if (gerber_parser_.gerber_file_->PeekChar() == 'X') {
		gerber_parser_.gerber_file_->pointer_++;
		if (!gerber_parser_.gerber_file_->GetFloat(x)) {
			return false;
		}

		if (gerber_parser_.gerber_file_->SkipWhiteSpace()) {
			return false;
		}

		if (gerber_parser_.gerber_file_->PeekChar() == 'Y') {
			gerber_parser_.gerber_file_->pointer_++;
			if (!gerber_parser_.gerber_file_->GetFloat(y)) {
				return false;
			}
		}
	}

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	return Add(std::make_shared<RectangleAperture>(code, UnitType::Get_mm(w, gerber_parser_.gerber_->unit_),
		UnitType::Get_mm(h, gerber_parser_.gerber_->unit_), UnitType::Get_mm(x, gerber_parser_.gerber_->unit_), UnitType::Get_mm(y, gerber_parser_.gerber_->unit_)));
}

bool AdParser::ApertureObround(int code) {
	double w = 0.0;
	double h = 0.0;
	double x = 0.0;
	double y = -1.0;

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace(',')) {
		return false;
	}

	if (!gerber_parser_.gerber_file_->GetFloat(w)) {
		return false;
	}

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('X')) {
		return false;
	}

	if (!gerber_parser_.gerber_file_->GetFloat(h)) {
		return false;
	}

	if (gerber_parser_.gerber_file_->SkipWhiteSpace()) {
		return false;
	}

	if (gerber_parser_.gerber_file_->PeekChar() == 'X') {
		gerber_parser_.gerber_file_->pointer_++;
		if (!gerber_parser_.gerber_file_->GetFloat(x) || gerber_parser_.gerber_file_->SkipWhiteSpace()) {
			return false;
		}

		if (gerber_parser_.gerber_file_->PeekChar() == 'Y') {
			gerber_parser_.gerber_file_->pointer_++;
			if (!gerber_parser_.gerber_file_->GetFloat(y)) {
				return false;
			}
		}
	}

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	return Add(std::make_shared<ObroundAperture>(code, UnitType::Get_mm(w, gerber_parser_.gerber_->unit_),
		UnitType::Get_mm(h, gerber_parser_.gerber_->unit_), UnitType::Get_mm(x, gerber_parser_.gerber_->unit_), UnitType::Get_mm(y, gerber_parser_.gerber_->unit_)));
}

bool AdParser::AperturePolygon(int code) {
	int n = 0;
	double w = 0.0;
	double a = 0.0;
	double x = -1.0;
	double y = -1.0;
	double tmp = 0.0;

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace(',')) {
		return false;
	}

	if (!gerber_parser_.gerber_file_->GetFloat(w)) {
		return false;
	}

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('X')) {
		return false;
	}

	if (!gerber_parser_.gerber_file_->GetFloat(tmp)) {
		return false;
	}
	n = round(tmp); // Some Gerbers use floats everywhere, even when it should
					 // actually be an integer...

	if (gerber_parser_.gerber_file_->SkipWhiteSpace()) {
		return false;
	}

	if (gerber_parser_.gerber_file_->PeekChar() == 'X') {
		gerber_parser_.gerber_file_->pointer_++;
		if (!gerber_parser_.gerber_file_->GetFloat(a) || gerber_parser_.gerber_file_->SkipWhiteSpace()) {
			return false;
		}

		if (gerber_parser_.gerber_file_->PeekChar() == 'X') {
			gerber_parser_.gerber_file_->pointer_++;
			if (!gerber_parser_.gerber_file_->GetFloat(x) || gerber_parser_.gerber_file_->SkipWhiteSpace()) {
				return false;
			}

			if (gerber_parser_.gerber_file_->PeekChar() == 'Y') {
				gerber_parser_.gerber_file_->pointer_++;
				if (!gerber_parser_.gerber_file_->GetFloat(y)) {
					return false;
				}
			}
		}
	}

	if (!gerber_parser_.gerber_file_->QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	return Add(std::make_shared<PolygonAperture>(code, UnitType::Get_mm(w, gerber_parser_.gerber_->unit_), n, a, UnitType::Get_mm(x, gerber_parser_.gerber_->unit_),
		UnitType::Get_mm(y, gerber_parser_.gerber_->unit_)));
}

bool AdParser::Add(const std::shared_ptr<Aperture>& aperture) {
	if (aperture->Code() >= 1000) {// NOLINT
		LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Aperture code out of range: D" << aperture->Code();
		return false;
	}
	if (gerber_parser_.GetAperture(aperture->Code())) {
		LOG(ERROR) << "Line " << gerber_parser_.gerber_file_->line_number_ << " - Error: Overloading of apertures not supported: D" << aperture->Code();
		return false;
	}

	gerber_parser_.SetAperture(aperture->Code(), aperture);
	return true;
}
