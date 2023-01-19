#pragma once
#include <memory>
#include <vector>

#include "gerber_parser/bound_box.h"

class Item;
class Aperture;
class Engine;

class Primitive
{
public:
    enum Type
    {
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

    virtual int Draw(Engine &engine) = 0;
    virtual BoundBox BBox() const = 0;

    Type type_;
    std::pair<double, double> start_;
    std::pair<double, double> end_;
    std::vector<std::shared_ptr<Item>> items_;

    static constexpr int kTimes = 10000;
};

class Item
{
public:
    Item(Primitive::Type type);

    virtual int Draw(Engine &engine) = 0;
    virtual BoundBox BBox(double &x, double &y) const = 0;

    double angle_;
    std::pair<double, double> start_;
    std::pair<double, double> end_;
    Primitive::Type type_;
    static constexpr int kTimes = 10000;
};

class BeginLineItem : public Item
{
public:
    BeginLineItem(double x, double y);
    int Draw(Engine &engine) override;
    BoundBox BBox(double &x, double &y) const override;
};

class LineItem : public Item
{
public:
    LineItem(double x, double y);
    int Draw(Engine &engine) override;
    BoundBox BBox(double &x, double &y) const override;
};

class ArcItem : public Item
{
public:
    ArcItem(double x, double y, double angle);

    int Draw(Engine &engine) override;
    BoundBox BBox(double &x, double &y) const override;

private:
    void DrawArcScaled(Engine &engine, double x, double y, double degree);
};

class Rectangle : public Primitive
{
public:
    Rectangle(double w, double h);

    int Draw(Engine &engine) override;
    BoundBox BBox() const override;

private:
    double width_{0};
    double height_{0};
};

class Circle : public Primitive
{
public:
    Circle(double x, double y, double diameter, bool fill = true);

    int Draw(Engine &engine) override;
    BoundBox BBox() const override;

private:
    bool fill_;
    double diameter_;
};

class Stroke : public Primitive
{
public:
    Stroke(std::shared_ptr<Aperture> aperture);

    int Draw(Engine &engine) override;
    BoundBox BBox() const override;

private:
    void DrawRectLine(
        Engine &engine,
        double x1, double y1,
        double x2, double y2,
        double w, double h);

    std::shared_ptr<Aperture> aperture_;
};

class Flash : public Primitive
{
public:
    Flash(double x, double y, std::shared_ptr<Aperture> aperture);

    int Draw(Engine &engine) override;
    BoundBox BBox() const override;

private:
    std::shared_ptr<Aperture> aperture_;
};

class Outline : public Primitive
{
public:
    Outline(bool fill = true);

    int Draw(Engine &engine) override;
    BoundBox BBox() const override;

private:
    bool fill_;
};
