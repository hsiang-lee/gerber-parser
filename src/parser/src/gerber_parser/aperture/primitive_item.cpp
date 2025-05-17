#include "primitive_item.h"
#include "gerber/gerber_primitive.h"
#include "gerber_macro.h"
#include "parser/macro_parser/operator_item.h"

#include <array>
#include <cmath>

PrimitiveItem::PrimitiveItem() {
  type_ = kEndOfFile;
  index_ = 0;
}

PrimitiveItem::~PrimitiveItem() {  // NOLINT
}

bool CirclePrimitiveItem::Render(GerberMacro &macro) {
  constexpr int modifier_cnt = 5;
  std::array<double, modifier_cnt> modifier{};

  for (int j = 0; j < modifier_cnt; j++) {
    if (j < modifiers_.size()) {
      modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
    } else {
      modifier[j] = 0.0;
    }
  }

  if (modifier[0] == 0.0) {
    macro.exposure_ = false;
  } else if (modifier[0] == 1.0) {
    macro.exposure_ = true;
  } else {
    macro.exposure_ = !macro.exposure_;
  }

  double d = modifier[1];
  double x = modifier[2];
  double y = modifier[3];
  double a = modifier[4] * kPi / 180.0;  // NOLINT
  double s = std::sin(a);
  double c = std::cos(a);

  macro.Add(new Circle(macro.Get_mm(c * x - s * y), macro.Get_mm(s * x + c * y),
                       macro.Get_mm(d), macro.exposure_));

  return true;
}

void PrimitiveItem::RenderLine(double x1, double y1, double x2, double y2,
                               double x3, double y3, double x4, double y4,
                               double x_r, double y_r, double angle,
                               GerberMacro &macro) {
  double r = 0.0;
  double a = 0.0;

  // Translate to center
  x1 -= x_r;
  y1 -= y_r;
  x2 -= x_r;
  y2 -= y_r;
  x3 -= x_r;
  y3 -= y_r;
  x4 -= x_r;
  y4 -= y_r;

  // Rotate the corners
  r = std::sqrt(y1 * y1 + x1 * x1);
  a = std::atan2(y1, x1) + angle;
  x1 = x_r + r * std::cos(a);
  y1 = y_r + r * std::sin(a);

  r = std::sqrt(y2 * y2 + x2 * x2);
  a = std::atan2(y2, x2) + angle;
  x2 = x_r + r * std::cos(a);
  y2 = y_r + r * std::sin(a);

  r = std::sqrt(y3 * y3 + x3 * x3);
  a = std::atan2(y3, x3) + angle;
  x3 = x_r + r * std::cos(a);
  y3 = y_r + r * std::sin(a);

  r = std::sqrt(y4 * y4 + x4 * x4);
  a = std::atan2(y4, x4) + angle;
  x4 = x_r + r * std::cos(a);
  y4 = y_r + r * std::sin(a);

  // Draw the line
  auto outline = new Outline(macro.exposure_);
  Item *item = new Composite::BeginLineItem(macro.Get_mm(x1), macro.Get_mm(y1));
  outline->items_.emplace_back(item);

  item = new Composite::LineItem(macro.Get_mm(x2), macro.Get_mm(y2));
  outline->items_.emplace_back(item);

  item = new Composite::LineItem(macro.Get_mm(x3), macro.Get_mm(y3));
  outline->items_.emplace_back(item);

  item = new Composite::LineItem(macro.Get_mm(x4), macro.Get_mm(y4));
  outline->items_.emplace_back(item);

  macro.Add(outline);
}

