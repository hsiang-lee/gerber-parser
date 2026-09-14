#include "engines/blend2d_engine.h"
#include "image_diff.h"
#include "test_helpers.h"

#include <cmath>
#include <string>

#include "gerber/gerber.h"
#include "gerber/gerber_layer.h"
#include "gerber_parser/gerber_parser.h"
#include <gtest/gtest.h>

namespace {

class Blend2DEngineTest : public testing::Test {
 protected:
  // 注：本机 blend2d 枚举成员为 C 风格 BL_FORMAT_PRGB32（计划中 BLFormat::PRGB32
  //     为 API 适配偏差，语义等价）。
  Blend2DEngineTest()
      : img_(64, 64, BL_FORMAT_PRGB32),
        engine_(img_, BoundBox(0.001, 0.021, 0.021, 0.001), 0.0) {}

  BLImage img_;
  Blend2DEngine engine_;
};

// 以 T5 的 ComputeViewTransform 将逻辑毫米坐标映射到设备像素。
// 映射与引擎内部矩阵一致：x_dev=(x−left)×sx+vp_left, y_dev=(top−y)×sy+vp_top。
std::pair<int, int> MapToDevicePixels(const BoundBox &bbox_mm, double offset,
                                      int physical_width, int physical_height,
                                      double mm_x, double mm_y) {
  auto vt = blend2d_semantics::ComputeViewTransform(
      bbox_mm.Scaled(Blend2DEngine::kTimes), offset, physical_width,
      physical_height);
  const double sx = vt.viewport_width / vt.width;
  const double sy = vt.viewport_height / vt.height;
  const double dx =
      vt.viewport_left + (mm_x * Blend2DEngine::kTimes - vt.left) * sx;
  const double dy =
      vt.viewport_top + (vt.top - mm_y * Blend2DEngine::kTimes) * sy;
  return {static_cast<int>(std::lround(dx)),
          static_cast<int>(std::lround(dy))};
}

}  // namespace

// —— T4: MoveTo/LineTo/CubicTo/CurrentPos/CachePoint/CachedPoint 结构断言 ——

TEST_F(Blend2DEngineTest, EmptyEngineOutlinePathHasZeroVertices) {
  EXPECT_EQ(engine_.CurrentOutlinePath().size(), 0u);
}

TEST_F(Blend2DEngineTest, MoveToAppendsSingleVertexScaledByKtimes) {
  engine_.MoveTo({1.0, 2.0});

  BLPath path = engine_.CurrentOutlinePath();
  ASSERT_EQ(path.size(), 1u);
  EXPECT_DOUBLE_EQ(path.vertexData()[0].x, 10000.0);
  EXPECT_DOUBLE_EQ(path.vertexData()[0].y, 20000.0);
}

TEST_F(Blend2DEngineTest, LineToAppendsScaledEndVertex) {
  engine_.MoveTo({0.0, 0.0});
  engine_.LineTo({3.0, 4.0});

  BLPath path = engine_.CurrentOutlinePath();
  ASSERT_EQ(path.size(), 2u);
  EXPECT_DOUBLE_EQ(path.vertexData()[1].x, 30000.0);
  EXPECT_DOUBLE_EQ(path.vertexData()[1].y, 40000.0);
}

TEST_F(Blend2DEngineTest, CubicToAppendsTwoControlAndEndScaledVertices) {
  engine_.MoveTo({0.0, 0.0});
  engine_.CubicTo({1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0});

  BLPath path = engine_.CurrentOutlinePath();
  ASSERT_EQ(path.size(), 4u);  // 起点 + 2 控制点 + 终点
  EXPECT_DOUBLE_EQ(path.vertexData()[1].x, 10000.0);
  EXPECT_DOUBLE_EQ(path.vertexData()[1].y, 10000.0);
  EXPECT_DOUBLE_EQ(path.vertexData()[2].x, 20000.0);
  EXPECT_DOUBLE_EQ(path.vertexData()[2].y, 20000.0);
  EXPECT_DOUBLE_EQ(path.vertexData()[3].x, 30000.0);
  EXPECT_DOUBLE_EQ(path.vertexData()[3].y, 30000.0);
}

