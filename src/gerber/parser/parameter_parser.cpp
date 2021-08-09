#include "parameter_parser.h"
#include "gerber.h"
#include <glog/logging.h>


ParameterParser::ParameterParser(Gerber& gerber) :gerber_(gerber) {

}

ParameterParser::~ParameterParser()
{
}

bool ParameterParser::Run() {
	auto delimiter = gerber_.gerber_file_.buffer_[gerber_.gerber_file_.index_ - 1];

	gerber_.gerber_file_.SkipWhiteSpace();

	while (gerber_.gerber_file_.index_ < gerber_.gerber_file_.buffer_.size() - 1) {
		auto first = gerber_.gerber_file_.PeekChar();
		if (first == delimiter) {
			gerber_.gerber_file_.index_ += 1;
			return true;
		}

		auto second = gerber_.gerber_file_.PeekNextChar();
		std::string code;
		code += first;
		code += second;

		if (code == "AD") {
			gerber_.gerber_file_.index_ += 2;
			if (!ApertureDefinition()) {
				return false;
			}
		}
		else if (code == "AM") {
			gerber_.gerber_file_.index_ += 2;
			if (!ApertureMacro()) {
				return false;
			}
		}
		else if (code == "AS") {
			if (gerber_warnings) {
				LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated command: AS";
			}
			gerber_.gerber_file_.index_ += 2;
			if (!AxisSelect()) {
				return false;
			}
		}
		else if (code == "FS") {
			gerber_.gerber_file_.index_ += 2;
			if (!FormatStatement()) {
				return false;
			}
		}
		else if (code == "IC") {
			gerber_.gerber_file_.index_ += 2;
			if (!IC()) {
				return false;
			}
		}
		else if (code == "IF") {
			gerber_.gerber_file_.index_ += 2;
			if (!IncludeFile()) {
				return false;
			}
		}
		else if (code == "IJ") {
			gerber_.gerber_file_.index_ += 2;
			if (!ImageJustify()) {
				return false;
			}
		}
		else if (code == "IN") {
			if (gerber_warnings) {
				LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated command: IN";
			}
			gerber_.gerber_file_.index_ += 2;
			if (!ImageName()) {
				return false;
			}
		}
		else if (code == "IO") {
			gerber_.gerber_file_.index_ += 2;
			if (!ImageOffset()) {
				return false;
			}
		}
		else if (code == "IP") {
			if (gerber_warnings) {
				LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated command: IP";
			}
			gerber_.gerber_file_.index_ += 2;
			if (!ImagePolarity()) {
				return false;
			}
		}
		else if (code == "IR") {
			if (gerber_warnings) {
				LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated command: IR";
			}
			gerber_.gerber_file_.index_ += 2;
			if (!ImageRotation()) {
				return false;
			}
		}
		else if (code == "KO") {
			gerber_.gerber_file_.index_ += 2;
			if (!Knockout()) {
				return false;
			}
		}
		else if (code == "LN") {
			if (gerber_warnings) {
				LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated command: LN";
			}
			gerber_.gerber_file_.index_ += 2;
			if (!LevelName()) {
				return false;
			}
		}
		else if (code == "LP") {
			gerber_.gerber_file_.index_ += 2;
			if (!LevelPolarity()) {
				return false;
			}
		}
		else if (code == "MI") {
			if (gerber_warnings) {
				LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated command: MI";
			}
			gerber_.gerber_file_.index_ += 2;
			if (!MirrorImage()) {
				return false;
			}
		}
		else if (code == "MO") {
			gerber_.gerber_file_.index_ += 2;
			if (!Mode()) {
				return false;
			}
		}
		else if (code == "OF") {
			if (gerber_warnings) {
				LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated command: OF";
			}
			gerber_.gerber_file_.index_ += 2;
			if (!Offset()) {
				return false;
			}
		}
		else if (code == "PF") {
			gerber_.gerber_file_.index_ += 2;
			if (!PlotFilm()) {
				return false;
			}
		}
		else if (code == "SF") {
			if (gerber_warnings) {
				LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Deprecated command: SF";
			}
			gerber_.gerber_file_.index_ += 2;
			if (!ScaleFactor()) {
				return false;
			}
		}
		else if (code == "SR") {
			gerber_.gerber_file_.index_ += 2;
			if (!StepAndRepeat()) {
				return false;
			}
		}
		else if (code == "IA") {
			gerber_.gerber_file_.index_ += 2;
			if (!Attribute()) {
				return false; // Aperture Attribute
			}
		}
		else if (code == "TD") {
			gerber_.gerber_file_.index_ += 2;
			if (!Attribute()) {
				return false; // Delete Attribute
			}
		}
		else if (code == "TF") {
			gerber_.gerber_file_.index_ += 2;
			if (!Attribute()) {
				return false; // File Attribute
			}
		}
		else if (code == "TO") {
			gerber_.gerber_file_.index_ += 2;
			if (!Attribute()) {
				return false; // Object Attribute
			}
		}
		else {
			LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Unknown parameter: " << code;
			return false;
		}

		gerber_.gerber_file_.SkipWhiteSpace();
	}

	LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Parameter Block without end-delimiter";

	return false;
}

