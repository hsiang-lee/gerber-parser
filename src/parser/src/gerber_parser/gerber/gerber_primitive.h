#pragma once
#include "gerber_parser/bound_box.h"
#include <vector>

class Aperture;
class Engine;

class Primitive {
 public:
  enum Type {
    cRectangle,
    cCircle,
    cOutline,

    cBeginLine,

    cLine,
    cArc,

    cClose,

    cStroke,
    cFlash
  };

  Primitive(Type type);
  virtual ~Primitive();

  virtual int Draw(Engine &engine) = 0;
  virtual BoundBox BBox() const = 0;

  Type type_;
};

class Single : public Primitive {
 protected:
  Single(Type type);
  virtual ~Single();

  public:
  std::pair<double, double> start_;
  std::pair<double, double> end_;
};

class Rectangle : public Single {
 public:
  Rectangle(double w, double h);

  int Draw(Engine &engine) override;
  BoundBox BBox() const override;

 private:
  double width_{0};
  double height_{0};
};

class Circle : public Single {
 public:
  Circle(double x, double y, double diameter, bool fill = true);

  int Draw(Engine &engine) override;
  BoundBox BBox() const override;

 private:
  bool fill_;
  double diameter_;
};

class Flash : public Single {
 public:
  Flash(double x, double y, Aperture *aperture);

  int Draw(Engine &engine) override;
  BoundBox BBox() const override;

  Aperture *GetAperture();

 private:
  Aperture *aperture_;
};

class Item {
 public:
  Item(Primitive::Type type);
  virtual ~Item();

  virtual int Draw(Engine &engine) = 0;
  virtual BoundBox BBox(double &x, double &y) const = 0;

  double angle_;
  std::pair<double, double> start_;
  std::pair<double, double> end_;
  Primitive::Type type_;
};

class Composite : public Primitive {
 public:
  class BeginLineItem : public Item {
   public:
    BeginLineItem(double x, double y);
    int Draw(Engine &engine) override;
    BoundBox BBox(double &x, double &y) const override;
  };

  class LineItem : public Item {
   public:
    LineItem(double x, double y);
    int Draw(Engine &engine) override;
    BoundBox BBox(double &x, double &y) const override;
  };

  class ArcItem : public Item {
   public:
    ArcItem(double x, double y, double angle);

    int Draw(Engine &engine) override;
    BoundBox BBox(double &x, double &y) const override;

   private:
    void DrawArcScaled(Engine &engine, double x, double y, double degree);
  };

  protected:
  Composite(Type type);
  virtual ~Composite();
  
  public:
  std::vector<Item *> items_;
};

class Stroke : public Composite {
 public:
  Stroke(Aperture *aperture);

  int Draw(Engine &engine) override;
  BoundBox BBox() const override;

 private:
  void DrawRectLine(Engine &engine, double x1, double y1, double x2, double y2,
                    double w, double h);

  Aperture *aperture_;
};

class Outline : public Composite {
 public:
  Outline(bool fill = true);

  int Draw(Engine &engine) override;
  BoundBox BBox() const override;

 private:
  bool fill_;
};
