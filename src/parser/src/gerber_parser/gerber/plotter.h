#pragma once
#include "gerber/gerber_enums.h"

#include <memory>

class Aperture;
class Outline;
class Stroke;
class GerberLayer;

class Plotter
{
public:
    Plotter(GerberLayer &layer, std::shared_ptr<Plotter> old_plotter);
    ~Plotter() = default;

    void OutlineBegin(unsigned line_number);
    void OutlineEnd(unsigned line_number);
    void Do(unsigned line_number);
    void ApertureSelect(Aperture *aperture, unsigned line_number);

    void SetExposure(GERBER_EXPOSURE exposure)
    {
        exposure_ = exposure;
    }

    void SetInterpolation(GERBER_INTERPOLATION interpolation)
    {
        interpolation_ = interpolation;
    }

    double x_, y_, i_, j_;
    bool multi_quadrant_;

private:
    void Move(unsigned line_number);
    void DrawLine();
    void DrawArc();
    void DrawFlash();

    double GetAngle(
        double x1, double y1, // Start, relative to center
        double x2, double y2  // End, relative to center
    );

    GerberLayer &layer_;
    GERBER_EXPOSURE exposure_{geOff};
    GERBER_INTERPOLATION interpolation_{giLinear};

    bool drawing_line_{false};
    double pre_x_{0.0}, pre_y_{0.0};
    double first_x_{0.0}, first_y_{0.0};

    Aperture *current_aperture_ = nullptr;
    Outline *outline_ = nullptr;
    Stroke *stroke_ = nullptr;
};