bool LineVectorPrimitiveItem::Render(GerberMacro &macro) {
  constexpr int modifier_count = 7;
  std::array<double, modifier_count> modifier{};

  for (int j = 0; j < modifier_count; j++) {
    if (j < modifiers_.size()) {
      modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
    } else {
      modifier[j] = 0.0;
    }
  }

  if (modifier[0] == 0.0) {
    macro.exposure_ = false;
  } else if (modifier[0] == 1.0) {
    macro.exposure_ = true;
  } else {
    macro.exposure_ = !macro.exposure_;
  }

  double w = 0.0;
  double a = 0.0;  // Width; Angle
  double a1 = 0.0;
  double a2 = 0.0;  // Angles of Start and End
  double r1 = 0.0;
  double r2 = 0.0;  // Radii of Start and End
  double x1 = 0.0;
  double y1 = 0.0;  // Start
  double x2 = 0.0;
  double y2 = 0.0;  // End
  double x3 = 0.0;
  double y3 = 0.0;  // Unit vector perpendicular to the line
  double x4 = 0.0;
  double y4 = 0.0;  // Corner1
  double x5 = 0.0;
  double y5 = 0.0;  // Corner2
  double x6 = 0.0;
  double y6 = 0.0;  // Corner3
  double x7 = 0.0;
  double y7 = 0.0;  // Corner4

  // Give the modifiers some names
  w = modifier[1];
  a = modifier[6] * kPi / 180.0;  // NOLINT
  x1 = modifier[2];
  y1 = modifier[3];
  x2 = modifier[4];
  y2 = modifier[5];  // NOLINT

  // Calculate the actual start and end points
  r1 = std::sqrt(y1 * y1 + x1 * x1);
  a1 = std::atan2(y1, x1) + a;
  x1 = r1 * std::cos(a1);
  y1 = r1 * std::sin(a1);

  r2 = std::sqrt(y2 * y2 + x2 * x2);
  a2 = std::atan2(y2, x2) + a;
  x2 = r2 * std::cos(a2);
  y2 = r2 * std::sin(a2);

  // Calculate the unit-vector perpendicular to the line
  a = std::atan2(y2 - y1, x2 - x1);
  a += kPi / 2.0;  // NOLINT
  x3 = std::cos(a);
  y3 = std::sin(a);

  // Scale the unit vector with the line width
  w /= 2.0;  // NOLINT
  x3 *= w;
  y3 *= w;

  // Calculate the corners
  x4 = x1 - x3;
  y4 = y1 - y3;
  x5 = x2 - x3;
  y5 = y2 - y3;
  x6 = x2 + x3;
  y6 = y2 + y3;
  x7 = x1 + x3;
  y7 = y1 + y3;

  // Draw the line
  auto outline = new Outline(macro.exposure_);
  Item *item = new Composite::BeginLineItem(macro.Get_mm(x4), macro.Get_mm(y4));
  outline->items_.emplace_back(item);

  item = new Composite::LineItem(macro.Get_mm(x5), macro.Get_mm(y5));
  outline->items_.emplace_back(item);

  item = new Composite::LineItem(macro.Get_mm(x6), macro.Get_mm(y6));
  outline->items_.emplace_back(item);

  item = new Composite::LineItem(macro.Get_mm(x7), macro.Get_mm(y7));
  outline->items_.emplace_back(item);

  macro.Add(outline);

  return true;
}

bool LineCenterPrimitiveItem::Render(GerberMacro &macro) {
  constexpr int modifier_count = 6;
  std::array<double, modifier_count> modifier{};

  for (int j = 0; j < modifier_count; j++) {
    if (j < modifiers_.size()) {
      modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
    } else {
      modifier[j] = 0.0;
    }
  }

  if (modifier[0] == 0.0) {
    macro.exposure_ = false;
  } else if (modifier[0] == 1.0) {
    macro.exposure_ = true;
  } else {
    macro.exposure_ = !macro.exposure_;
  }

  double a = 0.0;  // Rotation
  double w = 0.0;
  double h = 0.0;  // Width and Height
  double x0 = 0.0;
  double y0 = 0.0;  // Center
  double x1 = 0;
  double y1 = 0.0;  // Corner 1
  double x2 = 0.0;
  double y2 = 0.0;  // Corner 2
  double x3 = 0.0;
  double y3 = 0.0;  // Corner 3
  double x4 = 0.0;
  double y4 = 0.0;  // Corner 4

  // Give the modifiers some names
  w = modifier[1];
  h = modifier[2];
  x0 = modifier[3];
  y0 = modifier[4];
  a = modifier[5] * kPi / 180.0;  // NOLINT

  // Calculate the corners without rotation
  w /= 2.0;  // NOLINT
  h /= 2.0;  // NOLINT

  x1 = x0 - w;
  y1 = y0 - h;
  x2 = x0 + w;
  y2 = y0 - h;
  x3 = x0 + w;
  y3 = y0 + h;
  x4 = x0 - w;
  y4 = y0 + h;

  RenderLine(x1, y1, x2, y2, x3, y3, x4, y4, 0, 0, a, macro);
  return true;
}

