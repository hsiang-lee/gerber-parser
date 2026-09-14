# Blend2D 渲染层迁移 设计规范

> **状态:** 已批准（2026-09-14 用户批准，4 项 Open Question 决策闭合）

**目标:** 彻底移除项目的 Qt 依赖与 Python 支持（pybind11 绑定层），将渲染层改用 Blend2D（经 vcpkg 引入），解析器核心零改动。

**架构:** 新增 `Blend2DEngine : public Engine`，用 BLPath + BLContext 映射核心解析器发出的绘制指令；把原 `qpainter_engine.cpp` 中不在 Engine 接口内的 Gerber 渲染语义（逐 layer 遍历、step-and-repeat、negative/clear 前景背景互换、定点缩放与单位处理）移植进新引擎。所有第三方依赖统一到 vcpkg manifest，删除 Qt 引擎、Qt 查看器示例、Qt 测试依赖、整个 Python 绑定层及相关文档/CI 配置。

**技术栈:** C++17、CMake ≥ 3.20、Blend2D（vcpkg 包名 `blend2d`，许可证 Zlib，`find_package(Blend2D CONFIG REQUIRED)` → `Blend2D::Blend2D`）、googletest（vcpkg 包名 `gtest`）、gflags（vcpkg 包名 `gflags`，仅 `gerber2image` 使用）。不含 pybind11——Python 支持彻底移除。

---

## Background（现状与调研证据）

### 现状

- 核心解析器位于 `src/parser/src/gerber_parser/**`（约 6.3k 行），**零 Qt 依赖**，通过纯虚 `Engine` 接口（`src/parser/src/engine/engine.h`）发出绘制指令。
- Qt 相关代码分布：
  - `src/engines/`：`qpainter_engine.{h,cpp}`、`qgraphics_scene_engine.{h,cpp}`、`transformation.{h,cpp}`（合计约 886 行）
  - `example/`：3 个 Qt 示例 `gerber_viewer`、`gerber_viewer_qgraphics`、`gerber2image`（合计约 504 行）
  - `src/pygerber-parser/`：pybind11 绑定，运行时创建 `QApplication`（约 186 行）
  - `tests/gerber_renderer_test.cpp`：Qt 依赖的像素金标测试（约 469 行），渲染到 `QImage` 并与 `.bmp` 基线逐像素比对
- 构建现状：**未使用 vcpkg**。gflags / googletest / pybind11 走 git 子模块（`.gitmodules` + `add_subdirectory(3rdparty/...)`）；Qt6 走 `find_package(Qt6 ...)`（`src/CMakeLists.txt`、`tests/CMakeLists.txt`、`src/pygerber-parser/CMakeLists.txt` 各一处）。
- CI（`.github/workflows/workflow.yml`）通过 `jurplel/install-qt-action@v3` 安装 Qt。

### 关键调研结论

1. **渲染语义不在接口里，在实现里。** `Engine` 接口只声明了绘制原语（MoveTo/LineTo/CubicTo/AddRect/AddCircle/DrawAperture + BeginDrawOutline/EndDrawOutline/BeginDrawStroke/EndDrawStroke/CachePoint/CachedPoint/CurrentPos）。"逐 layer 遍历 + step-and-repeat 拷贝 + negative/clear 层前景背景互换"这些 Gerber 语义位于 `QPainterEngine::RenderGerber` 及其配套的画笔/画刷准备函数中，移植时必须一并搬入新引擎，否则渲染结果不等价。
2. **定点缩放约定。** 现引擎以 `kTimes = 10000` 将逻辑坐标缩放到设备坐标（所有绘制原语入参乘以 `kTimes`，`CurrentPos` 反向除回），并叠加 BoundBox 偏移；inches/mm 单位换算亦在此约定内。此约定与 Qt 无关，可原样移植。
3. **坐标变换依赖 Qt 类型。** `transformation.{h,cpp}` 基于 `QRect`/`QPoint` 实现"逻辑窗口 ↔ 物理视口"映射，无法直接复用；`QPainterEngine` 的交互态方法（Rotate/Scale/Move/Dev2Logic 等）同样是 Qt 类型绑定的。`transformation.h` 中"取 painter window / 取 viewport"这一映射概念本身是 Qt 无关的——其算法内核（按 BoundBox 宽高比 + 物理尺寸 + offset 计算平移缩放因子）完全可用 `BLMatrix2D` 或自写 2x3 矩阵表达。
4. **gflags 仅被一个示例使用。** 全仓库对 `gflags` 的引用只出现在 `example/gerber2image/`。已确认（2026-09-14）：该示例保留改造，gflags 随之保留并走 vcpkg。
5. **pybind11 绑定与 QApplication 强绑定。** 绑定层用全局 `QApplication` 初始化 GUI，图像经 `QImage` 中转后导出像素。（已被 2026-09-14 决策取代：Python 支持彻底移除，绑定层整体删除而非迁移，详见"Python 支持移除"章节。）
6. **旧像素基线由 QPainter 产出。** `tests/test_data/results/*.bmp` 是 QPainter 渲染结果；Blend2D 的抗锯齿实现与之不同，逐像素完全一致不现实（见测试策略）。
7. **README 多处提及 Qt 与 Python 绑定。** `README.md`（Qt 徽章、"Qt-based" 描述、依赖清单 "Qt 6.0+"、致谢，以及 Python 绑定功能条目、目录树条目、Python 用法示例章节、pybind11 致谢）、`README_zh.md`、`PYTHON_USAGE.md`（随 Python 支持移除整体删除）均在清理范围内。

