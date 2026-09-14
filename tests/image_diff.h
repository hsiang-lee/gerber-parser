#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>

#include <blend2d.h>

// 容差比对工具（契约定义，T12）。
// Blend2D 金标测试的过渡方案："差不多就行"——逐像素计算最大通道差统计量，
// 由调用方用阈值判定，不追求像素级一致。后续迁移到代码级/单元级测试时，
// 此工具与本测试中的渲染 helper 一起被替换，调用面收敛在 tests/ 内。

struct ImageDiffStats {
  double mean_abs_diff;  // 每像素"最大通道差"的均值（0..255）
  int    max_abs_diff;   // 所有像素最大通道差
  double diff_ratio;     // 最大通道差 > threshold 的像素占比（0..1）
};

// 单像素"最大通道差"（BGRA 四通道取绝对值最大者）
inline int MaxChannelDiff(const uint8_t *pa, const uint8_t *pe) {
  int ch_max = 0;
  for (int c = 0; c < 4; ++c) {
    int diff = std::abs(static_cast<int>(pa[c]) - static_cast<int>(pe[c]));
    if (diff > ch_max) ch_max = diff;
  }
  return ch_max;
}

// 双图逐像素差异的扫描统计中间量（CompareImages 提取的扫描内核返回）
struct DiffScanResult {
  double sum = 0.0;  // 最大通道差累加和
  int max = 0;       // 所有像素最大通道差
  int over_threshold = 0;  // 最大通道差 > threshold 的像素数
};

// 全图扫描内核：逐像素累积最大通道差统计（调用方保证两图尺寸一致）
inline DiffScanResult ScanImageDiff(const BLImageData &data_a,
                                    const BLImageData &data_e,
                                    int threshold) {
  const auto *base_a = static_cast<const uint8_t *>(data_a.pixelData);
  const auto *base_e = static_cast<const uint8_t *>(data_e.pixelData);

  DiffScanResult result;
  for (int y = 0; y < data_a.size.h; ++y) {
    const auto *row_a = base_a + static_cast<int64_t>(y) * data_a.stride;
    const auto *row_e = base_e + static_cast<int64_t>(y) * data_e.stride;
    for (int x = 0; x < data_a.size.w; ++x) {
      int ch_max = MaxChannelDiff(
          row_a + static_cast<int64_t>(x) * 4,
          row_e + static_cast<int64_t>(x) * 4);
      result.sum += ch_max;
      if (ch_max > result.max) result.max = ch_max;
      if (ch_max > threshold) ++result.over_threshold;
    }
  }
  return result;
}

// actual vs expected 按同尺寸逐像素比较；尺寸不等时自防护：提前返回
// "完全不同"统计量（mean=255、max=255、ratio=1.0），避免按较大尺寸
// 越界读另一图像（契约自包含，调用方无需先断言尺寸）。
inline ImageDiffStats CompareImages(const BLImage &actual,
                                    const BLImage &expected,
                                    int threshold = 32) {
  BLImageData data_a, data_e;
  actual.getData(&data_a);
  expected.getData(&data_e);

  if (data_a.size.w != data_e.size.w || data_a.size.h != data_e.size.h) {
    ImageDiffStats mismatch;
    mismatch.mean_abs_diff = 255.0;
    mismatch.max_abs_diff = 255;
    mismatch.diff_ratio = 1.0;
    return mismatch;
  }

  DiffScanResult scan = ScanImageDiff(data_a, data_e, threshold);
  const int64_t total = static_cast<int64_t>(data_a.size.w) *
                        static_cast<int64_t>(data_a.size.h);

  ImageDiffStats stats;
  stats.mean_abs_diff = total > 0 ? scan.sum / static_cast<double>(total) : 0.0;
  stats.max_abs_diff = scan.max;
  stats.diff_ratio =
      total > 0 ? static_cast<double>(scan.over_threshold) /
                      static_cast<double>(total)
                : 0.0;
  return stats;
}

// 读取 PRGB32/XRGB32 图像 (x,y) 像素（不透明像素两种格式数值一致）
inline uint32_t PixelAt(const BLImage &image, int x, int y) {
  BLImageData data;
  image.getData(&data);
  const auto *p = static_cast<const uint8_t *>(data.pixelData) +
                  static_cast<int64_t>(y) * data.stride +
                  static_cast<int64_t>(x) * 4;
  return *reinterpret_cast<const uint32_t *>(p);
}
