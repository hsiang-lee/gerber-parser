#pragma once

#include <memory>
#include <string>
#include <vector>

#include "gerber_parser/bound_box.h"

class Aperture;
class GerberLayer;

class UnitType {
 public:
  enum Type { guInches, guMillimeters };

  static double Get_mm(double number, Type unit);
};

class GerberApi Gerber {
 public:
  Gerber() {
    unit_ = UnitType::guInches;
    apertures_.resize(3000);  // NOLINT
  }

  bool negative_{false};
  bool IsNegative() const;

  UnitType::Type unit_{UnitType::guMillimeters};
  UnitType::Type Unit() const;

  BoundBox GetBBox() const;
  std::string Name() const;
  void SetName(const std::string& name);

  std::shared_ptr<Aperture> GetAperture(int code) const;
  void SetAperture(int code, std::shared_ptr<Aperture> aperture);

  const std::vector<std::shared_ptr<GerberLayer>>& GetLayers() const;
  void AddLayer(std::shared_ptr<GerberLayer> layer);

private:
  std::vector<std::shared_ptr<GerberLayer>> layers_;
  std::vector<std::shared_ptr<Aperture>> apertures_;
  std::string name_;
};
