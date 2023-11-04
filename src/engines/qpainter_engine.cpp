#include "qpainter_engine.h"
#include "aperture/aperture.h"
#include "gerber/gerber_layer.h"

#include <QPainter>

QPainterEngine::QPainterEngine(QPaintDevice *device, const BoundBox &bound_box,
                               double offset)
    : background_(0, 0, 0),
      foreground_(255, 255, 255),
      pic_(device),
      trans_(bound_box.Scaled(kTimes), offset) {
  trans_.SetPhysicalSize(pic_->width(), pic_->height());
}

void QPainterEngine::Resize() {
  trans_.SetPhysicalSize(pic_->width(), pic_->height());
}

void QPainterEngine::Rotate(int angle) { rotation_ = angle % 360; }

void QPainterEngine::Scale(double delta, double center_x, double center_y) {
  if (scale_ + delta < 0.2 || scale_ + delta > 10) {
    return;
  }

  scale_ += delta;

  if (center_x < -1000000 && center_y < -1000000) {
    scale_center_ = QPoint();
  } else {
    scale_center_ = QPoint(center_x, center_y);
  }

  aperture_imgs_.clear();
}

void QPainterEngine::Move(int delta_x, int delta_y) {
  movement_ += QPoint(delta_x, delta_y);
}

void QPainterEngine::SetConvertStroke2Fills(bool value) {
  convert_strokes2fills_ = value;
}

void QPainterEngine::Reset() {
  movement_ = QPoint();

  scale_ = 1.0;
  scale_center_ = QPoint(0, 0);
  rotation_ = 0;
}

void QPainterEngine::BeginRender() {
  painter_ = CreatePainter(pic_);
  current_painter_ = painter_;
}

QRect QPainterEngine::PainterWindow() { return trans_.GetPainterWindow(); }

std::shared_ptr<QPainter> QPainterEngine::CreatePainter(QPaintDevice *pic) {
  auto painter = std::make_shared<QPainter>(pic);
  painter->setWindow(trans_.GetPainterWindow());
  painter->setViewport(trans_.GetPainterViewport());
  painter->translate(trans_.GetPainterWindow().center());
  painter->rotate(rotation_);
  painter->translate(-trans_.GetPainterWindow().center());

  QPoint scale_center;
  if (scale_center_.isNull()) {
    scale_center = trans_.GetPainterWindow().center();
  } else {
    scale_center = painter->combinedTransform().inverted().map(scale_center_);
  }
  painter->translate(scale_center);
  painter->scale(scale_, scale_);
  painter->translate(-scale_center);

  QPoint origin(painter->deviceTransform().inverted().map(QPoint(0, 0)));
  QPoint movement(painter->deviceTransform().inverted().map(movement_));
  painter->translate(movement - origin);

  painter->setClipRect(painter->window());

  return painter;
}

void QPainterEngine::EndRender() {
  painter_ = nullptr;
  current_painter_ = nullptr;
}

void QPainterEngine::DrawBackground(const QColor &color) {
  QPainter painter(pic_);
  painter.setBrush(color);
  painter.drawRect(0, 0, pic_->width(), pic_->height());
}

int QPainterEngine::RenderGerber(const std::shared_ptr<Gerber> &gerber) {
  BeginRender();

  auto layers = gerber->GetLayers();
  for (const auto &layer : layers) {
    negative_ = layer->IsNegative();

    int ret = 0;
    if (layer->IsCopyLayer()) {
      const auto step_x = kTimes * layer->step_x_;
      const auto step_y = kTimes * layer->step_y_;

      current_painter_->save();
      for (int y = 0; y < layer->count_y_; ++y) {
        for (int x = 0; x < layer->count_x_; ++x) {
          ret = layer->Draw(this);
          current_painter_->translate(step_x, 0);
        }

        current_painter_->translate(-step_x * layer->count_x_, step_y);
      }
      current_painter_->restore();
    } else {
      ret = layer->Draw(this);
    }

    if (ret) {
      return ret;
    }
  }

  EndRender();

  return 0;
}

void QPainterEngine::PreparePenAndBrush() {
  current_painter_->setPen(QPen(Qt::transparent, 1.0));

  if (negative_) {
    current_painter_->setBrush(background_);
  } else {
    current_painter_->setBrush(foreground_);
  }
}

void QPainterEngine::PreparePenAndBrushForStroke(double line_width) {
  current_painter_->setBrush(Qt::transparent);

  auto pen = current_painter_->pen();
  if (negative_) {
    pen.setColor(background_);
  } else {
    pen.setColor(foreground_);
  }
  pen.setWidthF(line_width);
  current_painter_->setPen(pen);
}