bool ParameterParser::EndOfFile()
{
	return false;
}

bool ParameterParser::ApertureDefinition() {
	gerber_.start_of_level_ = false;

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('D')) {
		return false;
	}

	int code;
	if (!gerber_.gerber_file_.GetInteger(code)) {
		return false;
	}

	std::string aperture_type;
	if (!gerber_.gerber_file_.GetString(aperture_type)) {
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

bool ParameterParser::ApertureCircle(int code) {
	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace(',')) {
		return false;
	}

	double d = 0.0;
	if (!gerber_.gerber_file_.GetFloat(d)) {
		return false;
	}

	if (gerber_.gerber_file_.SkipWhiteSpace()) {
		return false;
	}

	double x = -1.0;
	double y = -1.0;
	if (gerber_.gerber_file_.PeekChar() == 'X') {
		gerber_.gerber_file_.index_++;
		if (!gerber_.gerber_file_.GetFloat(x)) {
			return false;
		}

		if (gerber_.gerber_file_.SkipWhiteSpace()) {
			return false;
		}

		if (gerber_.gerber_file_.PeekChar() == 'Y') {
			gerber_.gerber_file_.index_++;
			if (!gerber_.gerber_file_.GetFloat(y)) {
				return false;
			}
		}
	}

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	auto aperture = std::make_shared<GerberAperture>();
	aperture->code_ = code;
	aperture->Circle(Get_mm(d));

	if (x > 0.0) {
		if (y > 0.0) {
			aperture->HoleRectangle(Get_mm(x), Get_mm(y));
		}
		else {
			aperture->HoleCircle(Get_mm(x));
		}
	}

	return Add(aperture);
}

bool ParameterParser::ApertureRectangle(int code) {
	double w, h, x, y;

	w = h = 0.0;
	x = y = -1.0;

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace(',')) {
		return false;
	}

	if (!gerber_.gerber_file_.GetFloat(w)) {
		return false;
	}

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('X')) {
		return false;
	}

	if (!gerber_.gerber_file_.GetFloat(h)) {
		return false;
	}

	if (gerber_.gerber_file_.SkipWhiteSpace()) {
		return false;
	}

	if (gerber_.gerber_file_.PeekChar() == 'X') {
		gerber_.gerber_file_.index_++;
		if (!gerber_.gerber_file_.GetFloat(x)) {
			return false;
		}

		if (gerber_.gerber_file_.SkipWhiteSpace()) {
			return false;
		}

		if (gerber_.gerber_file_.PeekChar() == 'Y') {
			gerber_.gerber_file_.index_++;
			if (!gerber_.gerber_file_.GetFloat(y)) {
				return false;
			}
		}
	}

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	auto aperture = std::make_shared<GerberAperture>();
	aperture->code_ = code;
	aperture->Rectangle(Get_mm(w), Get_mm(h));
	if (x > 0.0) {
		if (y > 0.0) {
			aperture->HoleRectangle(Get_mm(x), Get_mm(y));
		}
		else {
			aperture->HoleCircle(Get_mm(x));
		}
	}
	return Add(aperture);
}

