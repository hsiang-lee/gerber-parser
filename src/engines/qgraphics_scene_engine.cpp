#include "qgraphics_scene_engine.h"
#include "gerber/gerber_level.h"
#include "aperture/aperture.h"
#include "gerber/gerber_primitive.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsItem>

QGraphicsSceneEngine::QGraphicsSceneEngine(const BoundBox &bound_box, const BoundBox &offset) : trans_(bound_box.Scaled(kTimes), offset)
{
    scene_ = new QGraphicsScene;
}

void QGraphicsSceneEngine::Resize()
{
}

void QGraphicsSceneEngine::Scale(double delta, double center_x, double center_y)
{
    if (trans_.Scale(delta, center_x, center_y))
    {
        aperture_imgs_.clear();
    }
}

void QGraphicsSceneEngine::Move(int delta_x, int delta_y)
{
    trans_.Move(delta_x, delta_y);
}

void QGraphicsSceneEngine::SetConvertStroke2Fills(bool value)
{
    convert_strokes2fills_ = value;
}

void QGraphicsSceneEngine::BeginRender()
{
    // scene_->setSceneRect(trans_.GetPainterWindow());
}

std::shared_ptr<QPainter> QGraphicsSceneEngine::CreatePainter(QPaintDevice *pic)
{
    return std::make_shared<QPainter>(pic);
}

void QGraphicsSceneEngine::EndRender()
{
    count_ = 0;
}

int QGraphicsSceneEngine::RenderGerber(const std::shared_ptr<Gerber> &gerber)
{
    BeginRender();

    auto levels = gerber->Levels();
    for (const auto &level : levels)
    {
        negative_ = level->IsNegative();

        int ret = 0;
        if (level->IsCopyLayer())
        {
            const auto step_x = kTimes * level->step_x_;
            const auto step_y = kTimes * level->step_y_;

            // current_painter_->save();
            for (int y = 0; y < level->count_y_; ++y)
            {
                for (int x = 0; x < level->count_x_; ++x)
                {
                    ret = level->Draw(this);
                    // current_painter_->translate(step_x, 0);
                }

                // current_painter_->translate(-step_x * level->count_x_, step_y);
            }
            // current_painter_->restore();
        }
        else
        {
            ret = level->Draw(this);
        }

        if (ret)
        {
            return ret;
        }

        count_ += 155;
    }

    EndRender();

    return 0;
}

void QGraphicsSceneEngine::PreparePenAndBrush()
{
    /*current_painter_->setPen(QPen(QColor(0, 0, 0, 0), 1.0));

    if (negative_)
    {
        current_painter_->setBrush(QColor(255, 255, 255));
    }
    else
    {
        current_painter_->setBrush(QColor(count_ % 256, (count_ + 153) % 256, (count_ + 25) % 256, 200));
    }*/
}

void QGraphicsSceneEngine::PreparePenAndBrushForStroke(double line_width)
{
    /*current_painter_->setBrush(QColor(0, 0, 0, 0));

    auto pen = current_painter_->pen();
    if (negative_)
    {
        pen.setColor(QColor(255, 255, 255));
    }
    else
    {
        pen.setColor(QColor(count_ % 256, (count_ + 153) % 256, (count_ + 25) % 256, 200));
    }
    pen.setWidthF(line_width);
    current_painter_->setPen(pen);*/
}

void QGraphicsSceneEngine::DrawAperture(std::shared_ptr<Aperture> aperture, const std::pair<double, double> &start)
{
    if (!PrepareExistAperture(aperture->Code()))
    {
        NewAperture(aperture);

        auto primitives = aperture->Primitives();
        for (auto &item : primitives)
        {
            path_ = QPainterPath();
            path_.setFillRule(Qt::FillRule::OddEvenFill);
            item->Draw(*this);

            current_painter_->setPen(QPen(QColor(0, 0, 0, 0), 1.0));

            if (negative_)
            {
                current_painter_->setBrush(QColor(255, 255, 255));
            }
            else
            {
                current_painter_->setBrush(QColor(count_ % 256, (count_ + 153) % 256, (count_ + 25) % 256, 200));
            }

            current_painter_->drawPath(path_);
        }

        EndDrawNewAperture();
    }

    auto item = scene_->addPixmap(*aperture_imgs_[aperture->Code()]);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    path_ = QPainterPath();
}