---

## Goals & Non-Goals

### Goals

- 彻底移除 Qt 依赖：代码、CMake、CI、文档中不再出现任何 Qt 类型 / `find_package(Qt6)` / install-qt-action。
- **彻底移除 Python / pybind11 绑定层**（2026-09-14 决策，范围超出"去 Qt"）：`src/pygerber-parser/`、`test_python_binding.py`、`PYTHON_USAGE.md`、pybind11 子模块及其全部构建/文档引用一并删除。
- 新增 `Blend2DEngine`，实现 `Engine` 接口全部方法，并移植 `QPainterEngine` 中的 Gerber 渲染语义。
- 依赖管理 vcpkg 化：新建 `vcpkg.json` manifest（最终依赖 = `blend2d` + `gtest` + `gflags`），第三方依赖走 `find_package`，禁止手动路径。
- `gerber2image` 改造为 Blend2D 版保留，作为 headless PNG 导出验证工具。
- 渲染验证以单元测试为先（Engine 指令映射的 BLPath 结构断言、语义逻辑独立单测）；像素金标对比（基线重生成 + 容差比对）仅作过渡验收手段。
- README / CMake 注释 / CI 中的 Qt 与 Python 绑定内容清理干净。

### Non-Goals

- **不改动解析器核心。** `src/parser/**`（vendored 仓库）内部零改动，`Engine` 接口不改。
- **不动 `src/parser/{tests,example,3rdparty}`**——它们不参与顶层构建；`src/parser/` 内部的 pybind11 相关引用亦**不在**"Python 支持移除"范围内。
- **不做 Web 可视化。** 本次只换渲染库；web 是后续可能性，明确不在本次范围。
- **不提供替代语言绑定。** Python 支持为彻底移除，非替换。
- 不追求与旧 QPainter 基线逐像素完全一致。

---

## 架构

### 组件概览

```
gerber_parser 核心（零改动, ~6.3k 行, 无 Qt）
      │  通过纯虚 Engine 接口发出绘制指令
      ▼
Blend2DEngine : public Engine          ← 新增（src/engines/blend2d_engine.{h,cpp}）
  ├─ BLPath   累积绘制指令（MoveTo/LineTo/CubicTo/AddRect/AddCircle/DrawAperture）
  ├─ BLContext 落盘（fillPath / strokePath, negative/clear 前景背景互换）
  ├─ 移植自 QPainterEngine::RenderGerber 的语义：
  │    逐 layer 遍历 · step-and-repeat 拷贝 · negative/clear 互换
  ├─ 移植 transformation.h 的"逻辑↔物理"映射（BLMatrix2D / 自写 2x3，Qt 类型全部替换）
  └─ kTimes=10000 定点缩放 + inches/mm 单位处理（照搬）
      │
      ▼
  渲染目标：BLImage →（PNG 导出 / 像素缓冲）
      ├─ tests/gerber_renderer_test.cpp（改用 Blend2D 渲染 + 容差比对，过渡验收手段）
      └─ example/gerber2image（改造为 Blend2D 版，headless PNG 导出验证工具）

（原 pygerber-parser 绑定分支已按 2026-09-14 决策整体删除，见"Python 支持移除"章节）
```

### Blend2DEngine（新增）