TEST_F(Blend2DEngineTest, CurrentPosReturnsLogicalEndPoint) {
  engine_.MoveTo({0.0, 0.0});
  engine_.CubicTo({1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0});

  auto pos = engine_.CurrentPos();
  EXPECT_DOUBLE_EQ(pos.first, 3.0);   // 除以 kTimes 还原逻辑坐标
  EXPECT_DOUBLE_EQ(pos.second, 3.0);
}

TEST_F(Blend2DEngineTest, CurrentPosAfterOnlyMoveTo) {
  engine_.MoveTo({1.0, 2.0});

  auto pos = engine_.CurrentPos();
  EXPECT_DOUBLE_EQ(pos.first, 1.0);
  EXPECT_DOUBLE_EQ(pos.second, 2.0);
}

TEST_F(Blend2DEngineTest, CachedPointDefaultsToZero) {
  auto pt = engine_.CachedPoint();
  EXPECT_DOUBLE_EQ(pt.first, 0.0);
  EXPECT_DOUBLE_EQ(pt.second, 0.0);
}

TEST_F(Blend2DEngineTest, CachePointStoresLogicalCoords) {
  engine_.CachePoint({7.5, -3.25});

  auto pt = engine_.CachedPoint();
  EXPECT_DOUBLE_EQ(pt.first, 7.5);
  EXPECT_DOUBLE_EQ(pt.second, -3.25);
}

// —— T5: blend2d_semantics 语义纯函数数值断言 ——

bool PathHasVertex(const BLPath &path, double x, double y) {
  for (size_t i = 0; i < path.size(); ++i) {
    if (path.vertexData()[i].x == x && path.vertexData()[i].y == y) {
      return true;
    }
  }
  return false;
}

TEST(Blend2DSemanticsTest, ComputeViewTransform_NonSquareBox) {
  // 20×10（Scaled(kTimes) 空间）→ 物理 1600×1600，offset 0.005
  auto vt = blend2d_semantics::ComputeViewTransform(
      BoundBox(0.0, 200000.0, 100000.0, 0.0), 0.005, 1600, 1600);

  EXPECT_DOUBLE_EQ(vt.left, 0.0);
  EXPECT_DOUBLE_EQ(vt.top, 150000.0);
  EXPECT_DOUBLE_EQ(vt.width, 200000.0);
  EXPECT_DOUBLE_EQ(vt.height, 200000.0);
  EXPECT_DOUBLE_EQ(vt.viewport_left, 8.0);
  EXPECT_DOUBLE_EQ(vt.viewport_top, 8.0);
  EXPECT_DOUBLE_EQ(vt.viewport_width, 1584.0);
  EXPECT_DOUBLE_EQ(vt.viewport_height, 1584.0);
}

TEST(Blend2DSemanticsTest, ComputeViewTransform_SquareBox) {
  auto vt = blend2d_semantics::ComputeViewTransform(
      BoundBox(0.0, 100000.0, 100000.0, 0.0), 0.0, 64, 64);

  EXPECT_DOUBLE_EQ(vt.left, 0.0);
  EXPECT_DOUBLE_EQ(vt.top, 100000.0);
  EXPECT_DOUBLE_EQ(vt.width, 100000.0);
  EXPECT_DOUBLE_EQ(vt.height, 100000.0);
  EXPECT_DOUBLE_EQ(vt.viewport_left, 0.0);
  EXPECT_DOUBLE_EQ(vt.viewport_top, 0.0);
  EXPECT_DOUBLE_EQ(vt.viewport_width, 64.0);
  EXPECT_DOUBLE_EQ(vt.viewport_height, 64.0);
}

