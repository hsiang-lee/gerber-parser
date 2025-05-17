#include "gerber_primitive.h"
#include "aperture/aperture.h"
#include "engine/engine.h"
#include "log/log.h"

#include <cmath>

Primitive::Primitive(Primitive::Type type) { type_ = type; }

Primitive::~Primitive() {}

Single::Single(Primitive::Type type) : Primitive(type) {
  start_ = std::make_pair(0.0, 0.0);
  end_.first = end_.second = 0.0;
}

Single::~Single() {}

Flash::Flash(double x, double y, Aperture *aperture)
    : Single(cFlash), aperture_(aperture) {
  start_ = std::make_pair(x, y);
  aperture_ = aperture;
  aperture->Visit();
}

int Flash::Draw(Engine &engine) {
  engine.DrawAperture(aperture_, start_);
  return 0;
}

BoundBox Flash::BBox() const {
  auto box = aperture_->BBox();
  return BoundBox(start_.first + box.Left(), start_.first + box.Right(),
                  start_.second + box.Top(), start_.second + box.Bottom());
}

Aperture *Flash::GetAperture() { return aperture_; }

Rectangle::Rectangle(double w, double h)
    : Single(cRectangle), width_(w), height_(h) {}

int Rectangle::Draw(Engine &engine) {
  const auto x = start_.first;
  const auto y = start_.second;
  const auto w = width_;
  const auto h = height_;
  engine.AddRect(x, y, w, h);
  return 0;
}

BoundBox Rectangle::BBox() const {
  return BoundBox(start_.first, start_.first + width_, start_.second + height_,
                  start_.second);
}

Circle::Circle(double x, double y, double diameter, bool fill)
    : Single(cCircle), fill_(fill), diameter_(diameter) {
  start_ = std::make_pair(x, y);
}

int Circle::Draw(Engine &engine) {
  const auto x = start_.first;
  const auto y = start_.second;
  const auto r = diameter_ / 2.0;
  engine.AddCircle(x, y, r);
  return 0;
}

BoundBox Circle::BBox() const {
  return BoundBox(
      start_.first - diameter_ / 2.0, start_.first + diameter_ / 2.0,
      start_.second + diameter_ / 2.0, start_.second - diameter_ / 2.0);
}

Composite::Composite(Primitive::Type type) : Primitive(type) {}

Composite::~Composite() {
  for (auto each : items_) {
    delete each;
  }
}

Stroke::Stroke(Aperture *aperture) : Composite(cStroke), aperture_(aperture) {}

int Stroke::Draw(Engine &engine) {
  engine.BeginDrawStroke(aperture_);

  for (auto &each : items_) {
    switch (each->type_) {
      case Primitive::cBeginLine:
        if (aperture_->SolidCircle()) {
          engine.MoveTo(
              std::make_pair(each->start_.first, each->start_.second));
        } else if (aperture_->SolidRectangle()) {
          engine.CachePoint(each->start_);
        } else {
          Log::log_ << "Error: Only solid circular or rectangular apertures "
                       "can be used for paths";
          return 1;
        }
        break;

      case Primitive::cLine:
        if (aperture_->SolidCircle()) {
          engine.LineTo(
              std::make_pair(each->start_.first, each->start_.second));
        } else if (aperture_->SolidRectangle()) {
          DrawRectLine(engine, engine.CachedPoint().first,
                       engine.CachedPoint().second, each->start_.first,
                       each->start_.second, aperture_->BBox().Width(),
                       aperture_->BBox().Height());
          engine.CachePoint(each->start_);
        } else {
          Log::log_ << "Error: Only solid circular or rectangular apertures "
                       "can be used for paths";
          return 2;
        }
        break;

      default:
        each->Draw(engine);
        break;
    }
  }

  engine.EndDrawStroke();
  return 0;
}

BoundBox Stroke::BBox() const { return BBox(); }

