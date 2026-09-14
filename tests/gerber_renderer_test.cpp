// Blend2D 渲染金标测试（M3 重写版，T13）。
//
// 与旧版（qpainter 引擎 + 图像像素相等，已随引擎迁移移除）的差异：
//   - 渲染引擎换成 Blend2DEngine（1600×1600 PRGB32）。
//   - 比对方式换成容差比对（tests/image_diff.h，T12 契约）：
//     mean/max/ratio 三项统计量，逐像素"最大通道差"，非像素级严格相等。
//     "差不多就行"——像素级一致性不追求，本项目后续以代码级/单元级测试替代。
//   - 基线由 .bmp 换成 .png（tests/test_data/gerber/results/*.png）。
//   - 删除 TestScale / TestMove（它们测试已删除的 Transformation 交互语义）。
//
// 阈值定稿记录（2026-09-14，实测偏差分布调定，不过度收紧）：
//   - 基线为重写后由本测试以 REGEN=1 一次性生成（同引擎同版本）；
//   - 正常模式下对 19 个文件首跑实测：全部 mean_abs_diff==0、max_abs_diff==0、
//     diff_ratio==0（确定性渲染，同引擎同输入逐像素一致）；
//   - 因此初始阈值（mean≤8.0 / max≤64 / ratio≤0.12）即为最终阈值：
//     留足余量以捕获未来引擎行为漂移（回归侦测），未收紧到 0。
//   - 若后续引擎行为有意变更，用 GERBER_TEST_REGENERATE=1 重生成基线并在人工
//     审核后调阈值，不允许直接跳到"跑通即采信"。

#include "engines/blend2d_engine.h"
#include "image_diff.h"
#include "test_helpers.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "gerber/gerber.h"
#include <gtest/gtest.h>

namespace {

// —— 阈值（T13 定稿，见文件头注释） ——
constexpr double kMaxMeanAbsDiff = 8.0;
constexpr int    kMaxMaxAbsDiff  = 64;
constexpr double kMaxDiffRatio   = 0.12;

bool RegenerateBaselines() {
  const char *v = std::getenv("GERBER_TEST_REGENERATE");
  return v != nullptr && std::string(v) == "1";
}

BLImage RenderGoldenImage(const std::shared_ptr<Gerber> &gerber,
                          bool convert_strokes2fills) {
  BLImage image(1600, 1600, BL_FORMAT_PRGB32);
  Blend2DEngine engine(image, gerber->GetBBox(), 0.005);
  engine.SetConvertStroke2Fills(convert_strokes2fills);
  engine.DrawBackground();
  engine.RenderGerber(gerber);
  return image;
}

// 比对（REGEN=1 时写基线并跳过比对；否则读基线并断言三项统计量）。
void AssertMatchesBaseline(const std::string &base_name,
                           const BLImage &rendered) {
  const std::string path =
      std::string(TestData) + "results/" + base_name + ".png";

  if (RegenerateBaselines()) {
    ASSERT_EQ(rendered.writeToFile(path.c_str()), BL_SUCCESS)
        << "write baseline failed: " << path;
    std::cerr << "[regen] " << base_name << ".png written\n";
    return;
  }

  BLImage baseline;
  ASSERT_EQ(baseline.readFromFile(path.c_str()), BL_SUCCESS)
      << "baseline missing, rerun with GERBER_TEST_REGENERATE=1: " << path;
  ASSERT_EQ(rendered.size(), baseline.size());

  ImageDiffStats stats = CompareImages(rendered, baseline);
  std::cerr << "[diff] " << base_name << ": mean=" << stats.mean_abs_diff
            << " max=" << stats.max_abs_diff << " ratio=" << stats.diff_ratio
            << "\n";

  EXPECT_LE(stats.mean_abs_diff, kMaxMeanAbsDiff) << base_name;
  EXPECT_LE(stats.max_abs_diff, kMaxMaxAbsDiff) << base_name;
  EXPECT_LE(stats.diff_ratio, kMaxDiffRatio) << base_name;
}

// 保留旧用例的 bbox 元断言：IsNegative / Name。
void ExpectDefaultLayerMeta(const std::shared_ptr<Gerber> &gerber) {
  ASSERT_NE(gerber, nullptr);
  EXPECT_FALSE(gerber->IsNegative());
  EXPECT_EQ(gerber->Name(), "");
}

}  // namespace

TEST(GerberRendererTest, TestRenderFromGerber) {
  auto gerber = ParseTestGerberFile("2301113563-f-gtl");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301113563-f-gtl",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestConvertStroke2Fill) {
  auto gerber = ParseTestGerberFile("2301113563-f-gtl");
  ASSERT_NE(gerber, nullptr);

  AssertMatchesBaseline(
      "2301113563-f-gtl_stroke2fill",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/true));
}

TEST(GerberRendererTest, TestRenderGerberFile1) {
  auto gerber = ParseTestGerberFile("2301113987c.dat");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301113987c.dat",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile2) {
  auto gerber = ParseTestGerberFile("2301113987c.rout");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301113987c.rout",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile3) {
  auto gerber = ParseTestGerberFile("2301113987-c-gbl");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301113987-c-gbl",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile4) {
  auto gerber = ParseTestGerberFile("2301113987-c-gbs");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301113987-c-gbs",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile5) {
  auto gerber = ParseTestGerberFile("2301113987-c-gtl");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301113987-c-gtl",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile6) {
  auto gerber = ParseTestGerberFile("2301113987-c-gts");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301113987-c-gts",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile7) {
  auto gerber = ParseTestGerberFile("2301115633.rout");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301115633.rout",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile8) {
  auto gerber = ParseTestGerberFile("2301115633lg.dat");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301115633lg.dat",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

// 原用例带 Scale(5.0)；交互视角（Transformation）已删除，统一按默认视图渲染，
// 基线随之重生成。
TEST(GerberRendererTest, TestRenderGerberFile9) {
  auto gerber = ParseTestGerberFile("2301115633lg.ld12");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301115633lg.ld12",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile10) {
  auto gerber = ParseTestGerberFile("2301115633lg.ld21");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301115633lg.ld21",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile11) {
  auto gerber = ParseTestGerberFile("2301115633-lg-gbl");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301115633-lg-gbl",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile12) {
  auto gerber = ParseTestGerberFile("2301115633-lg-gbs");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301115633-lg-gbs",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile13) {
  auto gerber = ParseTestGerberFile("2301115633-lg-gtl");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301115633-lg-gtl",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile14) {
  auto gerber = ParseTestGerberFile("2301115633-lg-gts");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "2301115633-lg-gts",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile15) {
  auto gerber = ParseTestGerberFile("hj.324v1.gts");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "hj.324v1.gts",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile16) {
  auto gerber = ParseTestGerberFile("BOTTOM.art");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "BOTTOM.art",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}

TEST(GerberRendererTest, TestRenderGerberFile17) {
  auto gerber = ParseTestGerberFile("P20230731.gtl");
  ExpectDefaultLayerMeta(gerber);

  AssertMatchesBaseline(
      "P20230731.gtl",
      RenderGoldenImage(gerber, /*convert_strokes2fills=*/false));
}
