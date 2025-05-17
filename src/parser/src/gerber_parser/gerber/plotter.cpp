#include "plotter.h"
#include "gerber/gerber_layer.h"
#include "gerber/gerber_primitive.h"
#include "gerber_parser/aperture/aperture.h"
#include "log/log.h"

#include <array>
#include <cmath>

constexpr double kPi = 3.141592653589793238463;

Plotter::Plotter(GerberLayer &layer, std::shared_ptr<Plotter> old_plotter)
    : layer_(layer) {
  x_ = y_ = i_ = j_ = 0.0;
  multi_quadrant_ = false;

  exposure_ = geOff;
  interpolation_ = giLinear;

  if (old_plotter) {
    exposure_ = old_plotter->exposure_;
    multi_quadrant_ = old_plotter->multi_quadrant_;
    interpolation_ = old_plotter->interpolation_;
    current_aperture_ = old_plotter->current_aperture_;

    pre_x_ = old_plotter->pre_x_;
    pre_y_ = old_plotter->pre_y_;
    x_ = old_plotter->x_;
    y_ = old_plotter->y_;

    drawing_line_ = old_plotter->drawing_line_;
  }
}

void Plotter::OutlineBegin(unsigned line_number) {
  exposure_ = geOff;
  Move(line_number);
  outline_ = new Outline();
}

void Plotter::OutlineEnd(unsigned line_number) {
  exposure_ = geOff;
  Move(line_number);
  layer_.Add(outline_);
  outline_ = nullptr;
}

void Plotter::Do(unsigned line_number) {
  switch (exposure_) {
    case geOn:
      DrawLine();
      break;

    case geOff:
      Move(line_number);
      break;

    case geFlash:
      Move(line_number);
      DrawFlash();
      exposure_ = geOff;
      break;

    default:
      break;
  }

  i_ = j_ = 0.0;
}

void Plotter::ApertureSelect(Aperture *aperture, unsigned line_number) {
  exposure_ = geOff;
  Move(line_number);
  current_aperture_ = aperture;
}

void Plotter::Move(unsigned line_number) {
  if (drawing_line_) {
    if (outline_) {
      if (first_x_ != pre_x_ || first_y_ != pre_y_) {
        Log::log_ << "Line " << line_number
                  << " - Warning: Deprecated feature: Open contours";
      }
    } else {
      layer_.Add(stroke_);
      stroke_ = nullptr;
    }
  }

  drawing_line_ = false;

  first_x_ = UnitType::Get_mm(x_, layer_.unit_);
  first_y_ = UnitType::Get_mm(y_, layer_.unit_);
  pre_x_ = first_x_;
  pre_y_ = first_y_;
}