void Stroke::DrawRectLine(Engine &engine, double x1, double y1, double x2,
                          double y2, double w, double h) {
  w /= 2.0;
  h /= 2.0;

  std::vector<std::pair<double, double>> pts;

  if (x2 > x1) {
    if (y2 > y1) {
      pts.push_back({x1 - w, y1 - h});
      pts.push_back({x1 + w, y1 - h});
      pts.push_back({x2 + w, y2 - h});
      pts.push_back({x2 + w, y2 + h});
      pts.push_back({x2 - w, y2 + h});
      pts.push_back({x1 - w, y1 + h});
    } else if (y1 > y2) {
      pts.push_back({x1 - w, y1 - h});
      pts.push_back({x2 - w, y2 - h});
      pts.push_back({x2 + w, y2 - h});
      pts.push_back({x2 + w, y2 + h});
      pts.push_back({x1 + w, y1 + h});
      pts.push_back({x1 - w, y1 + h});
    } else {  // y1 = y2
      pts.push_back({x1 - w, y1 - h});
      pts.push_back({x2 + w, y2 - h});
      pts.push_back({x2 + w, y2 + h});
      pts.push_back({x1 - w, y1 + h});
    }
  } else if (x1 > x2) {
    if (y2 > y1) {
      pts.push_back({x2 - w, y2 - h});
      pts.push_back({x1 - w, y1 - h});
      pts.push_back({x1 + w, y1 - h});
      pts.push_back({x1 + w, y1 + h});
      pts.push_back({x2 + w, y2 + h});
      pts.push_back({x2 - w, y2 + h});
    } else if (y1 > y2) {
      pts.push_back({x2 - w, y2 - h});
      pts.push_back({x2 + w, y2 - h});
      pts.push_back({x1 + w, y1 - h});
      pts.push_back({x1 + w, y1 + h});
      pts.push_back({x1 - w, y1 + h});
      pts.push_back({x2 - w, y2 + h});
    } else {  // y1 = y2
      pts.push_back({x2 - w, y2 - h});
      pts.push_back({x1 + w, y1 - h});
      pts.push_back({x1 + w, y1 + h});
      pts.push_back({x2 - w, y2 + h});
    }
  } else {  // x1 = x2
    if (y2 > y1) {
      pts.push_back({x1 - w, y1 - h});
      pts.push_back({x1 + w, y1 - h});
      pts.push_back({x2 + w, y2 + h});
      pts.push_back({x2 - w, y2 + h});
    } else {  // y1 >= y2
      pts.push_back({x2 - w, y2 - h});
      pts.push_back({x2 + w, y2 - h});
      pts.push_back({x1 + w, y1 + h});
      pts.push_back({x1 - w, y1 + h});
    }
  }

  if (!pts.empty()) {
    engine.MoveTo(pts[0]);
    for (int i = 0; i < pts.size(); ++i) {
      engine.LineTo(pts[i]);
    }
  }
}

Outline::Outline(bool fill) : Composite(Primitive::cOutline), fill_(fill) {}

int Outline::Draw(Engine &engine) {
  engine.BeginDrawOutline();

  for (auto &each : items_) {
    switch (each->type_) {
      case Primitive::cBeginLine:
        engine.MoveTo(std::make_pair(each->start_.first, each->start_.second));
        break;

      case Primitive::cArc:
        each->Draw(engine);
        break;

      case Primitive::cLine:
        engine.LineTo(std::make_pair(each->start_.first, each->start_.second));
        break;

      default:
        break;
    }
  }

  engine.EndDrawOutline();
  return 0;
}

BoundBox Outline::BBox() const {
  double x = 0.0;
  double y = 0.0;
  BoundBox bound_box;
  for (const auto &each : items_) {
    bound_box.Update(each->BBox(x, y));
  }

  return bound_box;
}

Item::Item(Primitive::Type type) : type_(type), angle_(0) {}

Item::~Item() {}

Composite::BeginLineItem::BeginLineItem(double x, double y)
    : Item(Primitive::cBeginLine) {
  start_ = std::make_pair(x, y);
}

int Composite::BeginLineItem::Draw(Engine &engine) { return 0; }

BoundBox Composite::BeginLineItem::BBox(double &x, double &y) const {
  x = start_.first;
  y = start_.second;
  return BoundBox(start_.first, start_.first, start_.second, start_.second);
}

Composite::LineItem::LineItem(double x, double y) : Item(Primitive::cLine) {
  start_ = std::make_pair(x, y);
}

int Composite::LineItem::Draw(Engine &engine) { return 0; }

BoundBox Composite::LineItem::BBox(double &x, double &y) const {
  x = start_.first;
  y = start_.second;
  return BoundBox(start_.first, start_.first, start_.second, start_.second);
}

Composite::ArcItem::ArcItem(double x, double y, double angle)
    : Item(Primitive::cArc) {
  start_ = std::make_pair(x, y);
  angle_ = angle;
}

int Composite::ArcItem::Draw(Engine &engine) {
  const auto x = start_.first;
  const auto y = start_.second;
  DrawArcScaled(engine, x, y, angle_);
  return 0;
}

BoundBox Composite::ArcItem::BBox(double &x, double &y) const {
  constexpr double kPi = 3.141592653589793238463;
  auto c = x - start_.first;
  const auto d = y - start_.second;
  const auto e = std::sqrt(c * c + d * d);
  c = std::atan2(d, c) + angle_ * kPi / 180.0;

  x = start_.first + e * std::cos(c);
  y = start_.second + e * std::sin(c);

  return BoundBox(start_.first - e, start_.first + e, start_.second + e,
                  start_.second - e);
}