bool ParameterParser::ApertureObround(int code) {
	double w, h, x, y;

	w = h = 0.0;
	x = y = -1.0;

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace(',')) {
		return false;
	}

	if (!gerber_.gerber_file_.GetFloat(w)) {
		return false;
	}

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('X')) {
		return false;
	}

	if (!gerber_.gerber_file_.GetFloat(h)) {
		return false;
	}

	if (gerber_.gerber_file_.SkipWhiteSpace()) {
		return false;
	}

	if (gerber_.gerber_file_.PeekChar() == 'X') {
		gerber_.gerber_file_.index_++;
		if (!gerber_.gerber_file_.GetFloat(x) || gerber_.gerber_file_.SkipWhiteSpace()) {
			return false;
		}

		if (gerber_.gerber_file_.PeekChar() == 'Y') {
			gerber_.gerber_file_.index_++;
			if (!gerber_.gerber_file_.GetFloat(y)) {
				return false;
			}
		}
	}

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	auto aperture = std::make_shared<GerberAperture>();
	aperture->code_ = code;
	aperture->Obround(Get_mm(w), Get_mm(h));
	if (x > 0.0) {
		if (y > 0.0) {
			aperture->HoleRectangle(Get_mm(x), Get_mm(y));
		}
		else {
			aperture->HoleCircle(Get_mm(x));
		}
	}
	return Add(aperture);
}

bool ParameterParser::AperturePolygon(int code) {
	int    n;
	double w, a, x, y, tmp;
	std::shared_ptr<GerberAperture> aperture;

	n = 0;
	w = 0.0;
	a = 0.0;
	x = -1.0;
	y = -1.0;

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace(',')) {
		return false;
	}

	if (!gerber_.gerber_file_.GetFloat(w)) {
		return false;
	}

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('X')) {
		return false;
	}

	if (!gerber_.gerber_file_.GetFloat(tmp)) {
		return false;
	}
	n = round(tmp); // Some Gerbers use floats everywhere, even when it should
					 // actually be an integer...

	if (gerber_.gerber_file_.SkipWhiteSpace()) {
		return false;
	}

	if (gerber_.gerber_file_.GetChar() == 'X') {
		gerber_.gerber_file_.index_++;
		if (!gerber_.gerber_file_.GetFloat(a) || gerber_.gerber_file_.SkipWhiteSpace()) {
			return false;
		}

		if (gerber_.gerber_file_.PeekChar() == 'X') {
			gerber_.gerber_file_.index_++;
			if (!gerber_.gerber_file_.GetFloat(x) || gerber_.gerber_file_.SkipWhiteSpace()) {
				return false;
			}

			if (gerber_.gerber_file_.PeekChar() == 'Y') {
				gerber_.gerber_file_.index_++;
				if (!gerber_.gerber_file_.GetFloat(y)) {
					return false;
				}
			}
		}
	}

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	aperture = std::make_shared<GerberAperture>();
	aperture->code_ = code;
	aperture->Polygon(Get_mm(w), n, a);
	if (x > 0.0) {
		if (y > 0.0) {
			aperture->HoleRectangle(Get_mm(x), Get_mm(y));
		}
		else {
			aperture->HoleCircle(Get_mm(x));
		}
	}
	return Add(aperture);
}

bool ParameterParser::ApertureMacro(int code, const std::string& name) {
	// Variable length array; Null terminated
	double* modifiers = new double[16];
	int     modifiers_size = 16;
	int     modifiers_length_ = 0;

	auto macro = FindMacro(name.c_str());
	if (!macro) {
		LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Macro not defined: " << name;
		delete[] modifiers;
		return false;
	}

	if (gerber_.gerber_file_.EndOfFile()) {
		delete[] modifiers;
		return false;
	}

	gerber_.gerber_file_.SkipWhiteSpace();

	if (gerber_.gerber_file_.PeekChar() != ',') {
		auto aperture = std::make_shared<GerberAperture>();
		aperture->code_ = code;
		aperture->UseMacro(macro, modifiers, 0);
		delete[] modifiers;

		if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('*')) {
			return false;
		}

		return Add(aperture);
	}
	gerber_.gerber_file_.index_++;

	if (!gerber_.gerber_file_.GetFloat(*modifiers)) {
		delete[] modifiers;
		return false;
	}
	modifiers_length_++;

	gerber_.gerber_file_.SkipWhiteSpace();

	while (!gerber_.gerber_file_.EndOfFile()) {
		if (gerber_.gerber_file_.PeekChar() == 'X') {
			gerber_.gerber_file_.index_++;

			if (modifiers_length_ == modifiers_size) {
				modifiers_size <<= 1;
				auto temp_modifiers = new double[modifiers_size];
				for (int j = 0; j < modifiers_length_; j++) {
					temp_modifiers[j] = modifiers[j];
				}
				delete[] modifiers;
				modifiers = temp_modifiers;
			}

			if (!gerber_.gerber_file_.GetFloat(*(modifiers + modifiers_length_))) {
				delete[] modifiers;
				return false;
			}
			modifiers_length_++;
		}
		else {
			break;
		}
		gerber_.gerber_file_.SkipWhiteSpace();
	}

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('*')) {
		delete[] modifiers;
		return false;
	}

	auto aperture = std::make_shared<GerberAperture>();
	aperture->code_ = code;
	aperture->UseMacro(macro, modifiers, modifiers_length_);
	return Add(aperture);
}