TEST(Blend2DSemanticsTest, StepRepeatTranslations_YOuterXInnerOrder) {
  auto ts = blend2d_semantics::StepRepeatTranslations(2, 3, 10.0, 20.0);

  ASSERT_EQ(ts.size(), 6u);
  EXPECT_DOUBLE_EQ(ts[0].first, 0.0);
  EXPECT_DOUBLE_EQ(ts[0].second, 0.0);
  EXPECT_DOUBLE_EQ(ts[1].first, 10.0);
  EXPECT_DOUBLE_EQ(ts[1].second, 0.0);
  EXPECT_DOUBLE_EQ(ts[2].first, 0.0);
  EXPECT_DOUBLE_EQ(ts[2].second, 20.0);
  EXPECT_DOUBLE_EQ(ts[3].first, 10.0);
  EXPECT_DOUBLE_EQ(ts[3].second, 20.0);
  EXPECT_DOUBLE_EQ(ts[4].first, 0.0);
  EXPECT_DOUBLE_EQ(ts[4].second, 40.0);
  EXPECT_DOUBLE_EQ(ts[5].first, 10.0);
  EXPECT_DOUBLE_EQ(ts[5].second, 40.0);
}

TEST(Blend2DSemanticsTest, StepRepeatTranslations_SingleCell) {
  auto ts = blend2d_semantics::StepRepeatTranslations(1, 1, 10.0, 20.0);

  ASSERT_EQ(ts.size(), 1u);
  EXPECT_DOUBLE_EQ(ts[0].first, 0.0);
  EXPECT_DOUBLE_EQ(ts[0].second, 0.0);
}

TEST(Blend2DSemanticsTest, StepRepeatTranslations_ZeroCountReturnsEmpty) {
  EXPECT_TRUE(blend2d_semantics::StepRepeatTranslations(0, 3, 10.0, 20.0).empty());
  EXPECT_TRUE(blend2d_semantics::StepRepeatTranslations(2, 0, 10.0, 20.0).empty());
}

TEST(Blend2DSemanticsTest, ResolveFillColor_PositiveReturnsForeground) {
  auto color = blend2d_semantics::ResolveFillColor(
      false, BLRgba32(0xFF000000), BLRgba32(0xFFFFFFFF));
  EXPECT_EQ(color.value, 0xFFFFFFFFu);
}

TEST(Blend2DSemanticsTest, ResolveFillColor_NegativeReturnsBackground) {
  auto color = blend2d_semantics::ResolveFillColor(
      true, BLRgba32(0xFF000000), BLRgba32(0xFFFFFFFF));
  EXPECT_EQ(color.value, 0xFF000000u);
}

TEST(Blend2DSemanticsTest, MakeRectPath_CoversFourScaledCorners) {
  auto path = blend2d_semantics::MakeRectPath(0.001, 0.001, 0.010, 0.010);

  EXPECT_EQ(path.size(), 4u);
  EXPECT_TRUE(PathHasVertex(path, 10.0, 10.0));
  EXPECT_TRUE(PathHasVertex(path, 110.0, 10.0));
  EXPECT_TRUE(PathHasVertex(path, 110.0, 110.0));
  EXPECT_TRUE(PathHasVertex(path, 10.0, 110.0));
}

TEST(Blend2DSemanticsTest, MakeCirclePath_VerticesOnScaledRadius) {
  auto path = blend2d_semantics::MakeCirclePath(0.010, 0.010, 0.005);

  ASSERT_GE(path.size(), 8u);
  for (size_t i = 0; i < path.size(); ++i) {
    double dx = path.vertexData()[i].x - 100.0;
    double dy = path.vertexData()[i].y - 100.0;
    EXPECT_NEAR(std::hypot(dx, dy), 50.0, 1e-6);
  }
}

// —— T6: AddRect/AddCircle 立即填充 + Begin/EndDrawOutline 累积填充 ——
// 夹具映射：窗口 kTimes 空间 (10..210)，物理 64×64，offset 0
//           x_dev=(x−10)×0.32，y_dev=(210−y)×0.32