void Plotter::DrawLine() {
  if (!drawing_line_) {
    if (current_aperture_ && !outline_) {
      const auto bound_box = current_aperture_->BBox();
      layer_.bound_box_.Update(
          pre_x_ + bound_box.Left(), pre_x_ + bound_box.Right(),
          pre_y_ + bound_box.Top(), pre_y_ + bound_box.Bottom());
    } else {
      layer_.bound_box_.Update(pre_x_, pre_x_, pre_y_, pre_y_);
    }

    auto tmp = new Composite::BeginLineItem(pre_x_, pre_y_);

    if (outline_) {
      outline_->items_.emplace_back(tmp);
    } else {
      stroke_ = new Stroke(current_aperture_);
      stroke_->items_.emplace_back(tmp);
    }

    tmp->start_ = std::make_pair(pre_x_, pre_y_);
    tmp->end_ = tmp->start_;
  } else {
    if (pre_x_ == UnitType::Get_mm(x_, layer_.unit_) &&
        pre_y_ == UnitType::Get_mm(y_, layer_.unit_) && i_ == 0.0 &&
        j_ == 0.0) {
      return;
    }
  }
  drawing_line_ = true;

  switch (interpolation_) {
    case giLinear: {
      auto tmp = new Composite::LineItem(UnitType::Get_mm(x_, layer_.unit_),
                              UnitType::Get_mm(y_, layer_.unit_));

      if (outline_) {
        outline_->items_.emplace_back(tmp);
      } else {
        stroke_->items_.emplace_back(tmp);
      }

      tmp->start_ = std::make_pair(UnitType::Get_mm(x_, layer_.unit_),
                                   UnitType::Get_mm(y_, layer_.unit_));
      tmp->end_ = tmp->start_;
      break;
    }
    case giLinear10X: {
      auto tmp =
          new Composite::LineItem(UnitType::Get_mm(x_, layer_.unit_) * 10.0,
                              UnitType::Get_mm(y_, layer_.unit_) * 10.0);

      if (outline_) {
        outline_->items_.emplace_back(tmp);
      } else {
        stroke_->items_.emplace_back(tmp);
      }

      tmp->start_ = std::make_pair(UnitType::Get_mm(x_, layer_.unit_) * 10.0,
                                   UnitType::Get_mm(y_, layer_.unit_) * 10.0);
      tmp->end_ = tmp->start_;
      break;
    }
    case giLinear0_1X: {
      auto tmp = new Composite::LineItem(UnitType::Get_mm(x_, layer_.unit_) * 0.1,
                              UnitType::Get_mm(y_, layer_.unit_) * 0.1);

      if (outline_) {
        outline_->items_.emplace_back(tmp);
      } else {
        stroke_->items_.emplace_back(tmp);
      }

      tmp->start_ = std::make_pair(UnitType::Get_mm(x_, layer_.unit_) * 0.1,
                                   UnitType::Get_mm(y_, layer_.unit_) * 0.1);
      tmp->end_ = tmp->start_;
      break;
    }
    case giLinear0_01X: {
      auto tmp =
          new Composite::LineItem(UnitType::Get_mm(x_, layer_.unit_) * 0.01,
                              UnitType::Get_mm(y_, layer_.unit_) * 0.01);

      if (outline_) {
        outline_->items_.emplace_back(tmp);
      } else {
        stroke_->items_.emplace_back(tmp);
      }

      tmp->start_ = std::make_pair(UnitType::Get_mm(x_, layer_.unit_) * 0.01,
                                   UnitType::Get_mm(y_, layer_.unit_) * 0.01);
      tmp->end_ = tmp->start_;
      break;
    }
    case giClockwiseCircular:
    case giCounterclockwiseCircular: {
      DrawArc();
      break;
    }
    default:
      break;
  }

  pre_x_ = UnitType::Get_mm(x_, layer_.unit_);
  pre_y_ = UnitType::Get_mm(y_, layer_.unit_);

  if (current_aperture_ && !outline_) {
    const auto bound_box = current_aperture_->BBox();
    layer_.bound_box_.Update(
        pre_x_ + bound_box.Left(), pre_x_ + bound_box.Right(),
        pre_y_ + bound_box.Top(), pre_y_ + bound_box.Bottom());
  } else {
    layer_.bound_box_.Update(pre_x_, pre_x_, pre_y_, pre_y_);
  }
}

