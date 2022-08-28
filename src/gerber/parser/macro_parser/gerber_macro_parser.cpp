#include "gerber_macro_parser.h"
#include "gerber/gerber_macro.h"
#include "gerber_file.h"
#include "operator_item.h"
#include "gerber/primitive_item.h"
#include <glog/logging.h>


GerberMacroParser::GerberMacroParser(GerberMacro& macro, const std::string& buffer) : macro_(macro) {
	gerber_file_ = std::make_unique<GerberFile>(std::vector<char>{buffer.begin(), buffer.end()});
}

GerberMacroParser::~GerberMacroParser()// NOLINT
{
}

bool GerberMacroParser::LoadMacro(bool inches) {
	if (!Primitive()) {
		return false;
	}

	if (gerber_file_->EndOfFile() || gerber_file_->GetChar() != '*') {
		return false;
	}

	while (Primitive()) {
		if (gerber_file_->EndOfFile() || gerber_file_->GetChar() != '*') {
			return false;
		}
	}

	return gerber_file_->EndOfFile();
}

bool GerberMacroParser::Primitive() {
	if (gerber_file_->EndOfFile()) {
		return false;
	}

	int identifier = 0;
	auto ret = gerber_file_->GetInteger(identifier);
	if (!ret) {
		if (gerber_file_->GetChar() == '$') {
			identifier = PrimitiveItem::kAssignment;
		}
		else {
			LOG(ERROR) << "Error: Macro primitive expected";
			return false;
		}
	}

	switch (identifier) {
	case PrimitiveItem::kComment:
		return Comment();

	case PrimitiveItem::kCircle:
		return Circle();

	case PrimitiveItem::kLineVector:
	case PrimitiveItem::kLineVector2:
		return LineVector();

	case PrimitiveItem::kLineCenter:
		return LineCenter();

	case PrimitiveItem::kLineLowerLeft:
		return LineLowerLeft();

	case PrimitiveItem::kEndOfFile:
		return true;

	case PrimitiveItem::kOutline:
		return Outline();

	case PrimitiveItem::kPolygon:
		return Polygon();

	case PrimitiveItem::kMoire:
		return Moire();

	case PrimitiveItem::kThermal:
		return Thermal();

	case PrimitiveItem::kAssignment:
		return Assignment();

	default:
		LOG(ERROR) << "Error: Unknown Macro Primitive: " << gerber_file_->PeekChar();
		return false;
	}

	LOG(ERROR) << "Error: End of block expected";
	return false;
}

bool GerberMacroParser::Comment() {
	while (!gerber_file_->EndOfFile()) {
		if (gerber_file_->GetChar() == '*') {
			return true;
		}
	}

	return false;
}

bool GerberMacroParser::Circle() {
	constexpr int modifier_count = 5;
	auto item = std::make_shared<PrimitiveItem>();
	item->type_ = PrimitiveItem::kCircle;
	item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);

	gerber_file_->SkipWhiteSpace();

	int j = 0;
	bool b = true;
	while (b && !gerber_file_->EndOfFile() && j < modifier_count) {
		if (gerber_file_->PeekChar() == ',') {
			gerber_file_->pointer_++;
			auto tmp = Modifier();
			item->modifiers_[j++] = tmp;
			b = bool(tmp);
		}
		else {
			break; // It is legal to specify fewer modifiers than the maximum
		}

		gerber_file_->SkipWhiteSpace();
	}

	macro_.Add(item);

	return b;
}


bool GerberMacroParser::LineVector() {
	constexpr int modifier_count = 7;
	auto item = std::make_shared<PrimitiveItem>();
	item->type_ = PrimitiveItem::kLineVector;
	item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);

	gerber_file_->SkipWhiteSpace();

	int j = 0;
	bool b = true;
	while (b && !gerber_file_->EndOfFile() && j < modifier_count) {
		if (gerber_file_->GetChar() == ',') {
			auto tmp = Modifier();
			item->modifiers_[j++] = tmp;
			b = bool(tmp);
		}
		else {
			break;
		}

		gerber_file_->SkipWhiteSpace();
	}

	macro_.Add(item);

	return b;
}


bool GerberMacroParser::LineCenter() {
	constexpr int modifier_count = 6;
	auto item = std::make_shared<PrimitiveItem>();
	item->type_ = PrimitiveItem::kLineCenter;
	item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);

	gerber_file_->SkipWhiteSpace();

	int j = 0;
	bool b = true;
	while (b && !gerber_file_->EndOfFile() && j < modifier_count) {
		if (gerber_file_->GetChar() == ',') {
			auto tmp = Modifier();
			item->modifiers_[j++] = tmp;
			b = bool(tmp);
		}
		else {
			break;
		}

		gerber_file_->SkipWhiteSpace();
	}

	macro_.Add(item);

	return b;
}