TEST_F(Blend2DEngineTest, AddRectImmediatelyFillsRectAndClearsPath) {
  engine_.DrawBackground();
  // 逻辑 kTimes 50..150 → 设备 x 12.8..44.8、y 19.2..51.2
  engine_.AddRect(0.005, 0.005, 0.010, 0.010);

  EXPECT_EQ(engine_.CurrentOutlinePath().size(), 0u);  // 立即清空语义
  EXPECT_EQ(PixelAt(img_, 32, 32), 0xFFFFFFFFu);      // 矩形中心 == 前景白
  EXPECT_EQ(PixelAt(img_, 0, 0), 0xFF000000u);        // (0,0) 矩形外 == 背景黑
  EXPECT_EQ(PixelAt(img_, 8, 8), 0xFF000000u);        // (8,8) 矩形外 == 黑
}

TEST_F(Blend2DEngineTest, AddCircleImmediatelyFillsCircleAndClearsPath) {
  engine_.DrawBackground();
  // 圆心 kTimes (110,110) → 设备 (32,32)，半径 50 单位 → 16px
  engine_.AddCircle(0.010, 0.010, 0.005);

  EXPECT_EQ(engine_.CurrentOutlinePath().size(), 0u);  // 立即清空语义
  EXPECT_EQ(PixelAt(img_, 32, 32), 0xFFFFFFFFu);      // 圆心 == 白
  EXPECT_EQ(PixelAt(img_, 32, 2), 0xFF000000u);       // 中心上方 30px，半径外 == 黑
}

TEST_F(Blend2DEngineTest, OutlineAccumulatesUntilEndDrawOutline) {
  engine_.DrawBackground();
  engine_.BeginDrawOutline();
  engine_.MoveTo({0.003, 0.003});
  engine_.LineTo({0.017, 0.003});
  engine_.LineTo({0.017, 0.017});
  engine_.LineTo({0.003, 0.017});

  // EndDrawOutline **之前** path 未清空
  EXPECT_EQ(engine_.CurrentOutlinePath().size(), 4u);
  engine_.EndDrawOutline();

  // EndDrawOutline 后已填充并清空
  EXPECT_EQ(engine_.CurrentOutlinePath().size(), 0u);
  // 矩形设备范围 x∈[6.4,51.2]、y∈[12.8,57.6]（映射见夹具注）
  EXPECT_EQ(PixelAt(img_, 30, 30), 0xFFFFFFFFu);  // 矩形内 == 白
  EXPECT_EQ(PixelAt(img_, 62, 62), 0xFF000000u);  // 矩形外 == 黑
}

// —— T7: DrawAperture + Begin/EndDrawStroke（夹具 blend2d_unit_circle_line） ——
// 夹具语义：D10=圆孔 Ø0.4mm；flash@(1,1)；G01 后 D01 画线 (1,1)→(2,2) 线宽 0.4mm。

namespace {

std::shared_ptr<Gerber> ParseCircleLineFixture() {
  return ParseTestGerberFile("blend2d_unit_circle_line.gbr");
}

}  // namespace

TEST(Blend2DFixtureTest, CircleLineFixtureParsesWithExpectedBBox) {
  auto gerber = ParseCircleLineFixture();

  ASSERT_NE(gerber, nullptr);
  EXPECT_NEAR(gerber->GetBBox().Right(), 2.2, 0.01);
  EXPECT_NEAR(gerber->GetBBox().Top(), 2.2, 0.01);
}

TEST(Blend2DFlashTest, DrawApertureFlashLandsAtMappedCenter) {
  auto gerber = ParseCircleLineFixture();
  auto aperture = gerber->GetAperture(10);
  ASSERT_NE(aperture, nullptr);

  const auto bbox = gerber->GetBBox();
  BLImage img(512, 512, BL_FORMAT_PRGB32);
  Blend2DEngine engine(img, bbox, 0.005);
  engine.DrawBackground();

  engine.DrawAperture(aperture, {1.0, 1.0});

  // flash 中心 (1,1) → 设备白；距中心 0.3mm > 半径 0.2mm → 黑
  auto center = MapToDevicePixels(bbox, 0.005, 512, 512, 1.0, 1.0);
  EXPECT_EQ(PixelAt(img, center.first, center.second), 0xFFFFFFFFu);
  auto outside = MapToDevicePixels(bbox, 0.005, 512, 512, 1.3, 1.0);
  EXPECT_EQ(PixelAt(img, outside.first, outside.second), 0xFF000000u);
}

