#pragma once
#include <map>
#include <memory>

class Aperture;
class Gerber;
class GerberLevel;
class Primitive;

class BoundBox;
class QPainter;
class QPixmap;
class QPaintDevice;

class GerberApi Engine {
 public:
  Engine() = default;

  virtual std::pair<double, double> CurrentPos() const = 0;
  virtual void CubicTo(const std::pair<double, double> &ctrl_pt1,
                       const std::pair<double, double> &ctrl_pt2,
                       const std::pair<double, double> &end_pt) = 0;
  virtual void AddRect(double x, double y, double w, double h) = 0;
  virtual void AddCircle(double x, double y, double radius) = 0;
  virtual void MoveTo(const std::pair<double, double> &pt) = 0;
  virtual void LineTo(const std::pair<double, double> &pt) = 0;
  virtual void DrawAperture(std::shared_ptr<Aperture> aperture,
                            const std::pair<double, double> &start) = 0;

  virtual void BeginDrawOutline() = 0;
  virtual void EndDrawOutline() = 0;

  virtual void BeginDrawStroke(std::shared_ptr<Aperture> aperture) = 0;
  virtual void CachePoint(const std::pair<double, double> &pt) = 0;
  virtual std::pair<double, double> CachedPoint() const = 0;
  virtual void EndDrawStroke() = 0;

  bool convert_strokes2fills_{false};
};