std::pair<double, double> QGraphicsSceneEngine::CurrentPos() const
{
    return std::make_pair(path_.currentPosition().x(), path_.currentPosition().y());
}

void QGraphicsSceneEngine::CubicTo(
    const std::pair<double, double> &ctrl_pt1,
    const std::pair<double, double> &ctrl_pt2,
    const std::pair<double, double> &end_pt)
{
    path_.cubicTo(ctrl_pt1.first, ctrl_pt1.second, ctrl_pt2.first, ctrl_pt2.second, end_pt.first, end_pt.second);
}

void QGraphicsSceneEngine::AddRect(double x, double y, double w, double h)
{
    path_.addRect(x, y, w, h);
}

void QGraphicsSceneEngine::AddCircle(double x, double y, double radius)
{
    path_.addEllipse(x - radius, y - radius, radius * 2, radius * 2);
}

void QGraphicsSceneEngine::MoveTo(const std::pair<double, double> &pt)
{
    path_.moveTo(pt.first, pt.second);
}

void QGraphicsSceneEngine::LineTo(const std::pair<double, double> &pt)
{
    path_.lineTo(pt.first, pt.second);
}

void QGraphicsSceneEngine::BeginDrawOutline()
{
    PreparePenAndBrush();
}

void QGraphicsSceneEngine::EndDrawOutline()
{
    auto item = scene_->addPath(path_, QPen(QColor(count_ % 256, (count_ + 153) % 256, (count_ + 25) % 256, 200)), QBrush(QColor(count_ % 256, (count_ + 153) % 256, (count_ + 25) % 256, 200)));
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    path_ = QPainterPath();
}

void QGraphicsSceneEngine::BeginDrawStroke(std::shared_ptr<Aperture> aperture)
{
    if (aperture->SolidCircle())
    {
        PreparePenAndBrushForStroke(aperture->BBox().Width() * kTimes);
    }
    else if (aperture->SolidRectangle())
    {
        PreparePenAndBrush();
    }
    else
    {
    }
}

void QGraphicsSceneEngine::CachePoint(const std::pair<double, double> &pt)
{
    rect_x_ = pt.first;
    rect_y_ = pt.second;
}

std::pair<double, double> QGraphicsSceneEngine::CachedPoint() const
{
    return std::make_pair(rect_x_, rect_y_);
}

void QGraphicsSceneEngine::EndDrawStroke()
{
    auto item = scene_->addPath(path_);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    path_ = QPainterPath();
}

void QGraphicsSceneEngine::EndDrawNewAperture()
{
}

std::shared_ptr<QPixmap> QGraphicsSceneEngine::PrepareExistAperture(int code)
{
    if (aperture_imgs_.find(code) != aperture_imgs_.end())
    {
        return aperture_imgs_[code];
    }

    return nullptr;
}

void QGraphicsSceneEngine::NewAperture(std::shared_ptr<Aperture> aperture)
{
    const auto bound_box = aperture->BBox();
    const auto left = bound_box.Left() * kTimes;
    const auto right = bound_box.Right() * kTimes;
    const auto top = bound_box.Top() * kTimes;
    const auto bottom = bound_box.Bottom() * kTimes;

    auto width = trans_.TranslateLogicCoord(right - left);
    auto height = trans_.TranslateLogicCoord(top - bottom);
    if (width < 1)
    {
        width = 1;
    }

    if (height < 1)
    {
        height = 1;
    }

    auto img = std::make_shared<QPixmap>(width * 10, height * 10);
    img->fill(QColor(255, 255, 255, 0));
    aperture_imgs_[aperture->Code()] = img;
    current_painter_ = std::make_shared<QPainter>(img.get());
    current_painter_->setWindow(left, bottom, right - left, top - bottom);
}

QGraphicsScene *QGraphicsSceneEngine::scene() const
{
    return scene_;
}