TEST(Blend2DFlashTest, StrokeSolidCircleDrawsLineWithWidth) {
  auto gerber = ParseCircleLineFixture();

  const auto bbox = gerber->GetBBox();
  BLImage img(512, 512, BL_FORMAT_PRGB32);
  Blend2DEngine engine(img, bbox, 0.005);
  engine.DrawBackground();

  // 全程走通 Flash + Stroke（BeginDrawStroke→MoveTo/LineTo→EndDrawStroke 全链）
  ASSERT_EQ(gerber->GetLayers()[0]->Draw(&engine), 0);

  // 线中点 (1.5,1.5) 被 0.4mm 线宽覆盖 → 白
  auto mid = MapToDevicePixels(bbox, 0.005, 512, 512, 1.5, 1.5);
  EXPECT_EQ(PixelAt(img, mid.first, mid.second), 0xFFFFFFFFu);

  // 垂直方向 0.3mm 外（> 半宽 0.2mm）→ 黑
  const double off = 0.3 / std::sqrt(2.0);
  auto side = MapToDevicePixels(bbox, 0.005, 512, 512, 1.5 - off, 1.5 + off);
  EXPECT_EQ(PixelAt(img, side.first, side.second), 0xFF000000u);
}

// —— T8: RenderGerber（逐 layer + negative 互换）+ DrawBackground 全图 ——

TEST(Blend2DRenderTest, DrawBackgroundFillsWholeImage) {
  // DrawBackground 由 T7 前置实现（全图断言作为回归保护，RED 阶段即通过）
  BLImage img(64, 64, BL_FORMAT_PRGB32);
  Blend2DEngine engine(img, BoundBox(0.0, 1.0, 1.0, 0.0), 0.0);

  engine.DrawBackground();
  for (int y = 0; y < 64; ++y) {
    for (int x = 0; x < 64; ++x) {
      EXPECT_EQ(PixelAt(img, x, y), 0xFF000000u);
    }
  }

  engine.DrawBackground(BLRgba32(0xFFFFFFFF));
  for (int y = 0; y < 64; ++y) {
    for (int x = 0; x < 64; ++x) {
      EXPECT_EQ(PixelAt(img, x, y), 0xFFFFFFFFu);
    }
  }
}

TEST(Blend2DRenderTest, RenderGerberPositiveLayerFillsFlashCenter) {
  auto gerber = ParseTestGerberFile("blend2d_unit_positive.gbr");
  ASSERT_NE(gerber, nullptr);
  const auto bbox = gerber->GetBBox();
  ASSERT_GT(bbox.Width(), 0.0);
  ASSERT_GT(bbox.Height(), 0.0);

  BLImage img(512, 512, BL_FORMAT_PRGB32);
  Blend2DEngine engine(img, bbox, 0.005);
  engine.DrawBackground();
  EXPECT_EQ(engine.RenderGerber(gerber), 0);

  // flash 中心 (1.0,1.0) 设备像素 == 前景白
  auto center = MapToDevicePixels(bbox, 0.005, 512, 512, 1.0, 1.0);
  EXPECT_EQ(PixelAt(img, center.first, center.second), 0xFFFFFFFFu);
  // 图像左下角（bbox 外）保持背景黑
  EXPECT_EQ(PixelAt(img, 0, 0), 0xFF000000u);
}

