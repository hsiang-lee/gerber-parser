#include "gerber_macro.h"
#include "primitive_item.h"
#include "parser/macro_parser/gerber_macro_parser.h"

GerberMacro::GerberMacro(const std::string& name, const std::string& buffer, bool inches) : name_(name) {
	exposure_ = true;
	inches_ = inches;

	parser_ = std::make_unique<GerberMacroParser>(*this, buffer);
	valid_ = parser_->LoadMacro(inches);
}

GerberMacro::~GerberMacro() { // NOLINT
}

bool GerberMacro::Valid() const {
	return valid_;
}

std::string GerberMacro::Name() const {
	return name_;
}

void GerberMacro::Add(const std::shared_ptr<RenderCommand>& render) {
	render_commands_.push_back(render);
}

double GerberMacro::Get_mm(double number) const {
	if (inches_) {
		number *= 25.4; // NOLINT
	}

	return number;
}

std::vector<std::shared_ptr<RenderCommand>> GerberMacro::Render(const std::vector<double>& modifiers) {
	render_commands_.clear();
	exposure_ = true;
	modifiers_ = modifiers;

	for (const auto& each : primitives_) {
		if (!each->Render(*this)) {
			return {};
		}
	}

	return render_commands_;
}

void GerberMacro::Add(const std::shared_ptr<PrimitiveItem>& primitive) {
	if (!primitive) {
		return;
	}

	primitives_.push_back(primitive);
}
