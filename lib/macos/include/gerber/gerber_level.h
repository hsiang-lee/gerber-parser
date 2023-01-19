#pragma once

#include <memory>
#include <string>
#include <vector>

#include "gerber/gerber.h"
#include "gerber_parser/bound_box.h"

class Plotter;
class Engine;
class Primitive;

class GerberLevel {
 private:
  std::vector<std::shared_ptr<Primitive>> primitives_{};
  void Add(const std::shared_ptr<Primitive> &primitive);

  friend class Plotter;
  friend class StrokesToFillsConverter;

  std::string name_;
  bool negative_;
  BoundBox bound_box_{0.0, 0.0, 0.0, 0.0};
  UnitType::Type unit_{UnitType::guMillimeters};

  void ConvertStrokesToFills();

 public:
  GerberLevel(const std::shared_ptr<GerberLevel> &previous_level,
              UnitType::Type units);
  ~GerberLevel();

  double GetRight() const;
  double GetTop() const;

  UnitType::Type GetUnit() const;
  void SetUnit(UnitType::Type type);

  // Step-and-Repeat
  int count_x_, count_y_;
  double step_x_, step_y_;
  bool IsCopyLayer() const;

  std::string GetName() const;
  void SetName(const std::string &name);

  bool IsNegative() const;
  void SetNegative(bool);

  BoundBox GetBBox() const;

  std::vector<std::shared_ptr<Primitive>> Primitives() const;

  int Draw(Engine *engine);
};