TEST(Blend2DRenderTest, RenderGerberNegativeLayerExchangesFillColor) {
  auto gerber = ParseTestGerberFile("blend2d_unit_negative.gbr");
  ASSERT_NE(gerber, nullptr);
  auto layers = gerber->GetLayers();
  ASSERT_GE(layers.size(), 1u);
  ASSERT_TRUE(layers[0]->IsNegative());  // LPC 夹具解析为负片语义
  const auto bbox = gerber->GetBBox();
  ASSERT_GT(bbox.Width(), 0.0);
  ASSERT_GT(bbox.Height(), 0.0);

  BLImage img(512, 512, BL_FORMAT_PRGB32);
  Blend2DEngine engine(img, bbox, 0.005);
  engine.DrawBackground();
  EXPECT_EQ(engine.RenderGerber(gerber), 0);

  // 负片 flash 中心以背景色（黑）填充 = 擦除语义，绝非前景白
  auto center = MapToDevicePixels(bbox, 0.005, 512, 512, 1.0, 1.0);
  EXPECT_EQ(PixelAt(img, center.first, center.second), 0xFF000000u);
  // 图像左上角（bbox 外）背景未被破坏
  EXPECT_EQ(PixelAt(img, 0, 0), 0xFF000000u);
}

TEST(Blend2DRenderTest, RenderGerberEmptyGerberReturnsZero) {
  // 空 Gerber 对象（无层）→ 空 layer 跳过语义：返回 0，不崩溃
  auto gerber = std::make_shared<Gerber>();
  BLImage img(64, 64, BL_FORMAT_PRGB32);
  Blend2DEngine engine(img, BoundBox(0.0, 1.0, 1.0, 0.0), 0.0);
  EXPECT_EQ(engine.RenderGerber(gerber), 0);
}

TEST(Blend2DRenderTest, RenderGerberWithConvertStrokesToFillsSucceeds) {
  // true 时 RenderGerber 走 parser 侧 StrokesToFillsConverter 路径（engine 公共
  // 不变量 convert_strokes2fills_ 被 GerberLayer::Draw 读取）且不返回非零。
  auto gerber = ParseCircleLineFixture();
  ASSERT_NE(gerber, nullptr);
  const auto bbox = gerber->GetBBox();

  BLImage img(512, 512, BL_FORMAT_PRGB32);
  Blend2DEngine engine(img, bbox, 0.005);
  engine.SetConvertStroke2Fills(true);
  engine.DrawBackground();
  EXPECT_EQ(engine.RenderGerber(gerber), 0);
}

// —— T9: step-and-repeat + context 变换 + inches/mm（M1 检查点） ——
// steprepeat 夹具 bbox 含 copy 扩展（Gerber::GetBBox 走 layer->GetRight/GetTop），
// 视口 506.88px 覆盖 4 个 flash；白像素阈值 40000：单 flash ≈16464px < 4 flash ≈65857px。

TEST(Blend2DRenderTest, RenderGerberCopyLayerRepeatsFlashAcrossStepGrid) {
  auto gerber = ParseTestGerberFile("blend2d_unit_steprepeat.gbr");
  ASSERT_NE(gerber, nullptr);
  auto layers = gerber->GetLayers();
  ASSERT_GE(layers.size(), 1u);
  ASSERT_TRUE(layers[0]->IsCopyLayer());

  const auto bbox = gerber->GetBBox();
  BLImage img(512, 512, BL_FORMAT_PRGB32);
  Blend2DEngine engine(img, bbox, 0.005);
  engine.DrawBackground();
  EXPECT_EQ(engine.RenderGerber(gerber), 0);

  // 4 个 flash 中心 (1,1),(2,1),(1,2),(2,2) 设备坐标全部白
  const double flash_x[2] = {1.0, 2.0};
  const double flash_y[2] = {1.0, 2.0};
  for (double y : flash_y) {
    for (double x : flash_x) {
      auto p = MapToDevicePixels(bbox, 0.005, 512, 512, x, y);
      EXPECT_EQ(PixelAt(img, p.first, p.second), 0xFFFFFFFFu)
          << "flash center (" << x << "," << y << ") should be white";
    }
  }

  // 4 单元间隙 (1.5,1.5)，距最近中心 0.707mm > 0.2mm 半径 → 黑
  auto gap = MapToDevicePixels(bbox, 0.005, 512, 512, 1.5, 1.5);
  EXPECT_EQ(PixelAt(img, gap.first, gap.second), 0xFF000000u);
}