bool GerberMacroParser::LineLowerLeft() {
	constexpr int modifier_count = 6;
	auto item = std::make_shared<PrimitiveItem>();
	item->type_ = PrimitiveItem::kLineLowerLeft;
	item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);

	gerber_file_->SkipWhiteSpace();

	int j = 0;
	bool b = true;
	while (b && !gerber_file_->EndOfFile() && j < modifier_count) {
		if (gerber_file_->GetChar() == ',') {
			auto tmp = Modifier();
			item->modifiers_[j++] = tmp;
			b = bool(tmp);
		}
		else {
			break;
		}

		gerber_file_->SkipWhiteSpace();
	}

	macro_.Add(item);

	return b;
}


bool GerberMacroParser::Outline() {
	if (gerber_file_->EndOfFile() || gerber_file_->GetChar() != ',') {
		return false;
	}

	int exposure = 0;
	auto ret = gerber_file_->GetInteger(exposure);
	if (!ret) {
		LOG(ERROR) << "Error: Integer exposure expected";
		return false;
	}

	if (gerber_file_->EndOfFile() || gerber_file_->GetChar() != ',') {
		return false;
	}

	int n = 0;
	ret = gerber_file_->GetInteger(n);
	if (!ret) {
		LOG(ERROR) << "Error: Integer number of outline points expected";
		return false;
	}

	const auto modifier_count = 5 + 2 * n;

	auto item = std::make_shared<PrimitiveItem>();
	item->type_ = PrimitiveItem::kOutline;
	item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);

	item->modifiers_[0] = std::make_shared<OperatorItem>(nullptr, nullptr, OperatorItem::kLiteral);
	item->modifiers_[0]->Value = macro_.exposure_;

	item->modifiers_[1] = std::make_shared<OperatorItem>(nullptr, nullptr, OperatorItem::kLiteral);
	item->modifiers_[1]->Value = n;

	gerber_file_->SkipWhiteSpace();

	auto j = 2;
	bool b = true;
	while (b && !gerber_file_->EndOfFile() && j < modifier_count) {
		if (gerber_file_->GetChar() == ',') {
			auto tmp = Modifier();
			item->modifiers_[j++] = tmp;
			b = bool(tmp);
		}
		else {
			break;
		}

		gerber_file_->SkipWhiteSpace();
	}

	macro_.Add(item);

	return b;
}


bool GerberMacroParser::Polygon() {
	constexpr int modifier_count = 6;
	auto item = std::make_shared<PrimitiveItem>();
	item->type_ = PrimitiveItem::kPolygon;
	item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);

	gerber_file_->SkipWhiteSpace();

	int j = 0;
	bool b = true;
	while (b && !gerber_file_->EndOfFile() && j < modifier_count) {
		if (gerber_file_->GetChar() == ',') {
			auto tmp = Modifier();
			item->modifiers_[j++] = tmp;
			b = bool(tmp);
		}
		else {
			break;
		}

		gerber_file_->SkipWhiteSpace();
	}

	macro_.Add(item);
	return b;
}

bool GerberMacroParser::Moire() {
	constexpr int modifier_count = 9;
	auto item = std::make_shared<PrimitiveItem>();
	item->type_ = PrimitiveItem::kMoire;
	item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);

	gerber_file_->SkipWhiteSpace();

	int j = 0;
	bool b = true;
	while (b && !gerber_file_->EndOfFile() && j < modifier_count) {
		if (gerber_file_->GetChar() == ',') {
			auto tmp = Modifier();
			item->modifiers_[j++] = tmp;
			b = bool(tmp);
		}
		else {
			break;
		}

		gerber_file_->SkipWhiteSpace();
	}

	macro_.Add(item);
	return b;
}


bool GerberMacroParser::Thermal() {
	constexpr int modifier_count = 6;
	auto item = std::make_shared<PrimitiveItem>();
	item->type_ = PrimitiveItem::kThermal;
	item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);

	gerber_file_->SkipWhiteSpace();

	int j = 0;
	bool b = true;
	while (b && !gerber_file_->EndOfFile() && j < modifier_count) {
		if (gerber_file_->GetChar() == ',') {
			auto tmp = Modifier();
			item->modifiers_[j++] = tmp;
			b = bool(tmp);
		}
		else {
			break;
		}

		gerber_file_->SkipWhiteSpace();
	}

	macro_.Add(item);

	return b;
}

