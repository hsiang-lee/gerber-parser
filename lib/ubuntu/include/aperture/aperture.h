#pragma once

#include <cstdint>
#include <vector>

#include "gerber_parser/bound_box.h"

class Engine;
class Primitive;

class GerberApi Aperture {
 private:
  uint16_t count_;

 protected:
  std::vector<Primitive *> primitives_{};
  Primitive *outline_ = nullptr;
  std::vector<Primitive *> Primitives();

  void RenderHole();

  void HoleCircle(double d);
  void HoleRectangle(double w, double h);

  virtual void RenderAperture() = 0;

  double dimension_x_;  // Also used for outside diameter of circles
  double dimension_y_;
  double hole_x_;  // For no hole, make this negative
  double hole_y_;  // For a round hole make this negative

  // Regular Polygon additional modifiers
  double rotation_;  // Degrees of rotaion (rotate the whole thing CCW)
  int code_;         // The code to use in the D-Code to load this tool

 public:
  Aperture(int code);
  virtual ~Aperture();

  int Code() const;

  enum ApertureType { tCircle, tMacro, tObround, tPolygon, tRectangle };

  virtual ApertureType Type() const = 0;

  // Used to determine if it is a basic shape or not
  virtual bool SolidCircle() = 0;
  virtual bool SolidRectangle() = 0;
  virtual BoundBox BBox() const = 0;

  int Draw(Engine *engine);

  void Visit();
  uint16_t GetCount() const;
};
