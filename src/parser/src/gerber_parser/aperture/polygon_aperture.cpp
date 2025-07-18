#include "polygon_aperture.h"
#include "gerber/gerber_primitive.h"
#include <cmath>

constexpr double kPi = 3.141592653589793238463;
constexpr double kTwo = 2.0;

PolygonAperture::PolygonAperture(int code, double w, int n, double a, double x,
                                 double y)
    : Aperture(code) {
  dimension_x_ = w;
  side_count_ = n;
  rotation_ = a;

  if (x > 0.0) {
    if (y > 0.0) {
      HoleRectangle(x, y);
    } else {
      HoleCircle(x);
    }
  }
}

BoundBox PolygonAperture::BBox() const {
  return BoundBox(-dimension_x_ / kTwo, dimension_x_ / kTwo,
                  dimension_x_ / kTwo, -dimension_x_ / kTwo);
}

Aperture::ApertureType PolygonAperture::Type() const {
  return ApertureType::tPolygon;
}

void PolygonAperture::RenderAperture() {
  outline_ = new Outline();

  // Get Rotation in range [0; 360)
  while (rotation_ < 0.0) {
    rotation_ += 360;  // NOLINT
  }

  while (rotation_ >= 360.0) {  // NOLINT
    rotation_ -= 360;           // NOLINT
  }

  auto r = dimension_x_ / kTwo;
  auto da = kTwo * kPi / side_count_;
  auto rot = rotation_ * kPi / 180.0;  // NOLINT
  auto lim = kTwo * kPi - da / kTwo;

  auto item =
      new Composite::BeginLineItem(r * std::cos(rot), r * std::sin(rot));
  outline_->items_.emplace_back(item);

  for (double a = da; a < lim; a += da) {
    auto item =
        new Composite::LineItem(r * std::cos(a + rot), r * std::sin(a + rot));
    outline_->items_.emplace_back(item);
  }

  primitives_.emplace_back(outline_);
  outline_ = nullptr;

  RenderHole();
}

bool PolygonAperture::SolidCircle() { return false; }

bool PolygonAperture::SolidRectangle() { return false; }