TEST(Blend2DRenderTest, RenderGerberStepRepeatProducesFourFlashAreas) {
  // 白盒等价：count_x×count_y=4 次绘制 → 白像素总量显著大于单次 flash
  auto gerber = ParseTestGerberFile("blend2d_unit_steprepeat.gbr");
  ASSERT_NE(gerber, nullptr);

  const auto bbox = gerber->GetBBox();
  BLImage img(512, 512, BL_FORMAT_PRGB32);
  Blend2DEngine engine(img, bbox, 0.005);
  engine.DrawBackground();
  EXPECT_EQ(engine.RenderGerber(gerber), 0);

  int white_count = 0;
  BLImageData data;
  img.getData(&data);
  const auto *base = static_cast<const uint8_t *>(data.pixelData);
  for (int y = 0; y < 512; ++y) {
    for (int x = 0; x < 512; ++x) {
      const auto *px = base + static_cast<size_t>(y) * data.stride +
                       static_cast<size_t>(x) * 4u;
      if (*reinterpret_cast<const uint32_t *>(px) == 0xFFFFFFFFu) {
        ++white_count;
      }
    }
  }
  EXPECT_GE(white_count, 40000);
}

TEST(Blend2DRenderTest, ContextTransformDrivesBothOffsets) {
  // 同一几何在两种 offset 渲染 → 各自映射的 flash 中心均为白，
  // 证明渲染由 ComputeViewTransform 驱动而非硬编码（变换在构造器已实现，批次 D 交接）。
  auto gerber = ParseTestGerberFile("blend2d_unit_positive.gbr");
  ASSERT_NE(gerber, nullptr);
  const auto bbox = gerber->GetBBox();

  for (double offset : {0.005, 0.02}) {
    BLImage img(512, 512, BL_FORMAT_PRGB32);
    Blend2DEngine engine(img, bbox, offset);
    engine.DrawBackground();
    EXPECT_EQ(engine.RenderGerber(gerber), 0);
    auto center = MapToDevicePixels(bbox, offset, 512, 512, 1.0, 1.0);
    EXPECT_EQ(PixelAt(img, center.first, center.second), 0xFFFFFFFFu)
        << "offset " << offset << " center should be white";
  }
}

TEST(Blend2DRenderTest, RenderGerberInchesUnitEndToEnd) {
  // inches 夹具：D10=Ø0.4in，flash@(1.0000in)=(25.4mm,25.4mm)，
  // bbox Right = 25.4 + 0.2in(5.08mm) = 30.48mm。
  auto gerber = ParseTestGerberFile("blend2d_unit_inches.gbr");
  ASSERT_NE(gerber, nullptr);
  EXPECT_EQ(gerber->Unit(), UnitType::guInches);
  EXPECT_NEAR(gerber->GetBBox().Right(), 30.48, 0.05);

  const auto bbox = gerber->GetBBox();
  BLImage img(512, 512, BL_FORMAT_PRGB32);
  Blend2DEngine engine(img, bbox, 0.005);
  engine.DrawBackground();
  EXPECT_EQ(engine.RenderGerber(gerber), 0);

  auto center = MapToDevicePixels(bbox, 0.005, 512, 512, 25.4, 25.4);
  EXPECT_EQ(PixelAt(img, center.first, center.second), 0xFFFFFFFFu);
}

// —— T12: image_diff.h 容差比对工具契约 ——
// 契约：CompareImages 逐像素取"最大通道差"，统计 mean/max/ratio；
//       ratio 仅统计严格大于 threshold 的像素；PixelAt 读 PRGB32/XRGB32 像素。
// 构造 4×4 全黑图 A 与变体 B（像素通道差均为 0 或单一目标像素异色）。

