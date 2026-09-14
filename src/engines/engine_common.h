#pragma once
// 引擎共享常量（单一事实源）。
// 逻辑坐标 → 渲染坐标的放大倍数，所有引擎共用，禁止在别处重新定义数值。

namespace gerber_engine {
constexpr int kTimes = 10000;
}  // namespace gerber_engine
