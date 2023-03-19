#include "qpainter_engine.h"
#include "aperture/aperture.h"
#include "gerber/gerber_layer.h"
#include "gerber/gerber_primitive.h"

#include <QPainter>

QPainterEngine::QPainterEngine(QPaintDevice *device, const BoundBox &bound_box,
                               const BoundBox &offset)
    : pic_(device), trans_(bound_box.Scaled(kTimes), offset)
{
  trans_.SetPhysicalSize(pic_->width(), pic_->height());
}

void QPainterEngine::Resize()
{
  trans_.SetPhysicalSize(pic_->width(), pic_->height());
}

void QPainterEngine::Scale(double delta, double center_x, double center_y)
{
  if (trans_.Scale(delta, center_x, center_y))
  {
    aperture_imgs_.clear();
  }
}

void QPainterEngine::Move(int delta_x, int delta_y)
{
  trans_.Move(delta_x, delta_y);
}

void QPainterEngine::SetConvertStroke2Fills(bool value)
{
  convert_strokes2fills_ = value;
}

void QPainterEngine::BeginRender()
{
  painter_ = CreatePainter(pic_);
  painter_->fillRect(0, 0, pic_->width(), pic_->height(),
                     QColor(255, 255, 255));
  painter_->setWindow(trans_.GetPainterWindow());
  painter_->setClipRect(painter_->window());
  painter_->setViewport(trans_.GetPainterViewport());
  current_painter_ = painter_;
}

std::shared_ptr<QPainter> QPainterEngine::CreatePainter(QPaintDevice *pic)
{
  return std::make_shared<QPainter>(pic);
}

void QPainterEngine::EndRender()
{
  painter_ = nullptr;
  count_ = 0;
}

int QPainterEngine::RenderGerber(const std::shared_ptr<Gerber> &gerber)
{
  BeginRender();

  auto layers = gerber->layers_;
  for (const auto &layer : layers)
  {
    negative_ = layer->IsNegative();

    int ret = 0;
    if (layer->IsCopyLayer())
    {
      const auto step_x = kTimes * layer->step_x_;
      const auto step_y = kTimes * layer->step_y_;

      current_painter_->save();
      for (int y = 0; y < layer->count_y_; ++y)
      {
        for (int x = 0; x < layer->count_x_; ++x)
        {
          ret = layer->Draw(this);
          current_painter_->translate(step_x, 0);
        }

        current_painter_->translate(-step_x * layer->count_x_, step_y);
      }
      current_painter_->restore();
    }
    else
    {
      ret = layer->Draw(this);
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

void QPainterEngine::PreparePenAndBrush()
{
  current_painter_->setPen(QPen(QColor(0, 0, 0, 0), 1.0));

  if (negative_)
  {
    current_painter_->setBrush(QColor(255, 255, 255));
  }
  else
  {
    current_painter_->setBrush(
        QColor(count_ % 256, (count_ + 153) % 256, (count_ + 25) % 256, 200));
  }
}

void QPainterEngine::PreparePenAndBrushForStroke(double line_width)
{
  current_painter_->setBrush(QColor(0, 0, 0, 0));

  auto pen = current_painter_->pen();
  if (negative_)
  {
    pen.setColor(QColor(255, 255, 255));
  }
  else
  {
    pen.setColor(
        QColor(count_ % 256, (count_ + 153) % 256, (count_ + 25) % 256, 200));
  }
  pen.setWidthF(line_width);
  current_painter_->setPen(pen);
}

void QPainterEngine::DrawAperture(std::shared_ptr<Aperture> aperture,
                                  const std::pair<double, double> &start)
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
        current_painter_->setBrush(QColor(count_ % 256, (count_ + 153) % 256,
                                          (count_ + 25) % 256, 200));
      }

      current_painter_->drawPath(path_);
    }

    EndDrawNewAperture();
  }

  const auto bound_box = aperture->BBox();
  current_painter_->drawPixmap(
      QRectF(start.first * kTimes + bound_box.Left() * kTimes,
             start.second * kTimes + bound_box.Bottom() * kTimes,
             bound_box.Width() * kTimes, bound_box.Height() * kTimes),
      *aperture_imgs_[aperture->Code()],
      QRectF(0.0, 0.0, aperture_imgs_[aperture->Code()]->width(),
             aperture_imgs_[aperture->Code()]->height()));

  path_ = QPainterPath();
}

std::pair<double, double> QPainterEngine::CurrentPos() const
{
  return std::make_pair(path_.currentPosition().x(),
                        path_.currentPosition().y());
}

void QPainterEngine::CubicTo(const std::pair<double, double> &ctrl_pt1,
                             const std::pair<double, double> &ctrl_pt2,
                             const std::pair<double, double> &end_pt)
{
  path_.cubicTo(ctrl_pt1.first, ctrl_pt1.second, ctrl_pt2.first,
                ctrl_pt2.second, end_pt.first, end_pt.second);
}

void QPainterEngine::AddRect(double x, double y, double w, double h)
{
  path_.addRect(x, y, w, h);
}

void QPainterEngine::AddCircle(double x, double y, double radius)
{
  path_.addEllipse(x - radius, y - radius, radius * 2, radius * 2);
}

void QPainterEngine::MoveTo(const std::pair<double, double> &pt)
{
  path_.moveTo(pt.first, pt.second);
}

void QPainterEngine::LineTo(const std::pair<double, double> &pt)
{
  path_.lineTo(pt.first, pt.second);
}

void QPainterEngine::BeginDrawOutline() { PreparePenAndBrush(); }

void QPainterEngine::EndDrawOutline()
{
  current_painter_->drawPath(path_);
  path_ = QPainterPath();
}

void QPainterEngine::BeginDrawStroke(std::shared_ptr<Aperture> aperture)
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

void QPainterEngine::CachePoint(const std::pair<double, double> &pt)
{
  rect_x_ = pt.first;
  rect_y_ = pt.second;
}

std::pair<double, double> QPainterEngine::CachedPoint() const
{
  return std::make_pair(rect_x_, rect_y_);
}

void QPainterEngine::EndDrawStroke()
{
  current_painter_->drawPath(path_);
  path_ = QPainterPath();
}

void QPainterEngine::EndDrawNewAperture() { current_painter_ = painter_; }

std::shared_ptr<QPixmap> QPainterEngine::PrepareExistAperture(int code)
{
  if (aperture_imgs_.find(code) != aperture_imgs_.end())
  {
    return aperture_imgs_[code];
  }

  return nullptr;
}

void QPainterEngine::NewAperture(std::shared_ptr<Aperture> aperture)
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