void QPainterEngine::DrawAperture(Aperture *aperture,
                                  const std::pair<double, double> &start) {
  if (!PrepareExistAperture(aperture->Code())) {
    NewAperture(aperture);
    aperture->Draw(this);
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

std::pair<double, double> QPainterEngine::CurrentPos() const {
  return std::make_pair(path_.currentPosition().x() / kTimes,
                        path_.currentPosition().y() / kTimes);
}

void QPainterEngine::CubicTo(const std::pair<double, double> &ctrl_pt1,
                             const std::pair<double, double> &ctrl_pt2,
                             const std::pair<double, double> &end_pt) {
  path_.cubicTo(ctrl_pt1.first * kTimes, ctrl_pt1.second * kTimes,
                ctrl_pt2.first * kTimes, ctrl_pt2.second * kTimes,
                end_pt.first * kTimes, end_pt.second * kTimes);
}

void QPainterEngine::AddRect(double x, double y, double w, double h) {
  path_.addRect(x * kTimes, y * kTimes, w * kTimes, h * kTimes);
  current_painter_->drawPath(path_);
  path_ = QPainterPath();
}

void QPainterEngine::AddCircle(double x, double y, double radius) {
  path_.addEllipse((x - radius) * kTimes, (y - radius) * kTimes,
                   radius * 2 * kTimes, radius * 2 * kTimes);
  current_painter_->drawPath(path_);
  path_ = QPainterPath();
}

void QPainterEngine::MoveTo(const std::pair<double, double> &pt) {
  path_.moveTo(pt.first * kTimes, pt.second * kTimes);
}

void QPainterEngine::LineTo(const std::pair<double, double> &pt) {
  path_.lineTo(pt.first * kTimes, pt.second * kTimes);
}

void QPainterEngine::BeginDrawOutline() { PreparePenAndBrush(); }

void QPainterEngine::EndDrawOutline() {
  current_painter_->drawPath(path_);
  path_ = QPainterPath();
}

void QPainterEngine::BeginDrawStroke(Aperture *aperture) {
  if (aperture->SolidCircle()) {
    PreparePenAndBrushForStroke(aperture->BBox().Width() * kTimes);
  } else if (aperture->SolidRectangle()) {
    PreparePenAndBrush();
  } else {
  }
}

void QPainterEngine::CachePoint(const std::pair<double, double> &pt) {
  rect_x_ = pt.first;
  rect_y_ = pt.second;
}

std::pair<double, double> QPainterEngine::CachedPoint() const {
  return std::make_pair(rect_x_, rect_y_);
}

void QPainterEngine::EndDrawStroke() {
  current_painter_->drawPath(path_);
  path_ = QPainterPath();
}

void QPainterEngine::EndDrawNewAperture() { current_painter_ = painter_; }

std::shared_ptr<QPixmap> QPainterEngine::PrepareExistAperture(int code) {
  if (aperture_imgs_.find(code) != aperture_imgs_.end()) {
    return aperture_imgs_[code];
  }

  return nullptr;
}

void QPainterEngine::NewAperture(Aperture *aperture) {
  const auto bound_box = aperture->BBox();
  const auto left = bound_box.Left() * kTimes;
  const auto right = bound_box.Right() * kTimes;
  const auto top = bound_box.Top() * kTimes;
  const auto bottom = bound_box.Bottom() * kTimes;

  const auto left_top = painter_->deviceTransform().map(QPoint(left, top));
  const auto right_bottom =
      painter_->deviceTransform().map(QPoint(right, bottom));
  const auto width = (right_bottom - left_top).x();
  const auto height = (right_bottom - left_top).y();

  auto img = std::make_shared<QPixmap>(width, height);
  img->fill(Qt::transparent);
  aperture_imgs_[aperture->Code()] = img;
  current_painter_ = std::make_shared<QPainter>(img.get());
  current_painter_->setWindow(left, bottom, right - left, top - bottom);
  current_painter_->setPen(QPen(Qt::transparent, 1.0));
  if (negative_) {
    current_painter_->setBrush(background_);
  } else {
    current_painter_->setBrush(foreground_);
  }
}

QPoint QPainterEngine::Dev2Logic(const QPoint &dev_pos) {
  auto painter = CreatePainter(pic_);
  auto pos = painter->deviceTransform().inverted().map(dev_pos);
  return painter->deviceTransform().inverted().map(dev_pos) / kTimes;
}

QRect QPainterEngine::Dev2Logic(const QRect &rect) {
  auto painter = CreatePainter(pic_);
  const auto top_left =
      painter->deviceTransform().inverted().map(rect.topLeft());
  const auto bottom_right =
      painter->deviceTransform().inverted().map(rect.bottomRight());
  return QRect(top_left, bottom_right);
}
