#include "aperture.h"
#include "gerber/gerber_primitive.h"

constexpr double kTwo = 2.0;

Aperture::Aperture(int code) {
  code_ = code;
  dimension_x_ = -1.0;
  dimension_y_ = -1.0;
  hole_x_ = -1.0;
  hole_y_ = -1.0;
  rotation_ = 0.0;
}

Aperture::~Aperture() {
  for (auto each : primitives_) {
    delete each;
  }
}

void Aperture::HoleCircle(double d) {
  hole_x_ = d;
  hole_y_ = -1.0;
}

void Aperture::HoleRectangle(double w, double h) {
  hole_x_ = w;
  hole_y_ = h;
}

void Aperture::RenderHole() {
  if (hole_x_ <= 0.0) {
    return;
  }

  outline_ = new Outline();

  if (hole_y_ < 0.0) {
    outline_->items_.emplace_back(
        new Composite::BeginLineItem(hole_x_ / kTwo, 0.0));
    outline_->items_.emplace_back(new Composite::ArcItem(0.0, 0.0, -360.0));
  } else {
    outline_->items_.emplace_back(
        new Composite::BeginLineItem(hole_x_ / kTwo, -hole_y_ / kTwo));
    outline_->items_.emplace_back(
        new Composite::LineItem(-hole_x_ / kTwo, -hole_y_ / kTwo));
    outline_->items_.emplace_back(
        new Composite::LineItem(-hole_x_ / kTwo, hole_y_ / kTwo));
    outline_->items_.emplace_back(
        new Composite::LineItem(hole_x_ / kTwo, hole_y_ / kTwo));
  }

  primitives_.emplace_back(outline_);
  outline_ = nullptr;
}

std::vector<Primitive *> Aperture::Primitives() {
  if (primitives_.empty()) {
    RenderAperture();
  }

  return primitives_;
}

int Aperture::Code() const { return code_; }

int Aperture::Draw(Engine *engine) {
  Primitives();
  for (auto &item : primitives_) {
    item->Draw(*engine);
  }

  return 0;
}

void Aperture::Visit() { ++count_; }

uint16_t Aperture::GetCount() const { return count_; }