bool LineLowerLeftPrimitiveItem::Render(GerberMacro &macro) {
  constexpr int modifier_count = 6;
  std::array<double, modifier_count> modifier{};

  for (int j = 0; j < modifier_count; j++) {
    if (j < modifiers_.size()) {
      modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
    } else {
      modifier[j] = 0.0;
    }
  }

  if (modifier[0] == 0.0) {
    macro.exposure_ = false;
  } else if (modifier[0] == 1.0) {
    macro.exposure_ = true;
  } else {
    macro.exposure_ = !macro.exposure_;
  }

  // Give the modifiers some names
  double w = modifier[1];
  double h = modifier[2];
  double x1 = modifier[3];
  double y1 = modifier[4];
  double a = modifier[5] * kPi / 180.0;  // NOLINT

  // Calculate the corners without rotation
  double x2 = x1 + w;
  double y2 = y1;
  double x3 = x1 + w;
  double y3 = y1 + h;
  double x4 = x1;
  double y4 = y1 + h;

  RenderLine(x1, y1, x2, y2, x3, y3, x4, y4, 0, 0, a, macro);
  return true;
}

bool OutlinePrimitiveItem::Render(GerberMacro &macro) {
  const auto modifier_count = modifiers_.size();
  std::vector<double> modifier(modifier_count);

  for (int j = 0; j < modifier_count; j++) {
    modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
  }

  if (modifier[0] == 0.0) {
    macro.exposure_ = false;
  } else if (modifier[0] == 1.0) {
    macro.exposure_ = true;
  } else {
    macro.exposure_ = !macro.exposure_;
  }

  auto N = int((modifier_count - 5.0) / 2.0 + 1.0);  // NOLINT

  double r = 0.0;
  double a = 0.0;  // Temporary
  std::vector<double> x(N);
  std::vector<double> y(N);

  int j = 0;
  for (j = 0; j < N; j++) {
    x[j] = modifier[2 * j + 2];
    y[j] = modifier[2 * j + 3];
  }
  double A = modifier[2 * j + 2] * kPi / 180.0;  // NOLINT

  // Make sure the polygon is closed
  x[N - 1] = x[0];
  y[N - 1] = y[0];

  // Rotate the points
  for (int j = 0; j < N; j++) {
    r = std::sqrt(y[j] * y[j] + x[j] * x[j]);
    a = std::atan2(y[j], x[j]) + A;
    x[j] = r * std::cos(a);
    y[j] = r * std::sin(a);
  }

  // Determine the wind
  double Area = 0.0;  // Actually double the area
  for (int j = 0; j < N - 1; j++) {
    Area += (x[j] - x[j + 1]) * (y[j] + y[j + 1]);
  }

  bool CCW = false;
  if (Area > 0.0) {
    CCW = true;
  }

  auto outline = new Outline(macro.exposure_);
  if (CCW) {
    Item *item =
        new Composite::BeginLineItem(macro.Get_mm(x[0]), macro.Get_mm(y[0]));
    outline->items_.emplace_back(item);

    for (int j = 1; j < N - 1; j++) {
      item = new Composite::LineItem(macro.Get_mm(x[j]), macro.Get_mm(y[j]));
      outline->items_.emplace_back(item);
    }
  } else {
    Item *item = new Composite::BeginLineItem(macro.Get_mm(x[N - 1]),
                                              macro.Get_mm(y[N - 1]));
    outline->items_.emplace_back(item);

    for (int j = N - 2; j > 0; j--) {
      item = new Composite::LineItem(macro.Get_mm(x[j]), macro.Get_mm(y[j]));
      outline->items_.emplace_back(item);
    }
  }

  macro.Add(outline);
  return true;
}