bool GerberMacroParser::Assignment() {
	int index = 0;
	auto ret = gerber_file_->GetInteger(index);
	if (!ret) {
		return false;
	}

	gerber_file_->SkipWhiteSpace();

	if (gerber_file_->pointer_ > gerber_file_->buffer_.size()) {
		return false;
	}

	if (gerber_file_->GetChar() != '=') {
		return false;
	}

	auto item = std::make_shared<PrimitiveItem>();
	item->type_ = PrimitiveItem::kAssignment;
	item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(1);
	item->modifiers_[0] = Modifier();
	item->index_ = index;

	if (!item->modifiers_[0]) {
		return false;
	}

	gerber_file_->SkipWhiteSpace();
	macro_.Add(item);
	return true;
}

// Term {("+" | "-") Term};
std::shared_ptr<OperatorItem> GerberMacroParser::Modifier() {
	gerber_file_->SkipWhiteSpace();

	auto left = Term();
	if (left == nullptr) {
		return nullptr;
	}

	while (!gerber_file_->EndOfFile()) {
		OperatorItem::Type opera = OperatorItem::kAdd;
		switch (gerber_file_->PeekChar()) {
		case '+':
			opera = OperatorItem::kAdd;
			break;

		case '-':
			opera = OperatorItem::kSubtract;
			break;

		default:
			return left;
		}
		gerber_file_->pointer_++;

		auto root = std::make_shared<OperatorItem>(left, Term(), opera);
		left = root;
	}

	return nullptr;
}

// Factor {("x" | "X" | "/") Factor};
std::shared_ptr<OperatorItem> GerberMacroParser::Term() {
	gerber_file_->SkipWhiteSpace();

	auto left = Factor();
	if (left == nullptr) {
		return nullptr;
	}

	while (!gerber_file_->EndOfFile()) {
		OperatorItem::Type opera = OperatorItem::kAdd;
		switch (gerber_file_->PeekChar()) {
		case 'x':
		case 'X':
			opera = OperatorItem::kMultiply;
			break;

		case '/':
			opera = OperatorItem::kDivide;
			break;

		default:
			return left;
		}
		gerber_file_->pointer_++;

		left = std::make_shared<OperatorItem>(left, Factor(), opera);
	}

	return nullptr;
}

// ["+" | "-"] (("(" Modifier ")") | Variable | Float)
std::shared_ptr<OperatorItem> GerberMacroParser::Factor() {
	if (gerber_file_->EndOfFile()) {
		return nullptr;
	}

	bool negative = false;
	if (gerber_file_->PeekChar() == '-') {
		negative = true;
		gerber_file_->pointer_++;
		gerber_file_->SkipWhiteSpace();
	}
	else if (gerber_file_->PeekChar() == '+') {
		gerber_file_->pointer_++;
		gerber_file_->SkipWhiteSpace();
	}

	std::shared_ptr<OperatorItem> item = nullptr;
	if (gerber_file_->PeekChar() == '(') {
		gerber_file_->pointer_++;

		item = Modifier();
		if (item == nullptr) {
			LOG(ERROR) << "Error: Expression expected";
			return nullptr;
		}

		gerber_file_->SkipWhiteSpace();

		if (gerber_file_->PeekChar() != ')') {
			LOG(ERROR) << "Error: ')' expected";
			return nullptr;
		}
		gerber_file_->pointer_++;
	}
	else {
		item = Variable();
		if (item == nullptr) {
			double d = 0.0;
			auto ret = gerber_file_->GetFloat(d);
			if (!ret) {
				LOG(ERROR) << "Error: Float expected";
				return nullptr;
			}

			item = std::make_shared<OperatorItem>(nullptr, nullptr, OperatorItem::kLiteral);
			item->Value = d;
		}
	}

	if (item != nullptr && negative) {
		auto left = std::make_shared<OperatorItem>(nullptr, nullptr, OperatorItem::kLiteral);
		auto root = std::make_shared<OperatorItem>(left, item, OperatorItem::kMultiply);
		root->Left()->Value = -1;
		item = root;
	}

	return item;
}

// "$" Integer
std::shared_ptr<OperatorItem> GerberMacroParser::Variable() {
	if (gerber_file_->EndOfFile() || gerber_file_->PeekChar() != '$') {
		return nullptr;
	}
	gerber_file_->pointer_++;

	int i = 0;
	auto ret = gerber_file_->GetInteger(i);
	if (!ret) {
		gerber_file_->pointer_--;
		return nullptr;
	}

	auto item = std::make_shared<OperatorItem>(nullptr, nullptr, OperatorItem::kVariable);
	item->Index = i;

	return item;
}