- **职责:** 将 `Engine` 接口的绘制指令流映射为 Blend2D 的 path 累积 + context 绘制；承载从 `QPainterEngine` 移植来的 Gerber 渲染语义（接口外的 layer/step-repeat/negative 逻辑）。
- **模块边界:** 对外暴露与 `QPainterEngine` 等价的渲染入口（接收 Gerber 对象 + BoundBox + 目标 BLImage），对内向 `Engine` 接口提供指令实现；对 `src/parser/**` 无任何反向修改。
- **依赖:** Blend2D（`Blend2D::Blend2D`）、`Engine` 接口、解析器类型（`Aperture`/`BoundBox`/`Gerber` 等，前向声明级）。gflags 仅被 `gerber2image` 工具使用，不是引擎依赖。
- **错误处理:** 关键场景——空 layer 跳过；`BLImage` 分配失败 / `BLContext` 构造失败返回错误码，调用方决定中止（与现 `RenderGerber` 返回 `int` 状态码的约定对齐）。

### Engine 接口 → Blend2D 指令映射表

| Engine 方法 | Blend2D 映射 |
|---|---|
| `MoveTo` | `BLPath::moveTo` |
| `LineTo` | `BLPath::lineTo` |
| `CubicTo` | `BLPath::cubicTo` |
| `AddRect` | 构造矩形 path 后 `BLContext::fillPath` / `strokePath` |
| `AddCircle` | 构造圆形 path 后 `BLContext::fillPath` / `strokePath` |
| `DrawAperture` | 按孔径类型构造 path → `fillPath` |
| `BeginDrawStroke` / `EndDrawStroke` | stroke 模式 begin/end；`EndDrawStroke` 中 `setStrokeStyle`（含线宽、round cap）后 `strokePath` |
| `BeginDrawOutline` / `EndDrawOutline` | outline 模式 begin/end；`EndDrawOutline` 中 `fillPath` |
| `CachePoint` / `CachedPoint` | 维护 `std::pair<double,double>` 缓存点（与现实现等价，Qt 无关） |
| `CurrentPos` | 返回当前坐标（÷ `kTimes` 还原逻辑坐标） |

### 语义移植清单（从 QPainterEngine → Blend2DEngine）

以下逻辑在 `Engine` 接口之外，必须在 `Blend2DEngine` 中落地：

| 现逻辑 | 移植要点 |
|---|---|
| `RenderGerber` 主循环 | 逐 layer 遍历，每 layer 建 path 后 fill |
| step-and-repeat | 按 `count_x_/count_y_/step_x_/step_y_`（× `kTimes`）对每个绘制单元做平移拷贝（对应现 `translate` 嵌套循环） |
| negative / clear 层 | 前景/背景互换：normal 层 `fill(foreground)`，negative/clear 层 `fill(background)`；`negative_` 状态下颜色反转 |
| `kTimes = 10000` 定点缩放 | 所有入参 × `kTimes` 落设备坐标，`CurrentPos` ÷ `kTimes` 还原（照搬，非 Qt 相关） |
| inches/mm 单位处理 | 现 `transformation` + parser 的单位→坐标换算约定照搬 |
| 逻辑窗口 ↔ 物理视口映射 | `Transformation::GetPainterWindow/GetPainterViewport` 的换算算法用 `BLMatrix2D` 或自写 2x3 矩阵重写 |
| 抗锯齿 + round cap 描边 | `BLStrokeStyle` 设 `BL_STROKE_CAP_ROUND` + `BL_STROKE_JOIN_ROUND` + AA |
| 背景填充 / 前景色 | `BLContext::fill` 矩形铺底；前景/背景色用 `BLRgba32` |

> **硬约束：移植过程中禁止引入任何 Qt 类型**——`QPainterPath`→`BLPath`，`QPainter`→`BLContext`，`QImage`/`QPixmap`→`BLImage`，`QColor`→`BLRgba32`，`QRect`/`QPoint`→`BLBox`/`BLPoint` 或自写 2x3 矩阵。`transformation.{h,cpp}` 的算法在 `Blend2DEngine` 内用上述类型重写，不作为独立可复用模块保留。

---

## 构建系统变更

### vcpkg manifest（新建 `vcpkg.json`）

```json
{
  "name": "gerber-parser",
  "version": "1.0",
  "dependencies": [
    "blend2d",
    "gtest",
    "gflags"
  ]
}
```

