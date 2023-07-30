#pragma once
#include <map>
#include <memory>

#include "engine/engine.h"
#include "transformation.h"

#include <QColor>
#include <QPainterPath>

class Aperture;
class Gerber;
class GerberLevel;
class Primitive;

class BoundBox;
class QPainter;
class QPixmap;
class QPaintDevice;

class QPainterEngine : public Engine
{
public:
    QPainterEngine(QPaintDevice *device, const BoundBox &bound_box, double offset);

    void Resize();
    void Rotate(int angle);
    void Scale(double delta, double center_x = -999999, double center_y = -999999);
    void Move(int delta_x, int delta_y);
    void SetConvertStroke2Fills(bool value);
    void Reset();

    void SetSelectRect(const QRect& rect);

    QPoint Dev2Logic(const QPoint& dev_pos);
    QRect Dev2Logic(const QRect& rect);
    QRect PainterWindow();

    void DrawBackground(const QColor& color = QColor(255, 255, 255));
    int RenderGerber(const std::shared_ptr<Gerber> &);

    void BeginRender();
    void EndRender();

    void PreparePenAndBrush();
    void PreparePenAndBrushForStroke(double line_width);

    virtual std::shared_ptr<QPainter> CreatePainter(QPaintDevice *pic);

    std::pair<double, double> CurrentPos() const override;
    void CubicTo(
        const std::pair<double, double> &ctrl_pt1,
        const std::pair<double, double> &ctrl_pt2,
        const std::pair<double, double> &end_pt);
    void AddRect(double x, double y, double w, double h) override;
    void AddCircle(double x, double y, double radius) override;
    void MoveTo(const std::pair<double, double> &pt) override;
    void LineTo(const std::pair<double, double> &pt) override;
    void DrawAperture(std::shared_ptr<Aperture> aperture, const std::pair<double, double> &start) override;

    void BeginDrawOutline() override;
    void EndDrawOutline() override;

    void BeginDrawStroke(std::shared_ptr<Aperture> aperture) override;
    void CachePoint(const std::pair<double, double> &pt) override;
    std::pair<double, double> CachedPoint() const override;
    void EndDrawStroke() override;

    void EndDrawNewAperture();
    std::shared_ptr<QPixmap> PrepareExistAperture(int code);
    void NewAperture(std::shared_ptr<Aperture>);

private:
    QPaintDevice *pic_;
    std::shared_ptr<QPainter> painter_;

    static constexpr int kTimes = 10000;

    int rotation_{ 0 };
    double scale_{ 1.0 };
    QPoint scale_center_{0, 0};
    Transformation trans_;

    QPoint movement_;
    QRect select_rect_;

    std::map<int, std::shared_ptr<QPixmap>> aperture_imgs_;

    std::shared_ptr<QPainter> current_painter_;

    friend class GerberLevel;
    friend class Primitive;
    friend class Outline;
    friend class Stroke;
    friend class Arc;
    friend class Flash;
    friend class ApertureLevel;

    double rect_x_{0.0};
    double rect_y_{0.0};

    uint32_t count_{0};
    bool negative_{false};
    QPainterPath path_;
};