void Plotter::DrawArc() {
  double x1 = 0.0;
  double y1 = 0.0;  // Start, relative to center
  double x2 = 0.0;
  double y2 = 0.0;  // End, relative to center
  double x3 = 0.0;
  double y3 = 0.0;  // Center

  if (multi_quadrant_) {
    x1 = -UnitType::Get_mm(i_, layer_.unit_);
    y1 = -UnitType::Get_mm(j_, layer_.unit_);

    x3 = pre_x_ + UnitType::Get_mm(i_, layer_.unit_);
    y3 = pre_y_ + UnitType::Get_mm(j_, layer_.unit_);

    x2 = UnitType::Get_mm(x_, layer_.unit_) - x3;
    y2 = UnitType::Get_mm(y_, layer_.unit_) - y3;
  } else {
    std::array<double, 4> i{};
    std::array<double, 4> j{};

    i[0] = UnitType::Get_mm(i_, layer_.unit_);
    j[0] = UnitType::Get_mm(j_, layer_.unit_);
    i[1] = -i[0];
    j[1] = j[0];
    i[2] = -i[0];
    j[2] = -j[0];
    i[3] = i[0];
    j[3] = -j[0];

    // Find the best match for the centre coordinates
    // by minimising the error function.
    int T = 0;
    double Error = INFINITY;
    for (int t = 0; t < 4; ++t) {
      auto a = GetAngle(-i[t], -j[t],
                        UnitType::Get_mm(x_, layer_.unit_) - pre_x_ - i[t],
                        UnitType::Get_mm(y_, layer_.unit_) - pre_y_ - j[t]);
      if (interpolation_ == giClockwiseCircular) {
        if (a > 0.0) {
          continue;
        }
      } else {  // CCW
        if (a < 0.0) {
          continue;
        }
      }

      x2 = UnitType::Get_mm(x_, layer_.unit_) - pre_x_ - i[t];
      y2 = UnitType::Get_mm(y_, layer_.unit_) - pre_y_ - j[t];
      const auto error =
          std::fabs((i[t] * i[t] + j[t] * j[t]) - (x2 * x2 + y2 * y2));
      if (error < Error) {
        T = t;
        Error = error;
      }
    }

    x3 = pre_x_ + i[T];
    y3 = pre_y_ + j[T];

    x1 = -i[T];
    y1 = -j[T];
    x2 = UnitType::Get_mm(x_, layer_.unit_) - x3;
    y2 = UnitType::Get_mm(y_, layer_.unit_) - y3;
  }

  const auto angle = GetAngle(x1, y1, x2, y2);
  auto tmp = new Composite::ArcItem(x3, y3, angle);
  if (outline_) {
    outline_->items_.emplace_back(tmp);
  } else {
    stroke_->items_.emplace_back(tmp);
  }

  tmp->end_.first = pre_x_ = UnitType::Get_mm(x_, layer_.unit_);
  tmp->end_.second = pre_y_ = UnitType::Get_mm(y_, layer_.unit_);

  auto right = x3 + x1;
  auto top = y3 + y1;
  auto left = right;
  auto bottom = top;

  const auto rad = std::sqrt(x1 * x1 + y1 * y1);  // Radius
  const auto start_angle = std::atan2(y1, x1);
  const auto end_angle = start_angle + angle * kPi / 180;
  if (0 > start_angle && 0 < end_angle) {
    right = x3 + rad;
  } else {
    right = x3 + rad * std::max(std::cos(start_angle), std::cos(end_angle));
  }

  if (kPi > start_angle && kPi < end_angle) {
    left = x3 - rad;
  } else {
    left = x3 + rad * std::min(std::cos(start_angle), std::cos(end_angle));
  }

  if (kPi / 2 > start_angle && kPi / 2 < end_angle) {
    top = y3 - rad;
  } else {
    top = y3 - rad * std::max(std::sin(start_angle), std::sin(end_angle));
  }

  if (3 * kPi / 2 > start_angle && 3 * kPi / 2 < end_angle) {
    bottom = x3 + rad;
  } else {
    bottom = x3 - rad * std::min(std::sin(start_angle), std::sin(end_angle));
  }

  if (current_aperture_ && !outline_) {
    const auto bound_box = current_aperture_->BBox();
    left += bound_box.Left();
    bottom += bound_box.Bottom();
    right += bound_box.Right();
    top += bound_box.Top();
  }

  layer_.bound_box_.Update(left, right, top, bottom);
}

void Plotter::DrawFlash() {
  layer_.Add(new Flash(pre_x_, pre_y_, current_aperture_));

  if (current_aperture_) {
    const auto bound_box = current_aperture_->BBox();
    layer_.bound_box_.Update(
        pre_x_ + bound_box.Left(), pre_x_ + bound_box.Right(),
        pre_y_ + bound_box.Top(), pre_y_ + bound_box.Bottom());
  }
}

double Plotter::GetAngle(double x1, double y1,  // Start, relative to center
                         double x2, double y2   // End, relative to center
) {
  double a =
      (std::atan2(y2, x2) - std::atan2(y1, x1)) * 180.0 / kPi;  // [-360; 360]

  if (interpolation_ == giClockwiseCircular) {  // CW
    while (a >= 0.0) {
      a -= 360.0;  // NOLINT
    }

    if (!multi_quadrant_ && a < -90.001) {  // NOLINT
      a += 360;                             // NOLINT
    }
  } else {  // CCW
    while (a <= 0.0) {
      a += 360.0;  // NOLINT
    }

    if (!multi_quadrant_ && a > 90.001) {  // NOLINT
      a -= 360;                            // NOLINT
    }
  }

  if (multi_quadrant_) {
    if (std::fabs(a) < 0.001) {  // NOLINT
      if (a >= 0.0) {            // NOLINT
        a += 360;                // NOLINT
      } else {
        a -= 360;  // NOLINT
      }
    }
  }

  return a;
}
