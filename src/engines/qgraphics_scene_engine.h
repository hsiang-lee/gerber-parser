#pragma once
#include <map>
#include <memory>

#include "engine/engine.h"
#include "transformation.h"

#include <QPainterPath>

class Aperture;
class Gerber;
class GerberLevel;
class Primitive;

class BoundBox;
class QPainter;
class QPixmap;
class QPaintDevice;
class QGraphicsScene;

class QGraphicsSceneEngine : public Engine
{
public:
    QGraphicsSceneEngine(const BoundBox& bound_box, double offset);

    void Resize();
    void Scale(double delta, double center_x = 0.0, double center_y = 0.0);
    void Move(int delta_x, int delta_y);
    void SetConvertStroke2Fills(bool value);

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
        const std::pair<double, double> &end_pt) override;
    void AddRect(double x, double y, double w, double h) override;
    void AddCircle(double x, double y, double radius) override;
    void MoveTo(const std::pair<double, double> &pt) override;
    void LineTo(const std::pair<double, double> &pt) override;
    void DrawAperture(Aperture* aperture, const std::pair<double, double> &start) override;

    void BeginDrawOutline() override;
    void EndDrawOutline() override;

    void BeginDrawStroke(Aperture* aperture) override;
    void CachePoint(const std::pair<double, double> &pt) override;
    std::pair<double, double> CachedPoint() const override;
    void EndDrawStroke() override;

    void EndDrawNewAperture();
    std::shared_ptr<QPixmap> PrepareExistAperture(int code);
    void NewAperture(Aperture*);

    QGraphicsScene *scene() const;

private:
    static constexpr int kTimes = 10000;

    Transformation trans_;

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
    QGraphicsScene *scene_;
};