bool ParameterParser::ApertureMacro() {
	gerber_.start_of_level_ = false;

	std::string name;
	if (!gerber_.gerber_file_.GetString(name)) {
		return false;
	}

	auto macro = std::make_shared<GerberMacro>();
	macro->Name = name;

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	gerber_.gerber_file_.SkipWhiteSpace();

	auto j = gerber_.gerber_file_.index_;
	while (!gerber_.gerber_file_.EndOfFile()) {
		if (gerber_.gerber_file_.PeekChar() == '%') {
			if (macro->LoadMacro(gerber_.gerber_file_.buffer_.data() + j, gerber_.gerber_file_.index_ - j, gerber_.units_ == guInches)) {
				Add(macro);
			}
			else {
				LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Invalid aperture macro";
			}
			return true;
		}
		gerber_.gerber_file_.index_++;
		gerber_.gerber_file_.SkipWhiteSpace();
	}

	return false;
}

bool ParameterParser::AxisSelect() {
	LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: AxisSelect ignored";

	gerber_.start_of_level_ = false;
	return gerber_.gerber_file_.QueryCharUntilEnd('%');
}

bool ParameterParser::FormatStatement() {
	gerber_.start_of_level_ = false;

	gerber_.gerber_file_.SkipWhiteSpace();

	while (gerber_.gerber_file_.index_ < gerber_.gerber_file_.buffer_.size() - 1) {
		switch (gerber_.gerber_file_.PeekChar()) {
		case 'L':
			gerber_.format_.omit_trailing_zeroes_ = false;
			break;

		case 'T':
			gerber_.format_.omit_trailing_zeroes_ = true;
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
			gerber_.gerber_file_.index_++;
			break;

		case 'X':
			gerber_.gerber_file_.index_++;
			gerber_.format_.XInteger = gerber_.gerber_file_.GetChar() - '0';
			gerber_.format_.XDecimal = gerber_.gerber_file_.PeekChar() - '0';
			break;

		case 'Y':
			gerber_.gerber_file_.index_++;
			gerber_.format_.YInteger = gerber_.gerber_file_.GetChar() - '0';
			gerber_.format_.YDecimal = gerber_.gerber_file_.PeekChar() - '0';
			break;

		case '*':
			gerber_.gerber_file_.index_++;
			return true;

		default:
			LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Unrecognised FS modifier: " << gerber_.gerber_file_.PeekChar();
			return false;
		}
		gerber_.gerber_file_.index_++;
		gerber_.gerber_file_.SkipWhiteSpace();
	}

	return false;
}

bool ParameterParser::IC() {
	LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: IC Parameter ignored";

	gerber_.start_of_level_ = false;
	return gerber_.gerber_file_.QueryCharUntilEnd('%');
}

bool ParameterParser::MirrorImage() {
	LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: MirrorImage not implemented";

	gerber_.start_of_level_ = false;

	while (!gerber_.gerber_file_.EndOfFile()) {
		if (gerber_.gerber_file_.GetChar() == '*') {
			return true;
		}
	}

	return false;
}

bool ParameterParser::Mode() {
	gerber_.start_of_level_ = false;

	gerber_.gerber_file_.SkipWhiteSpace();

	while (gerber_.gerber_file_.index_ < gerber_.gerber_file_.buffer_.size() - 1) {
		if (gerber_.gerber_file_.PeekChar() == 'I' && gerber_.gerber_file_.PeekNextChar() == 'N') {
			gerber_.units_ = guInches;
			gerber_.gerber_file_.index_ += 2;

		}
		else if (gerber_.gerber_file_.PeekChar() == 'M' && gerber_.gerber_file_.PeekNextChar() == 'M') {
			gerber_.units_ = guMillimeters;
			gerber_.gerber_file_.index_ += 2;

		}
		else if (gerber_.gerber_file_.PeekChar() == '*') {
			if (gerber_.current_level_) {
				gerber_.current_level_->units_ = gerber_.units_;
			}

			gerber_.gerber_file_.index_++;
			return true;

		}
		else {
			LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Unrecognised MO modifier: " << gerber_.gerber_file_.PeekChar();
			return false;
		}
		gerber_.gerber_file_.SkipWhiteSpace();
	}

	return false;
}