bool PolygonPrimitiveItem::Render(GerberMacro &macro) {
  constexpr int modifier_count = 6;
  std::array<double, modifier_count> modifier{};

  for (int j = 0; j < modifier_count; j++) {
    if (j < modifiers_.size()) {
      modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
    } else {
      modifier[j] = 0.0;
    }
  }

  if (modifier[0] == 0.0) {
    macro.exposure_ = false;
  } else if (modifier[0] == 1.0) {
    macro.exposure_ = true;
  } else {
    macro.exposure_ = !macro.exposure_;
  }

  int N = int(modifier[1]);
  double X = modifier[2];
  double Y = modifier[3];
  double R = modifier[4] / 2.0;          // NOLINT
  double A = modifier[5] * kPi / 180.0;  // NOLINT

  std::vector<double> x(N);
  std::vector<double> y(N);

  double da = 2.0 * kPi / N;  // NOLINT

  double r = std::sqrt(Y * Y + X * X);
  double a = std::atan2(Y, X) + A;
  X = r * std::cos(a);
  Y = r * std::sin(a);

  a = A;
  for (int j = 0; j < N; j++) {
    x[j] = X + R * std::cos(a);
    y[j] = Y + R * std::sin(a);
    a += da;
  }

  auto outline = new Outline(macro.exposure_);
  Item *item =
      new Composite::BeginLineItem(macro.Get_mm(x[0]), macro.Get_mm(y[0]));
  outline->items_.emplace_back(item);

  for (int j = 1; j < N; j++) {
    item = new Composite::LineItem(macro.Get_mm(x[j]), macro.Get_mm(y[j]));
    outline->items_.emplace_back(item);
  }

  macro.Add(outline);
  return true;
}

bool MoirePrimitiveItem::Render(GerberMacro &macro) {
  constexpr int modifier_count = 9;
  std::array<double, modifier_count> modifier{};

  for (int j = 0; j < modifier_count; j++) {
    if (j < modifiers_.size()) {
      modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
    } else {
      modifier[j] = 0.0;
    }
  }

  auto X = modifier[0];
  auto Y = modifier[1];
  auto OD = modifier[2];
  auto thickness = modifier[3];
  auto gap = modifier[4];
  int N = int(modifier[5]);  // NOLINT

  auto width = modifier[6];     // NOLINT
  auto length = modifier[7];    // NOLINT
  auto rotation = modifier[8];  // NOLINT

  // Draw rings:
  double d = OD;
  for (int j = 0; j < N; j++) {
    if (d < width) {
      break;
    }

    macro.Add(new Circle(macro.Get_mm(X), macro.Get_mm(Y), macro.Get_mm(d)));

    d -= thickness * 2.0;  // NOLINT
    if (d < width) {
      break;
    }

    auto outline = new Outline();
    outline->items_.emplace_back(new Composite::BeginLineItem(
        macro.Get_mm(X + d / 2.0), macro.Get_mm(Y)));
    outline->items_.emplace_back(
        new Composite::ArcItem(macro.Get_mm(X), macro.Get_mm(Y), -360.0));
    macro.Add(outline);

    d -= gap * 2.0;  // NOLINT
  }

  auto dx = width / 2.0;   // NOLINT
  auto dy = length / 2.0;  // NOLINT

  auto x1 = -dx;
  auto y1 = -dy;
  auto x2 = +dx;
  auto y2 = -dy;
  auto x3 = +dx;
  auto y3 = +dy;
  auto x4 = -dx;
  auto y4 = +dy;

  RenderLine(X + x1, Y + y1, X + x2, Y + y2, X + x3, Y + y3, X + x4, Y + y4, X,
             Y, rotation, macro);
  RenderLine(X + x1, Y + y1, X + x2, Y + y2, X + x3, Y + y3, X + x4, Y + y4, X,
             Y, rotation + kPi / 2.0, macro  // NOLINT
  );

  return true;
}

