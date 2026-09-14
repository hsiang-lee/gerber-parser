#include "blend2d_engine.h"

#include <cmath>

#include "aperture/aperture.h"
#include "gerber/gerber.h"
#include "gerber/gerber_layer.h"

Blend2DEngine::Blend2DEngine(BLImage &image, const BoundBox &bound_box,
                             double offset)
    : image_(image), bound_box_(bound_box), offset_(offset) {
  ctx_.begin(image_);

  // 窗口→视口仿射（含 Y 翻转）set 到 context：路径顶点保持 kTimes 逻辑空间。
  // x_dev = viewport_left + (x−left)×sx, y_dev = viewport_top + (top−y)×sy
  // （负高度窗口语义：height<0 表示 Y 轴翻转视口）
  const auto vt = blend2d_semantics::ComputeViewTransform(
      bound_box_.Scaled(kTimes), offset_, image_.width(), image_.height());
  const double sx = vt.viewport_width / vt.width;
  const double sy = vt.viewport_height / vt.height;
  ctx_.setTransform(BLMatrix2D(sx, 0, 0, -sy,
                               vt.viewport_left - vt.left * sx,
                               vt.viewport_top + vt.top * sy));
}

Blend2DEngine::~Blend2DEngine() { ctx_.end(); }

int Blend2DEngine::RenderGerber(const std::shared_ptr<Gerber> &gerber) {
  // 渲染骨架：逐 layer，negative_ 互换；copy layer 用 StepRepeatTranslations
  // 平移序列（y 外 x 内序）驱动逐格绘制。非 copy layer 视为单次零平移。
  for (const auto &layer : gerber->GetLayers()) {
    negative_ = layer->IsNegative();

    auto translations = std::vector<std::pair<double, double>>{{0.0, 0.0}};
    if (layer->IsCopyLayer()) {
      translations = blend2d_semantics::StepRepeatTranslations(
          layer->count_x_, layer->count_y_, layer->step_x_, layer->step_y_);
    }

    for (const auto &t : translations) {
      ctx_.save();
      ctx_.translate(t.first * kTimes, t.second * kTimes);
      const int ret = layer->Draw(this);
      ctx_.restore();
      if (ret) {
        return ret;
      }
    }
  }

  return 0;
}

void Blend2DEngine::DrawBackground(const BLRgba32 &color) {
  // 背景在设备空间整图填充：临时回到单位阵（视图变换仅作用于几何绘制）
  ctx_.save();
  ctx_.resetTransform();
  ctx_.fillAll(color);
  ctx_.restore();
}

void Blend2DEngine::SetConvertStroke2Fills(bool value) {
  convert_strokes2fills_ = value;
}

BLPath Blend2DEngine::CurrentOutlinePath() const { return path_; }

std::pair<double, double> Blend2DEngine::CurrentPos() const {
  BLPoint last{0.0, 0.0};
  if (path_.size() > 0) {
    path_.getLastVertex(&last);
  }
  return std::make_pair(last.x / kTimes, last.y / kTimes);
}

void Blend2DEngine::CubicTo(const std::pair<double, double> &ctrl_pt1,
                            const std::pair<double, double> &ctrl_pt2,
                            const std::pair<double, double> &end_pt) {
  path_.cubicTo(ctrl_pt1.first * kTimes, ctrl_pt1.second * kTimes,
                ctrl_pt2.first * kTimes, ctrl_pt2.second * kTimes,
                end_pt.first * kTimes, end_pt.second * kTimes);
}

void Blend2DEngine::AddRect(double x, double y, double w, double h) {
  // MakeRectPath 构建（x,y 为矩形左下角，宽 w 高 h，顶点 ×kTimes）→ 立即填充
  ctx_.setFillStyle(blend2d_semantics::ResolveFillColor(
      negative_, background_, foreground_));
  ctx_.fillPath(blend2d_semantics::MakeRectPath(x, y, w, h));
  path_.reset();
}

void Blend2DEngine::AddCircle(double x, double y, double radius) {
  ctx_.setFillStyle(blend2d_semantics::ResolveFillColor(
      negative_, background_, foreground_));
  ctx_.fillPath(blend2d_semantics::MakeCirclePath(x, y, radius));
  path_.reset();
}

void Blend2DEngine::MoveTo(const std::pair<double, double> &pt) {
  path_.moveTo(pt.first * kTimes, pt.second * kTimes);
}

void Blend2DEngine::LineTo(const std::pair<double, double> &pt) {
  path_.lineTo(pt.first * kTimes, pt.second * kTimes);
}

void Blend2DEngine::DrawAperture(Aperture *aperture,
                                 const std::pair<double, double> &start) {
  // 矢量化渲染 aperture（其内部由 Primitives 组成）：平移 context 至 flash 位后走本引擎流程。
  ctx_.save();
  ctx_.translate(start.first * kTimes, start.second * kTimes);
  aperture->Draw(this);
  ctx_.restore();
  path_.reset();
}

void Blend2DEngine::BeginDrawOutline() {
  // 设置填充色（负片时以背景色填充）
  ctx_.setFillStyle(
      blend2d_semantics::ResolveFillColor(negative_, background_, foreground_));
  stroke_mode_ = false;
}

