#pragma once
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <blend2d.h>

#include "engine/engine.h"
#include "engine_common.h"
#include "gerber_parser/bound_box.h"

class Aperture;
class Gerber;
class BoundBox;

class Blend2DEngine : public Engine {
 public:
  static constexpr int kTimes = gerber_engine::kTimes;

  Blend2DEngine(BLImage &image, const BoundBox &bound_box, double offset);
  ~Blend2DEngine();

  // 渲染入口（Begin/EndRender 由构造/析构隐式管理，不对外暴露）
  int RenderGerber(const std::shared_ptr<Gerber> &gerber);
  void DrawBackground(const BLRgba32 &color = BLRgba32(0, 0, 0));
  void SetConvertStroke2Fills(bool value);

  // 观测面：返回当前累积 path 的拷贝（设计文档授权的 BLPath 结构断言入口，非 mock）
  BLPath CurrentOutlinePath() const;

  // —— Engine 接口全部方法（override）
  std::pair<double, double> CurrentPos() const override;
  void CubicTo(const std::pair<double, double> &ctrl_pt1,
               const std::pair<double, double> &ctrl_pt2,
               const std::pair<double, double> &end_pt) override;
  void AddRect(double x, double y, double w, double h) override;
  void AddCircle(double x, double y, double radius) override;
  void MoveTo(const std::pair<double, double> &pt) override;
  void LineTo(const std::pair<double, double> &pt) override;
  void DrawAperture(Aperture *aperture,
                    const std::pair<double, double> &start) override;
  void BeginDrawOutline() override;
  void EndDrawOutline() override;
  void BeginDrawStroke(Aperture *aperture) override;
  void CachePoint(const std::pair<double, double> &pt) override;
  std::pair<double, double> CachedPoint() const override;
  void EndDrawStroke() override;

 private:
  // HOW：成员自定（至少含 BLPath path_、BLContext 生命周期、BLRgba32 前景/背景、
  //       bool negative_、缓存点 pair<double,double>、image 引用）
  BLImage &image_;
  BLContext ctx_;
  BLPath path_;

  BoundBox bound_box_;  // 逻辑窗口（默认非缩放空间，供视图映射使用）
  double offset_;

  BLRgba32 background_{0, 0, 0};
  BLRgba32 foreground_{255, 255, 255};
  bool negative_{false};
  bool stroke_mode_{false};  // BeginDrawStroke 判定：true=strokePath / false=fillPath
  std::pair<double, double> cached_point_{0.0, 0.0};
};

namespace blend2d_semantics {
// 逻辑窗口 ↔ 物理视口映射（负高度窗口语义：height<0 表示 Y 轴翻转视口）
struct ViewTransform {
  double left, top, width, height;          // 逻辑窗口（bound_box.Scale(kTimes) 空间，负高度语义=Y 轴翻转视口）
  double viewport_left, viewport_top,
         viewport_width, viewport_height;   // 物理视口
};
ViewTransform ComputeViewTransform(const BoundBox &scaled_box, double offset,
                                   int physical_width, int physical_height);

// step-and-repeat 平移序列（y 外层、x 内层）
std::vector<std::pair<double, double>> StepRepeatTranslations(
    int count_x, int count_y, double step_x, double step_y);

// 负片/clear 前景背景互换：negative ? background : foreground
BLRgba32 ResolveFillColor(bool negative, const BLRgba32 &background,
                          const BLRgba32 &foreground);

// 构造已按 kTimes 缩放坐标的矩形/圆形 path（AddRect/AddCircle 的构建内核）
BLPath MakeRectPath(double x, double y, double w, double h);
BLPath MakeCirclePath(double x, double y, double radius);
}  // namespace blend2d_semantics
