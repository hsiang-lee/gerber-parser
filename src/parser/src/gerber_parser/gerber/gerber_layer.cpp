#include "gerber_layer.h"
#include "aperture/aperture.h"
#include "engine/engine.h"
#include "gerber/gerber_primitive.h"
#include "gerber/stroke2fill_converter.h"

constexpr double kPi = 3.141592653589793238463;
constexpr double kVeryLarge = 1e5;
constexpr double kVeryLittle = -1e5;

GerberLayer::GerberLayer(GerberLayer *previous_layer, UnitType::Type unit)
    : bound_box_(kVeryLarge, kVeryLittle, kVeryLittle, kVeryLarge) {
  count_x_ = count_y_ = 1;
  step_x_ = step_y_ = 0.0;

  negative_ = false;
  unit_ = unit;

  if (previous_layer) {
    SetName(previous_layer->name_);

    count_x_ = previous_layer->count_x_;
    count_y_ = previous_layer->count_y_;
    step_x_ = previous_layer->step_x_;
    step_y_ = previous_layer->step_y_;

    negative_ = previous_layer->negative_;
  }
}

GerberLayer::~GerberLayer() {
  for (auto each : primitives_) {
    delete each;
  }
}

bool GerberLayer::IsCopyLayer() const { return count_x_ > 1 || count_y_ > 1; }

double GerberLayer::GetRight() const {
  if (count_x_ > 1) {
    return bound_box_.Left() + step_x_ * (count_x_ - 1) + bound_box_.Right() -
           bound_box_.Left();
  }

  return bound_box_.Right();
}

double GerberLayer::GetTop() const {
  if (count_y_ > 1) {
    return bound_box_.Bottom() + step_y_ * (count_y_ - 1) + bound_box_.Top() -
           bound_box_.Bottom();
  }

  return bound_box_.Top();
}

UnitType::Type GerberLayer::GetUnit() const { return unit_; }

void GerberLayer::SetUnit(UnitType::Type type) { unit_ = type; }

std::string GerberLayer::GetName() const { return name_; }

void GerberLayer::SetName(const std::string &name) { name_ = name; }

bool GerberLayer::IsNegative() const { return negative_; }

void GerberLayer::SetNegative(bool negative) { negative_ = negative; }

BoundBox GerberLayer::GetBBox() const { return bound_box_; }

BoundBox GerberLayer::FindPrimitive(const std::pair<double, double> &pt) {
  for (auto &each : primitives_) {
    if (each->type_ != Primitive::cFlash) {
      continue;
    }

    auto flash = dynamic_cast<Flash *>(each);
    auto aperture = flash->GetAperture();
    if (aperture->Type() != Aperture::tCircle) {
      continue;
    }

    auto box = flash->BBox();
    if (box.Left() < pt.first && box.Right() > pt.first && box.Top() > pt.second && box.Bottom() < pt.second) {
        return box;
    }
  }

  return BoundBox();
}

void GerberLayer::Add(Primitive *primitive) {
  primitives_.emplace_back(primitive);
}

std::vector<Primitive *> GerberLayer::Primitives() const { return primitives_; }

void GerberLayer::ConvertStrokesToFills() {
  StrokesToFillsConverter converter(*this);
  converter.ExtractSegments();
  converter.JoinSegments();
  converter.AddSegments();
}

int GerberLayer::Draw(Engine *engine) {
  if (engine->convert_strokes2fills_) {
    ConvertStrokesToFills();
  }

  for (const auto &each : primitives_) {
    if (auto ret = each->Draw(*engine)) {
      return ret;
    }
  }

  return 0;
}