void Blend2DEngine::EndDrawOutline() {
  ctx_.fillPath(path_);
  path_.reset();
}

void Blend2DEngine::BeginDrawStroke(Aperture *aperture) {
  // SolidCircle → stroke 样式（线宽 = BBox().Width()×kTimes，RoundCap/RoundJoin）；
  // SolidRectangle → 填充模式（DrawRectLine 生成闭合多边形 path，EndDrawStroke 填充）。
  const auto color = blend2d_semantics::ResolveFillColor(
      negative_, background_, foreground_);
  if (aperture->SolidCircle()) {
    ctx_.setStrokeStyle(color);
    ctx_.setStrokeWidth(aperture->BBox().Width() * kTimes);
    ctx_.setStrokeCaps(BL_STROKE_CAP_ROUND);
    ctx_.setStrokeJoin(BL_STROKE_JOIN_ROUND);
    stroke_mode_ = true;
  } else {
    ctx_.setFillStyle(color);
    stroke_mode_ = false;
  }
}

void Blend2DEngine::CachePoint(const std::pair<double, double> &pt) {
  cached_point_ = pt;
}

std::pair<double, double> Blend2DEngine::CachedPoint() const {
  return cached_point_;
}

void Blend2DEngine::EndDrawStroke() {
  if (stroke_mode_) {
    ctx_.strokePath(path_);
  } else {
    ctx_.fillPath(path_);
  }
  stroke_mode_ = false;
  path_.reset();
}

namespace blend2d_semantics {

ViewTransform ComputeViewTransform(const BoundBox &scaled_box, double offset,
                                   int physical_width, int physical_height) {
  // 逻辑窗口 → 物理视口映射（含留边 offset 与等比缩放居中）
  ViewTransform vt;
  const double physical_w = physical_width * (1 - 2 * offset);
  const double physical_h = physical_height * (1 - 2 * offset);
  const double scale_x = physical_w / scaled_box.Width();
  const double scale_y = physical_h / scaled_box.Height();

  if (scale_x < scale_y) {
    vt.left = scaled_box.Left();
    vt.top =
        scaled_box.Top() + scaled_box.Height() * (scale_y / scale_x - 1) / 2;
    vt.width = scaled_box.Width();
    vt.height = scaled_box.Height() * (scale_y / scale_x);
  } else {
    double margin =
        (scale_x / scale_y * scaled_box.Width() - scaled_box.Width()) / 2;
    vt.left = scaled_box.Left() - margin;
    vt.top = scaled_box.Top();
    vt.width = scaled_box.Width() * (scale_x / scale_y);
    vt.height = scaled_box.Height();
  }

  vt.viewport_left = physical_width * offset;
  vt.viewport_top = physical_height * offset;
  vt.viewport_width = physical_w;
  vt.viewport_height = physical_h;
  return vt;
}

std::vector<std::pair<double, double>> StepRepeatTranslations(
    int count_x, int count_y, double step_x, double step_y) {
  // y 外层、x 内层循环序
  std::vector<std::pair<double, double>> translations;
  for (int y = 0; y < count_y; ++y) {
    for (int x = 0; x < count_x; ++x) {
      translations.emplace_back(x * step_x, y * step_y);
    }
  }
  return translations;
}

BLRgba32 ResolveFillColor(bool negative, const BLRgba32 &background,
                          const BLRgba32 &foreground) {
  return negative ? background : foreground;
}

BLPath MakeRectPath(double x, double y, double w, double h) {
  // 4 顶点开口矩形（不调用 close()：blPathClose 会追加第 5 个顶点；
  // 填充时 Blend2D 视开口 path 为隐式闭合）。
  BLPath path;
  path.moveTo(x * gerber_engine::kTimes, y * gerber_engine::kTimes);
  path.lineTo((x + w) * gerber_engine::kTimes, y * gerber_engine::kTimes);
  path.lineTo((x + w) * gerber_engine::kTimes,
              (y + h) * gerber_engine::kTimes);
  path.lineTo(x * gerber_engine::kTimes, (y + h) * gerber_engine::kTimes);
  return path;
}

BLPath MakeCirclePath(double x, double y, double radius) {
  // 圆用圆周采样多边形近似：顶点全部落在圆周上（BLPath::addCircle/arcTo
  // 存储的是贝塞尔控制点，不满足"所有顶点到圆心距离==半径"合约，故不用）。
  // 采样点数取 kCircleSegments，闭合由填充规则处理（隐式闭合）。
  constexpr int kCircleSegments = 64;
  const double cx = x * gerber_engine::kTimes;
  const double cy = y * gerber_engine::kTimes;
  const double r = radius * gerber_engine::kTimes;
  BLPath path;
  for (int i = 0; i < kCircleSegments; ++i) {
    const double angle = 2.0 * 3.14159265358979323846 * i / kCircleSegments;
    if (i == 0) {
      path.moveTo(cx + r * std::cos(angle), cy + r * std::sin(angle));
    } else {
      path.lineTo(cx + r * std::cos(angle), cy + r * std::sin(angle));
    }
  }
  return path;
}

}  // namespace blend2d_semantics