> 依赖清单已定稿（2026-09-14）：最终集合 = `blend2d` + `gtest` + `gflags`。`pybind11` 随 Python 支持移除而不在清单内；`gflags` 因保留 `gerber2image` 而走 vcpkg。`blend2d` 许可证 Zlib，符合商用约束。

### CMake 变更点

- **顶层 `CMakeLists.txt`：**
  - 移除 `add_subdirectory(3rdparty/pybind11)`（随 Python 支持删除）、`add_subdirectory(3rdparty/gflags)`、`add_subdirectory(3rdparty/googletest)`（后两者转 vcpkg）及其他 pybind11 相关内容；
  - 移除 clang-tidy target 中的 `find_package(Qt6 ...)` 与 `${Qt6Widgets_INCLUDE_DIRS}`（tidy 头文件路径改走 vcpkg toolchain）；
  - 引入 vcpkg toolchain（`CMAKE_TOOLCHAIN_FILE` 或 preset）。
- **`src/CMakeLists.txt`：** 移除 `add_subdirectory(pygerber-parser)`（绑定层整体删除）；`find_package(Blend2D CONFIG REQUIRED)`；`gerber_engine` 库源文件换成 `blend2d_engine.cpp`（`file(GLOB engines/*.cpp)` 保留，内容自然收敛）；`target_link_libraries` 把 `Qt6::Core/Widgets/Gui` 换成 `Blend2D::Blend2D`；删除 `set(CMAKE_AUTOMOC ON)`。
- **`tests/CMakeLists.txt`：** `find_package(Qt6 ...)` → `find_package(Blend2D CONFIG REQUIRED)` + `find_package(GTest CONFIG REQUIRED)`；测试目标链接 `Blend2D::Blend2D` + `GTest::gtest`，移除 Qt 链接与 AUTOMOC。
- **`src/pygerber-parser/`：** 整个目录删除（含其 `CMakeLists.txt`、绑定源文件与示例脚本及其 Python/pybind11/Qt6 引用）。
- **`example/gerber2image/CMakeLists.txt`：** 改造为 Blend2D 版：移除 Qt 依赖，改链 `Blend2D::Blend2D` + gflags（vcpkg）；其余示例 CMakeLists 随示例一并删除。

### 子模块清理

`.gitmodules` 现仅含 3 个子模块，本次迁移后**全部移除，不再保留任何子模块依赖**：

- `3rdparty/googletest`：移除（转 vcpkg `gtest`）。
- `3rdparty/gflags`：移除（转 vcpkg `gflags`）。
- `3rdparty/pybind11`：移除（Python 支持彻底删除，无替代依赖）。

三条目移除后 `.gitmodules` 随之清空，可整体删除。

---

## 删除清单（文件级）

**Qt 引擎（`src/engines/`）—— 全部删除，由 `blend2d_engine.{h,cpp}` 替代：**
- `qpainter_engine.{h,cpp}`
- `qgraphics_scene_engine.{h,cpp}`
- `transformation.{h,cpp}`（其算法内核并入 `Blend2DEngine`，不保留独立模块）

**示例：**
- `example/gerber_viewer/`（整个目录：Qt Widgets 查看器）—— 删除
- `example/gerber_viewer_qgraphics/`（整个目录：QGraphics 查看器）—— 删除
- `example/gerber2image/` —— **保留，改造为 Blend2D 版**（headless PNG 导出，兼作验证工具；2026-09-14 决策，方案一）

**Python 支持层（整体删除）：**
- `src/pygerber-parser/`（整个目录：pybind11 绑定源文件 + 示例脚本 + CMakeLists）
- `test_python_binding.py`（仓库根目录）
- `PYTHON_USAGE.md`（仓库根目录，整文件删除）
- `3rdparty/pybind11` 子模块及构建引用（见"构建系统变更"与"Python 支持移除"）

**测试（不删文件，改造）：**
- `tests/gerber_renderer_test.cpp`：去 `QImage`，改 Blend2D 渲染 + 容差比对（过渡验收手段，见测试策略）。

---

## Python 支持移除（替代原"pybind11 绑定"章节）

2026-09-14 用户决策：**彻底移除 Python 支持**——放弃原"pybind11 绑定改用 Blend2D 渲染"方案，范围从"去 Qt"扩大为"去 Qt + 去 Python 绑定层"。