bool ParameterParser::Offset() {
	double d;

	gerber_.start_of_level_ = false;

	gerber_.gerber_file_.SkipWhiteSpace();
	while (!gerber_.gerber_file_.EndOfFile()) {
		switch (gerber_.gerber_file_.GetChar()) {
		case 'A':
			if (!gerber_.gerber_file_.GetCoordinate(d, 5, 5, gerber_.format_.omit_trailing_zeroes_)) {
				return false;
			}
			offset_a_ = d;
			break;

		case 'B':
			if (!gerber_.gerber_file_.GetCoordinate(d, 5, 5, gerber_.format_.omit_trailing_zeroes_)) {
				return false;
			}
			offset_b_ = d;
			break;

		case '*':
			if (offset_a_ != 0.0 || offset_b_ != 0.0) {
				LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Offsets ignored";
			}
			return true;

		default:
			LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Unrecognised Offset Modifier: " << gerber_.gerber_file_.PeekChar();
			return false;
		}
		gerber_.gerber_file_.SkipWhiteSpace();
	}

	return false;
}

bool ParameterParser::ScaleFactor() {
	double d;

	gerber_.start_of_level_ = false;

	gerber_.gerber_file_.SkipWhiteSpace();

	while (!gerber_.gerber_file_.EndOfFile()) {
		switch (gerber_.gerber_file_.GetChar()) {
		case 'A':
			if (!gerber_.gerber_file_.GetFloat(d)) {
				return false;
			}
			scale_a_ = d;
			break;

		case 'B':
			if (!gerber_.gerber_file_.GetFloat(d)) {
				return false;
			}
			scale_b_ = d;
			break;

		case '*':
			if (scale_b_ != 1.0 || scale_b_ != 1.0) {
				LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: Scale Factor ignored";
			}
			return true;

		default:
			LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Unrecognised Scale Factor Modifier: " << gerber_.gerber_file_.PeekChar();
			return false;
		}
		gerber_.gerber_file_.SkipWhiteSpace();
	}

	return false;
}

bool ParameterParser::Add(std::shared_ptr<GerberAperture> aperture) {
	if (aperture->code_ >= 1000) {
		LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Aperture code out of range: D" << aperture->code_;
		return false;
	}
	if (gerber_.apertures_[aperture->code_]) {
		LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Overloading of apertures not supported: D" << aperture->code_;
		return false;
	}

	gerber_.apertures_[aperture->code_] = aperture;
	return true;
}

bool ParameterParser::IncludeFile() {
	LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: IncludeFile not implemented";

	gerber_.start_of_level_ = false;
	return gerber_.gerber_file_.QueryCharUntilEnd('%');
}

bool ParameterParser::ImageJustify() {
	LOG(WARNING) << "Line " << " - Warning: ImageJustify ignored" << gerber_.gerber_file_.line_number_;

	gerber_.start_of_level_ = false;
	return gerber_.gerber_file_.QueryCharUntilEnd('%');
}

bool ParameterParser::ImageName() {
	gerber_.start_of_level_ = false;

	if (!gerber_.gerber_file_.GetString(gerber_.name_)) {
		return false;
	}

	gerber_.gerber_file_.SkipWhiteSpace();
	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	return true;
}

bool ParameterParser::ImageOffset() {
	LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: ImageOffset not implemented";
	gerber_.start_of_level_ = false;
	return gerber_.gerber_file_.QueryCharUntilEnd('%');
}

bool ParameterParser::ImagePolarity() {

	gerber_.start_of_level_ = false;

	gerber_.gerber_file_.SkipWhiteSpace();
	if (gerber_.gerber_file_.index_ >= gerber_.gerber_file_.buffer_.size() - 2) {
		return false;
	}
	if (gerber_.gerber_file_.PeekChar() == 'P' &&
		gerber_.gerber_file_.PeekNextChar() == 'O' &&
		gerber_.gerber_file_.buffer_[gerber_.gerber_file_.index_ + 2] == 'S') {
		gerber_.negative_ = false;
	}
	else if (gerber_.gerber_file_.PeekChar() == 'N' &&
		gerber_.gerber_file_.PeekNextChar() == 'E' &&
		gerber_.gerber_file_.buffer_[gerber_.gerber_file_.index_ + 2] == 'G') {
		gerber_.negative_ = true;
	}
	else {
		LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Unknown Image Polarity";
		return false;
	}
	gerber_.gerber_file_.index_ += 3;

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	return true;
}

