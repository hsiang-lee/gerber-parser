#include "gerber.h"
#include "gerber/gerber_level.h"

constexpr double kVeryLarge = 1e5;
constexpr double kVeryLittle = -1e5;

double UnitType::Get_mm(double number, Type unit) {
	if (unit == guInches) {
		number *= 25.4;// NOLINT
	}

	return number;
}

bool Gerber::IsNegative() const
{
	return negative_;
}

BoundBox Gerber::GetBBox() const {
	if (levels_.empty()) {
		return { 0.0, 0.0, 0.0, 0.0 };
	}

	BoundBox bound_box(kVeryLarge, kVeryLittle, kVeryLittle, kVeryLarge);
	std::for_each(levels_.begin(), levels_.end(), [&bound_box](const std::shared_ptr<GerberLevel>& level) {
		bound_box.UpdateBox(level->GetBoundBox().Left(), level->GetRight(), level->GetTop(), level->GetBoundBox().Bottom());
	}
	);

	return bound_box;
}

UnitType::Type Gerber::Unit() const
{
	return unit_;
}

std::string Gerber::Name() const
{
	return name_;
}

std::vector<std::shared_ptr<GerberLevel>> Gerber::Levels() const
{
	return levels_;
}