void Composite::ArcItem::DrawArcScaled(Engine &engine, double x, double y,
                                      double degree) {
  const auto cur_pos = engine.CurrentPos();
  if (std::fabs(cur_pos.first - x) < 1e-15 &&
      std::fabs(cur_pos.second - y) < 1e-15) {
    return;
  }

  if (std::fabs(degree) >
      45.001) {  // Slightly large so that the circle has 4 segments
    degree /= 2.0;
    DrawArcScaled(engine, x, y, degree);
    DrawArcScaled(engine, x, y, degree);
    return;
  }

  const auto delta_x = cur_pos.first - x;
  const auto delta_y = cur_pos.second - y;
  double radius = std::sqrt(delta_y * delta_y + delta_x * delta_x);

  double t, t1, t2;
  double e, de, dt, rb, xb, yb;

  constexpr double kPi = 3.141592653589793;
  double b = degree * kPi / 180 / 2.0;

  double a1 = std::atan2(delta_y, delta_x);

  double a4 = a1 + degree * kPi / 180;
  double x4 = radius * std::cos(a4);
  double y4 = radius * std::sin(a4);

  double a5 = a1 + b;
  double x5 = radius * std::cos(a5);
  double y5 = radius * std::sin(a5);

  b = std::cos(b);
  double x6 = x5 / b;
  double y6 = y5 / b;

  // Best extimate
  t1 = 6 * x6 - 3 * delta_x - 3 * (x4);
  t2 = 6 * y6 - 3 * delta_y - 3 * (y4);
  if (std::fabs(t1) > std::fabs(t2)) {
    t = (8 * x5 - 4 * delta_x - 4 * (x4)) / t1;
  } else {
    t = (8 * y5 - 4 * delta_y - 4 * (y4)) / t2;
  }

  // Newton-Raphson to fit B(1/3) to the circle
  radius = radius * radius;

  dt = 1e-3;
  t2 = 1.0 / 3.0;  // Fitting B(1/3) to the circle is close enough to the
  t1 = 1.0 - t2;   // optimum solution.

  double x2 = (1.0 - t) * delta_x + t * x6;
  double y2 = (1.0 - t) * delta_y + t * y6;
  double x3 = (1.0 - t) * (x4) + t * x6;
  double y3 = (1.0 - t) * (y4) + t * y6;
  xb = t1 * t1 * t1 * delta_x + 3.0 * t1 * t1 * t2 * (x2) +
       3.0 * t1 * t2 * t2 * (x3) + t2 * t2 * t2 * (x4);
  yb = t1 * t1 * t1 * delta_y + 3.0 * t1 * t1 * t2 * (y2) +
       3.0 * t1 * t2 * t2 * (y3) + t2 * t2 * t2 * (y4);
  rb = xb * xb + yb * yb;
  e = rb - radius;
  while (e > 1e-12) {
    x2 = (1.0 - (t + dt)) * delta_x + (t + dt) * x6;
    y2 = (1.0 - (t + dt)) * delta_y + (t + dt) * y6;
    x3 = (1.0 - (t + dt)) * (x4) + (t + dt) * x6;
    y3 = (1.0 - (t + dt)) * (y4) + (t + dt) * y6;
    xb = t1 * t1 * t1 * delta_x + 3.0 * t1 * t1 * t2 * (x2) +
         3.0 * t1 * t2 * t2 * (x3) + t2 * t2 * t2 * (x4);
    yb = t1 * t1 * t1 * delta_y + 3.0 * t1 * t1 * t2 * (y2) +
         3.0 * t1 * t2 * t2 * (y3) + t2 * t2 * t2 * (y4);
    rb = xb * xb + yb * yb;
    de = (rb - radius - e) / dt;

    t -= e / de;  // Newton-Raphson

    x2 = (1.0 - t) * delta_x + t * x6;
    y2 = (1.0 - t) * delta_y + t * y6;
    x3 = (1.0 - t) * (x4) + t * x6;
    y3 = (1.0 - t) * (y4) + t * y6;
    xb = t1 * t1 * t1 * delta_x + 3.0 * t1 * t1 * t2 * (x2) +
         3.0 * t1 * t2 * t2 * (x3) + t2 * t2 * t2 * (x4);
    yb = t1 * t1 * t1 * delta_y + 3.0 * t1 * t1 * t2 * (y2) +
         3.0 * t1 * t2 * t2 * (y3) + t2 * t2 * t2 * (y4);
    rb = xb * xb + yb * yb;
    e = rb - radius;
  }

  engine.CubicTo(std::make_pair(x2 + x, y2 + y), std::make_pair(x3 + x, y3 + y),
                 std::make_pair(x4 + x, y4 + y));
}