**理由：** Python 绑定层无用户反馈，纯维护成本；若保留需为本次渲染层迁移额外适配（去 `QApplication`、像素导出改道等），投入与收益不成比例。

**删除范围：**
- `src/pygerber-parser/`（pybind11 绑定源文件 + 示例脚本 + 其 CMakeLists，约 186 行）
- `test_python_binding.py`（仓库根目录）
- `PYTHON_USAGE.md`（整文件）
- `3rdparty/pybind11` 子模块（`.gitmodules` 条目 + 目录）
- 顶层 `CMakeLists.txt` 的 `add_subdirectory(3rdparty/pybind11)` 及 pybind11 相关内容；`src/CMakeLists.txt` 的 `add_subdirectory(pygerber-parser)`
- CI 中 Python 绑定相关步骤（实测：现有 workflow 无 Python/pybind11 步骤，无需删除操作）
- README / README_zh 中 Python 绑定功能描述（功能条目、目录树、用法示例章节、pybind11 致谢）

**边界：** `src/parser/` 内部的 pybind11 相关引用**不动**——该目录为 vendored 仓库，不参与顶层构建，不在删除范围内。

**后果（决策原文记录）：** 移除后项目不再提供 Python API；不提供替代语言绑定。

---

## 测试策略

2026-09-14 决策：**单元测试优先，像素金标降级为过渡验收手段**——尽可能把渲染验证转化为更小的单元测试，最终目标是小单测覆盖后退出像素对比。

- **单元测试（首选验证手段）。**
  - Engine 指令映射：构造最小绘制序列后断言 `BLPath` 结构（顶点数、segment 类型、坐标值），逐条验证"Engine 接口 → Blend2D 指令映射表"；
  - 语义逻辑：逐 layer 遍历、step-and-repeat 平移拷贝、negative/clear 前景背景互换、`kTimes` 定点缩放、inches/mm 单位处理等逐项独立单测（纯逻辑输入→输出断言，不落像素）。
- **像素金标（过渡验收手段）。** 迁移期保留 Blend2D 渲染 vs 基线比对，作为端到端等价性的兜底：
  - **基线重生成：** 原 `.bmp` 基线是 QPainter 产出，逐像素一致不现实；以 Blend2D 渲染结果重新生成基线，并人工审核基线正确性（对照 Gerber 原始语义，非仅"跑通即采信"）；
  - **容差比对：** 在灰度/降采样维度上设阈值（平均误差 / 最大误差 / 差异像素占比等度量），先跑通、观察实际偏差分布后再调定；
  - **退出条件：** 单元测试确认覆盖指令映射与语义移植清单后，像素对比退出（可降级为手动运行的可选检查）。
- **纯解析测试保持不动。** `src/parser/tests/**` 零 Qt，本次不触碰。
- **TDD 约束。** 遵循全局 TDD 工作模式：为 `Blend2DEngine` 每条指令映射与每条移植语义先写测试（RED），再实现（GREEN），后重构；金标测试只作集成层过渡验收，非最终形态。
- **粒度授权。** 以上为策略层决策；具体测试用例集、断言粒度、容差度量与阈值、像素对比退出的落地时机，由 plan-writer 在计划阶段依此策略细化（用户已授权 plan-writer/executor 按实际情况把握）。

---

## 文档与 CI 清理

- **`README.md`：** 移除 Qt 徽章；将 "Qt-based" 描述改为 Blend2D；依赖清单 "Qt 6.0+" → Blend2D（+ vcpkg 说明）；致谢去 Qt；**删除 Python 绑定内容**（功能条目、目录树 pygerber-parser 项、Python 用法示例章节、pybind11 致谢）。
- **`README_zh.md`：** 同步中文版本对应段落（含删除 Python 绑定内容）。
- **`PYTHON_USAGE.md`：** **整文件删除**（随 Python 支持移除；原"清理其中 Qt 段落"方案作废）。
- **CMake 注释：** 清理任何残留 Qt 说明。
- **CI `.github/workflows/workflow.yml`：** 移除 `jurplel/install-qt-action@v3` 步骤；新增 vcpkg 集成步骤（导出 `CMAKE_TOOLCHAIN_FILE`，manifest 模式自动装依赖）；构建参数保持 `-DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON`（gerber2image 确定保留为 Blend2D 版）。
- **验收：** 全仓库 `grep -rin '\bqt\b'` 除迁移文档外应无命中；CMake configure 不再请求 `find_package(Qt6)`；`pybind11` / `pygerber` 引用除迁移文档与 vendored `src/parser/` 外亦无命中。