bool ThermalPrimitiveItem::Render(GerberMacro &macro) {
  constexpr int modifier_count = 6;
  std::array<double, modifier_count> modifier{};

  for (int j = 0; j < modifier_count; j++) {
    if (j < modifiers_.size()) {
      modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
    } else {
      modifier[j] = 0.0;
    }
  }

  double X = modifier[0];
  double Y = modifier[1];
  double OD = modifier[2];
  double ID = modifier[3];
  double Gap = modifier[4];
  double Rot = modifier[5];  // NOLINT

  OD /= 2.0;           // NOLINT
  ID /= 2.0;           // NOLINT
  Gap /= 2.0;          // NOLINT
  Rot *= kPi / 180.0;  // NOLINT

  double x1 = std::sqrt(OD * OD - Gap * Gap);
  double y1 = Gap;
  double x2 = Gap;
  double y2 = std::sqrt(ID * ID - Gap * Gap);

  double a1 = -2.0 * std::atan2(y1, x1) + kPi / 2.0;  // NOLINT
  double a2 = 2.0 * std::atan2(y2, x2) - kPi / 2.0;   // NOLINT

  double r = std::sqrt(y1 * y1 + x1 * x1);
  double t = std::atan2(y1, x1) + Rot;
  x1 = r * std::cos(t);
  y1 = r * std::sin(t);

  r = std::sqrt(y2 * y2 + x2 * x2);
  t = std::atan2(y2, x2) + Rot;
  x2 = r * std::cos(t);
  y2 = r * std::sin(t);

  a1 *= 180.0 / kPi;  // NOLINT
  a2 *= 180.0 / kPi;  // NOLINT

  for (int j = 0; j < 4; j++) {
    auto outline = new Outline();
    outline->items_.emplace_back(new Composite::BeginLineItem(
        macro.Get_mm(X + x1), macro.Get_mm(Y + y1)));
    outline->items_.emplace_back(
        new Composite::ArcItem(macro.Get_mm(X), macro.Get_mm(Y), a1));
    outline->items_.emplace_back(
        new Composite::LineItem(macro.Get_mm(X + x2), macro.Get_mm(Y + y2)));
    outline->items_.emplace_back(
        new Composite::ArcItem(macro.Get_mm(X), macro.Get_mm(Y), -a2));
    macro.Add(outline);

    // Rotate 90 deg
    t = x1;
    x1 = -y1;
    y1 = t;
    t = x2;
    x2 = -y2;
    y2 = t;
  }

  return true;
}

bool AssignmentPrimitiveItem::Render(GerberMacro &macro) {
  if (modifiers_.empty()) {
    return false;
  }

  if (index_ > macro.modifiers_.size()) {
    macro.modifiers_.resize(index_);
  }

  macro.modifiers_[index_ - 1] = Evaluate(modifiers_[0], macro.modifiers_);
  return true;
}

double PrimitiveItem::Evaluate(const std::shared_ptr<OperatorItem> &root,
                               const std::vector<double> &modifiers) {
  if (!root) {
    return 0.0;
  }

  switch (root->Opera()) {
    case OperatorItem::kAdd:
      return Evaluate(root->Left(), modifiers) +
             Evaluate(root->Right(), modifiers);

    case OperatorItem::kSubtract:
      return Evaluate(root->Left(), modifiers) -
             Evaluate(root->Right(), modifiers);

    case OperatorItem::kMultiply:
      return Evaluate(root->Left(), modifiers) *
             Evaluate(root->Right(), modifiers);

    case OperatorItem::kDivide:
      return Evaluate(root->Left(), modifiers) /
             Evaluate(root->Right(), modifiers);

    case OperatorItem::kVariable:
      if (root->Index > 0 && root->Index <= modifiers.size()) {
        return modifiers[root->Index - 1];
      } else {
        return 0.0;
      }

    case OperatorItem::kLiteral:
      return root->Value;

    default:
      return 0.0;
  }
}