namespace {

BLImage MakeSolidImage(int width, int height, uint32_t argb) {
  BLImage img(width, height, BL_FORMAT_PRGB32);
  BLImageData data;
  img.getData(&data);
  auto *p = static_cast<uint8_t *>(data.pixelData);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      *reinterpret_cast<uint32_t *>(p + static_cast<size_t>(y) * data.stride +
                                    static_cast<size_t>(x) * 4u) = argb;
    }
  }
  return img;
}

void SetPixel(BLImage &img, int x, int y, uint32_t argb) {
  BLImageData data;
  img.getData(&data);
  auto *p = static_cast<uint8_t *>(data.pixelData);
  *reinterpret_cast<uint32_t *>(p + static_cast<size_t>(y) * data.stride +
                                static_cast<size_t>(x) * 4u) = argb;
}

}  // namespace

TEST(ImageDiffTest, IdenticalImagesGiveZeroStats) {
  BLImage a = MakeSolidImage(4, 4, 0xFF000000);
  BLImage b = MakeSolidImage(4, 4, 0xFF000000);

  auto stats = CompareImages(a, b);
  EXPECT_DOUBLE_EQ(stats.mean_abs_diff, 0.0);
  EXPECT_EQ(stats.max_abs_diff, 0);
  EXPECT_DOUBLE_EQ(stats.diff_ratio, 0.0);
}

TEST(ImageDiffTest, SingleWhitePixelShiftsStats) {
  BLImage a = MakeSolidImage(4, 4, 0xFF000000);
  BLImage b = MakeSolidImage(4, 4, 0xFF000000);
  SetPixel(b, 0, 0, 0xFFFFFFFF);

  auto stats = CompareImages(a, b, 32);
  EXPECT_DOUBLE_EQ(stats.mean_abs_diff, 255.0 / 16.0);  // 15.9375
  EXPECT_EQ(stats.max_abs_diff, 255);
  EXPECT_DOUBLE_EQ(stats.diff_ratio, 1.0 / 16.0);       // 0.0625
}

TEST(ImageDiffTest, ThresholdIsStrictlyGreaterNotInclusive) {
  BLImage a = MakeSolidImage(4, 4, 0xFF000000);

  // 通道差恰为 32 → 严格大于 32 才计入 → ratio 应为 0
  BLImage b32 = MakeSolidImage(4, 4, 0xFF000000);
  SetPixel(b32, 0, 0, 0xFF202020);
  auto stats32 = CompareImages(a, b32, 32);
  EXPECT_DOUBLE_EQ(stats32.diff_ratio, 0.0);

  // 通道差 33 → 严格大于 32 → ratio = 1/16
  BLImage b33 = MakeSolidImage(4, 4, 0xFF000000);
  SetPixel(b33, 0, 0, 0xFF212121);
  auto stats33 = CompareImages(a, b33, 32);
  EXPECT_DOUBLE_EQ(stats33.diff_ratio, 1.0 / 16.0);
}

TEST(ImageDiffTest, PixelAtReadsArgbValue) {
  BLImage black = MakeSolidImage(4, 4, 0xFF000000);
  BLImage white = MakeSolidImage(4, 4, 0xFFFFFFFF);

  EXPECT_EQ(PixelAt(black, 0, 0), 0xFF000000u);
  EXPECT_EQ(PixelAt(white, 0, 0), 0xFFFFFFFFu);
}

TEST(ImageDiffTest, SizeMismatchYieldsMaxFailureStats) {
  // actual 4×4 vs expected 2×2：尺寸不等时 CompareImages 必须自防护，
  // 不得按 actual 尺寸越界读 expected 的像素，而是返回"完全不同"统计量
  // （max=255、diff_ratio=1.0），由调用方阈值判定必然判不匹配。
  BLImage a = MakeSolidImage(4, 4, 0xFF000000);
  BLImage b = MakeSolidImage(2, 2, 0xFF000000);

  auto stats = CompareImages(a, b, 32);
  EXPECT_DOUBLE_EQ(stats.mean_abs_diff, 255.0);
  EXPECT_EQ(stats.max_abs_diff, 255);
  EXPECT_DOUBLE_EQ(stats.diff_ratio, 1.0);
}