---

## Open Questions（全部已解决 · 2026-09-14 用户确认）

1. **example 处置 —— RESOLVED：方案一。** 删除 `gerber_viewer`、`gerber_viewer_qgraphics`；**保留 `gerber2image` 并改造为 Blend2D 版**（headless 导出 PNG，兼作验证工具）。
2. **子模块 → vcpkg 的迁移范围 —— RESOLVED：范围扩大，彻底移除 Python 支持。** `pybind11` 不做 vcpkg 迁移，绑定层整体删除（明细见"Python 支持移除"章节）；`googletest` 走 vcpkg `gtest`。
3. **像素比对容差度量与阈值 —— RESOLVED：测试策略调整。** 单元测试优先（Engine 指令映射 BLPath 结构断言、语义逻辑独立单测），像素金标降级为过渡验收手段，小单测覆盖后退出；具体度量、阈值与粒度由 plan-writer 在计划阶段细化（用户授权 plan-writer/executor 按实际情况把握）。
4. **gflags 是否仍需要 —— RESOLVED：保留，走 vcpkg。** `gerber2image` 保留并继续使用 gflags；`3rdparty/gflags` 子模块移除，改由 vcpkg `gflags` 提供。

---

## 里程碑批次划分建议

> 建议性拆分，供实现计划参考；顺序遵循"先立依赖基座、再换渲染实现、最后删旧清理"，每批次可独立编译通过。

1. **M0 — 构建基座 vcpkg 化（前置）：** 建 `vcpkg.json`（`blend2d`、`gtest`、`gflags`），接入 toolchain，CI 增加 vcpkg 步骤；`find_package(Blend2D)` 就位。此阶段 Qt 引擎与 Blend2D 共存，现有测试保持全绿。
2. **M1 — Blend2DEngine 落地（核心）：** 新增 `blend2d_engine.{h,cpp}`，实现 `Engine` 全接口映射 + 语义移植清单（TDD，先写失败测试；含 BLPath 结构断言与语义逻辑单测）。Qt 旧引擎暂共存，保证可回退。
3. **M2 — 移除 Python 支持层：** 删除 `src/pygerber-parser/`、`test_python_binding.py`、`PYTHON_USAGE.md`、pybind11 子模块及构建/文档引用。纯删除动作，不依赖 M1，可与其他批次并行。
4. **M3 — 测试切换：** `gerber_renderer_test` 改 Blend2D 渲染，重生成基线，按容差跑通（过渡验收）；按测试策略补齐指令映射/语义逻辑单测。
5. **M4 — 删除与清理收尾：** 按删除清单删 Qt 引擎与两个查看器示例，将 `gerber2image` 改造为 Blend2D 版；完成 googletest/gflags/pybind11 子模块清理收尾，清理 README / CMake 注释 / CI workflow；grep 验收零 Qt、零 Python 绑定残留（vendored `src/parser/` 除外）。

（依赖关系：M1 依赖 M0 提供 `Blend2D::Blend2D`；M3 依赖 M1；M4 的 `gerber2image` 改造依赖 M1；M2 独立于其余批次。所有删除动作须按保证中间态可编译的顺序落位，避免中间态破坏构建。）

---

## 设计决策来源

本文档记录的设计决策来自 **2026-09-14 brainstorm 对话**，用户确认要点：采用 Blend2D、经 vcpkg 引入、**彻底删除 Qt 且不再支持 Qt**、文档清理干净、Web 可视化不在本次范围。

**2026-09-14 第二轮用户确认（4 项 Open Question 全部闭合，文档批准）：**
1. 示例处置采用**方案一**——删两个查看器，`gerber2image` 保留并改造为 Blend2D 版 headless PNG 导出；
2. **彻底移除 Python 支持**——取代原 OQ2"pybind11 走 vcpkg 还是保留子模块"的选项，范围扩大；理由为无用户反馈、纯维护成本；`src/parser/` 内部 pybind11 引用不动；
3. **单元测试优先、像素金标降级为过渡验收手段**——取代原"像素金标为主、容差先跑通再调"方案；具体测试粒度授权 plan-writer/executor 在计划阶段把握；
4. **gflags 保留，走 vcpkg**。

spec-writer 仅整理决策为规范文档，未新增或更改设计决策；原 Open Questions 已全部闭合，无遗留待确认项。