bool ParameterParser::ImageRotation() {
	LOG(WARNING) << "Line " << gerber_.gerber_file_.line_number_ << " - Warning: ImageRotation not implemented";

	gerber_.start_of_level_ = false;
	return gerber_.gerber_file_.QueryCharUntilEnd('*');
}

bool ParameterParser::Knockout() {
	LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Knockout not implemented";
	return false;
}

bool ParameterParser::LevelName() {
	std::string name;
	if (!gerber_.gerber_file_.GetString(name)) {
		return false;
	}

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	if (!gerber_.start_of_level_) {
		auto level = std::make_shared<GerberLevel>(gerber_.current_level_, gerber_.units_);
		level->SetName(name);
		gerber_.Add(level);
	}
	else {
		gerber_.current_level_->SetName(name);
	}

	gerber_.start_of_level_ = true;

	return true;
}

bool ParameterParser::LevelPolarity() {
	if (!gerber_.start_of_level_) {
		auto level = std::make_shared<GerberLevel>(gerber_.current_level_, gerber_.units_);
		gerber_.Add(level);
	}
	gerber_.start_of_level_ = true;

	if (gerber_.gerber_file_.SkipWhiteSpace()) {
		return false;
	}

	switch (gerber_.gerber_file_.GetChar()) {
	case 'C':
		gerber_.current_level_->negative_ = true;
		break;

	case 'D':
		gerber_.current_level_->negative_ = false;
		break;

	default:
		LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Unknown level polarity: " << gerber_.gerber_file_.PeekChar();
		return false;
	}

	if (!gerber_.gerber_file_.QueryCharUntilNotWhiteSpace('*')) {
		return false;
	}

	return true;
}

bool ParameterParser::PlotFilm() {
	LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: PlotFilm not implemented";
	gerber_.start_of_level_ = false;
	return gerber_.gerber_file_.QueryCharUntilEnd('%');
}

bool ParameterParser::StepAndRepeat() {
	int          x, y;
	double       i, j;

	if (!gerber_.start_of_level_) {
		auto level = std::make_shared<GerberLevel>(gerber_.current_level_, gerber_.units_);
		gerber_.Add(level);
	}
	gerber_.start_of_level_ = true;

	x = y = 1;
	i = j = 0.0;

	gerber_.gerber_file_.SkipWhiteSpace();
	while (!gerber_.gerber_file_.EndOfFile()) {
		switch (gerber_.gerber_file_.GetChar()) {
		case 'X':
			if (!gerber_.gerber_file_.GetInteger(x)) {
				return false;
			}

			break;

		case 'Y':
			if (!gerber_.gerber_file_.GetInteger(y)) {
				return false;
			}

			break;

		case 'I':
			if (!gerber_.gerber_file_.GetFloat(i)) {
				return false;
			}

			break;

		case 'J':
			if (!gerber_.gerber_file_.GetFloat(j)) {
				return false;
			}

			break;

		case '*':
			gerber_.current_level_->CountX = x;
			gerber_.current_level_->CountY = y;
			gerber_.current_level_->StepX = Get_mm(i);
			gerber_.current_level_->StepY = Get_mm(j);
			return true;

		default:
			LOG(ERROR) << "Line " << gerber_.gerber_file_.line_number_ << " - Error: Unrecognised Step-and-Repeat Modifier: " << gerber_.gerber_file_.PeekChar();
			return false;
		}

		gerber_.gerber_file_.SkipWhiteSpace();
	}

	return false;
}

bool ParameterParser::Attribute() {
	while (!gerber_.gerber_file_.EndOfFile()) {
		if (gerber_.gerber_file_.GetChar() == '*') {
			return true;
		}
	}

	return false;
}

void ParameterParser::Add(std::shared_ptr<GerberMacro> Macro) {
	macros_.push_back(Macro);
}

std::shared_ptr<GerberMacro> ParameterParser::FindMacro(const std::string& Name) {
	for (const auto& macro : macros_) {
		if (macro->Name == Name) {
			return macro;
		}
	}

	return nullptr;
}

double ParameterParser::Get_mm(double number) {
	if (gerber_.units_ == guInches) {
		number *= 25.4;
	}

	return number;
}
