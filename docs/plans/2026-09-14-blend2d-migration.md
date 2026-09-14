# Blend2D 渲染层迁移 — 实现计划

> **需求来源:** `docs/specs/2026-09-14-blend2d-migration-design.md`（已批准，唯一需求来源）
> **计划状态:** 已按 Plan-Writer 规范产出；前置 skill 已加载（tdd / refactoring）。
> **执行纪律:** 每个代码任务按 RED-GREEN-REFACTOR 执行；GREEN 后按 refactoring skill 输出 REFACTOR_LEDGER；批次内逐任务完整红绿循环，全部 GREEN 后一次 commit。

---

## Phase 0: 引用验证结论

设计文档中全部文件路径均已核实存在（见下表），无冲突的现有实现：

| 引用 | 核实结果 |
|---|---|
| `src/parser/src/engine/engine.h` | ✅ 纯虚 `Engine`（MoveTo/LineTo/CubicTo/AddRect/AddCircle/DrawAperture/BeginDrawOutline/EndDrawOutline/BeginDrawStroke/EndDrawStroke/CachePoint/CachedPoint/CurrentPos + `bool convert_strokes2fills_`） |
| `src/engines/qpainter_engine.{h,cpp}` | ✅ 301 行；`kTimes=10000`（h:75）；负片互换（cpp:139-143）；step-repeat y 外/x 内循环（cpp:113-120）；stroke 线宽 `BBox().Width()*kTimes` + RoundCap/RoundJoin（cpp:146-159）；DrawAperture pixmap 缓存（cpp:161-179） |
| `src/engines/qgraphics_scene_engine.{h,cpp}` | ✅ 删除对象 |
| `src/engines/transformation.{h,cpp}` | ✅ 算法内核=LogicLeft/Top/Width/Height + 视口（cpp:14-91）；`GetPainterWindow()` 返回 `QRect(left,top,width,-height)` |
| `tests/gerber_renderer_test.cpp` | ✅ 469 行；21 个用例；TestData 宏在 `tests/CMakeLists.txt:20` 指向 `tests/test_data/gerber/`；基线 `results/*.bmp` 21 个（含 `_scale1_2.bmp`、`_move.bmp`、`_stroke2fill.bmp`） |
| `tests/CMakeLists.txt` | ✅ Qt find_package + AUTOMOC + 裸 `gtest/gtest_main`；`file(GLOB_RECURSE *.cpp)` → 新测试文件自动入编 |
| `src/CMakeLists.txt` | ✅ Qt6 find_package/AUTOMOC/Qt 链接（:1-2,15）；`add_subdirectory(pygerber-parser)`（:19）；`file(GLOB engines/*.cpp)`（:7） |
| 顶层 `CMakeLists.txt` | ✅ `add_subdirectory(3rdparty/pybind11)`（:11）；gflags(15)/googletest(23)；example×3(17-19)；clang-tidy 目标 Qt6（:31,44） |
| `example/gerber2image/` | ✅ `main.cpp`(49 行) + `CMakeLists.txt`（Qt6+gflags+rc/qrc） + `app_win32.rc` + `gerber2image.qrc/.ico/.png` |
| `example/gerber_viewer/`、`example/gerber_viewer_qgraphics/` | ✅ 删除对象 |
| `src/pygerber-parser/`（3 文件）+ `test_python_binding.py` + `PYTHON_USAGE.md` | ✅ 删除对象 |
| `.gitmodules` | ✅ 3 子模块：gflags / googletest / pybind11 |
| `.github/workflows/workflow.yml` | ✅ install-qt-action@v3（:20-26）；configure `-DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON`（:35）；无 Python 步骤 |
| `vcpkg.json` / `CMakePresets.json` | ❌ 不存在 → 新建 |
| 解析器调用方流 | ✅ `Aperture::Draw(Engine*)`（aperture.cpp:67-74）→ primitive `Draw(*engine)`（gerber_primitive.cpp）：Flash→`DrawAperture`、Rectangle→`AddRect`、Circle→`AddCircle`、Stroke→`BeginDrawStroke/CachePoint/MoveTo/LineTo/EndDrawStroke`、Outline→`BeginDrawOutline/MoveTo/LineTo/CubicTo(Arc)/EndDrawOutline`；`GerberLayer::Draw` 处理 `convert_strokes2fills_`（gerber_layer.cpp:105-117） |

**边界（不触碰）:** `src/parser/**`（vendored，其内部 CMake/示例含独立 gtest/pybind11 引用，不参与顶层构建）、`src/parser/3rdparty/**`。

---

## Phase 1: 文件结构映射

| 行为 | 文件 | 说明 |
|---|---|---|
| 新建 | `vcpkg.json` | 依赖清单（blend2d/gtest/gflags 全文见 T1） |
| 新建 | `CMakePresets.json` | vcpkg toolchain preset（全文见 T1） |
| 新建 | `src/engines/blend2d_engine.h` / `blend2d_engine.cpp` | 新引擎 + `blend2d_semantics` 纯函数集（算法内聚于此，transformation 不保留独立模块） |
| 新建 | `tests/blend2d_engine_test.cpp` | 指令映射 BLPath 结构断言 + 语义纯函数单测（GLOB 自动入编 TestGerberRenderer） |
| 新建 | `tests/image_diff.h` | 容差比对统计工具（M3 金标用） |
| 新建 | `tests/test_data/gerber/gerber_files/blend2d_unit_*.gbr` | 4 个微型夹具（circle_line / positive / negative / steprepeat / inches = 5 个） |
| 修改 | `CMakeLists.txt`（顶层）、`src/CMakeLists.txt`、`tests/CMakeLists.txt`、`example/gerber2image/CMakeLists.txt`、`README.md`、`README_zh.md`、`.github/workflows/workflow.yml` | 见各任务 |
| 修改 | `tests/gerber_renderer_test.cpp` | M3 重写为 Blend2D 渲染 + 容差比对 |
| 删除 | `src/engines/qpainter_engine.*`、`qgraphics_scene_engine.*`、`transformation.*`；`example/gerber_viewer/`、`example/gerber_viewer_qgraphics/`；`src/pygerber-parser/`、`test_python_binding.py`、`PYTHON_USAGE.md`；`3rdparty/{gflags,googletest,pybind11}`、`.gitmodules`；`tests/test_data/gerber/results/*.bmp`（换 PNG） | 各任务 |

---

## Phase 2: 批次与依赖总览

### 批次总览（批次类型预标注；`∥` = 文件集不相交可并行；批内串行 = 共享文件或依赖）

| 批次 | 任务 | 类型 | 依赖 | 批内并行性 | SHA 基线记录点 |
|---|---|---|---|---|---|
| A | T1 vcpkg.json + CMakePresets | **契约批** | — | — | A 后记录 |
| B | T2 依赖切 vcpkg；T3 CI 加 vcpkg | 机械批 | A | T2 ∥ T3（CMake× ↔ workflow.yml） | B 后记录 |
| C | T4 引擎头+基础 path 方法；T5 语义纯函数 | **契约批** | B | 串行（共享 blend2d_engine.* + 测试文件） | C 后记录 |
| D | T6 AddRect/AddCircle+Outlines；T7 DrawAperture+Strokes | 机械批 | C | 串行（共享同一批文件） | D 后记录 |
| E | T8 RenderGerber 层/负片/背景；T9 step-repeat+变换+inches | 机械批 | D | 串行（共享同一批文件）；**检查点：Task T9（M1 收尾 ctest 全绿）** | E 后记录 |
| F | T10 删 Python 构建/文件；T11 删 README Python 内容 | 机械批 | —（独立） | T10 ∥ T11（CMake+文件 ↔ READMEs）；**F ∥ G 可并行** | F 后记录 |
| G | T12 image_diff 工具+测试；T13 金标重写+基线重生成；T14 tests/CMake 去 Qt | **契约批（T12 定义比对契约）→ 机械** | E | 串行（T13 依赖 T12，T14 依赖 T13）；**F ∥ G 可并行** | G 后记录 |
| H | T15 gerber2image 改造 Blend2D | 机械批 | E | — | H 后记录 |
| I | T16 删 Qt 引擎；T17 删查看器+tidy 清理；T18 README Qt 清理；T19 CI 去 Qt | 机械批 | H（T16 依赖 T15） | T16 ∥ T17 ∥ T18 ∥ T19（四文件集两两不相交） | I 后记录 |
| J | T20 全量验收 | **机械批，检查点：Task T20（全量 grep + ctest）** | I | — | J 后记录 |

依赖链：`A → B → C → D → E → {F ∥ G} → H → I → J`。M2（F）与 M3（G）文件集不相交（F: src/CMakeLists.txt、顶层 CMakeLists、READMEs、删除文件；G: tests/*、基线），可并行；M1（C-D-E）与 M2 均触及 `src/CMakeLists.txt` → 必须串行（E 在前）。

**批次 commit 策略（每批一条 commit）:** 批次内逐任务完整 TDD 红绿循环（纪律不减），全部 GREEN 后一次 commit；批内某任务 BLOCKED 时，对已完成且测试通过的任务**抢救 commit**（commit message 注明被阻塞任务编号）。每批完成后 `git rev-parse HEAD` 记入下表（Creative 执行时回填）：

```
SHA 基线:
A: ____  B: ____  C: ____  D: ____  E: ____  F: ____  G: ____  H: ____  I: ____  J: ____
```

---

## Phase 2: 任务明细

---

### Task T1: vcpkg.json + CMakePresets.json（M0 基座）

**What（计划锁定，不可改）:**

**Files:**
- Create: `vcpkg.json`
- Create: `CMakePresets.json`

**声明式产物（保留全文）:**

`vcpkg.json`：
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

`CMakePresets.json`：
```json
{
  "version": 2,
  "cmakeMinimumRequired": { "major": 3, "minor": 20, "patch": 0 },
  "configurePresets": [
    {
      "name": "default",
      "displayName": "Default (vcpkg manifest)",
      "generator": "Unix Makefiles",
      "binaryDir": "${sourceDir}/build",
      "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",
        "BUILD_TESTS": "ON",
        "BUILD_EXAMPLES": "ON",
        "BUILD_TESTING": "OFF"
      }
    }
  ],
  "buildPresets": [
    { "name": "default", "configurePreset": "default" }
  ],
  "testPresets": [
    { "name": "default", "configurePreset": "default", "output": { "outputOnFailure": true } }
  ]
}
```

**验证命令（可复制粘贴）:**
- 前置：本机需已装 Qt6（M0 阶段 Qt 仍在构建中，`find_package(Qt6)` 照旧）；`export VCPKG_ROOT=<vcpkg 安装目录>`（vcpkg 须已 clone）。
- `cmake --preset default` → 预期：configure 成功；日志显示 vcpkg manifest 模式自动安装 `blend2d/gtest/gflags`（首次较慢）
- `cmake --build build -j$(nproc)` → 预期：构建成功（Qt 引擎/测试如旧）
- `ctest --test-dir build --output-on-failure` → 预期：`TestGerberRenderer` 全绿（旧 Qt 金标全部通过，含像素相等）

**How（executor 自由）:**
- 本文档为纯配置产物，无实现本体。若本机无 `VCPKG_ROOT`，改用等价 `cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<绝对路径>/scripts/buildsystems/vcpkg.cmake -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON -DBUILD_TESTING=OFF` 验证。

---

### Task T2: 第三方依赖切换到 vcpkg（gtest/gflags/blend2d 就位）

**What（计划锁定，不可改）:**

**Files:**
- Modify: `tests/CMakeLists.txt`
- Modify: `example/gerber2image/CMakeLists.txt`
- Modify: `src/CMakeLists.txt`
- Modify: `CMakeLists.txt`（顶层）
- Delete: `3rdparty/googletest/`、`3rdparty/gflags/`（子模块，`git rm -r`）
- Modify: `.gitmodules`（移除 gflags、googletest 两个条目）

**接口/配置变更点（逐文件锁定）:**
- `tests/CMakeLists.txt`：删除 `find_package(Qt6 ...)` 与 `set(CMAKE_AUTOMOC ON)`；新增 `find_package(GTest CONFIG REQUIRED)` 与 `find_package(Blend2D CONFIG REQUIRED)`；`target_link_libraries` 中 `gtest`/`gtest_main` 改为 `GTest::gtest`/`GTest::gtest_main` 并追加 `Blend2D::Blend2D`。**注意：本任务仍保留测试内容对 Qt 的 include（QApplication/QImage 仍被 tests/gerber_renderer_test.cpp 使用），去 Qt 在 M3-T14。**
- `example/gerber2image/CMakeLists.txt`：删除 `find_package(Qt6 ...)` 与 `set(CMAKE_AUTORCC ON)` 之外先去 Qt？——**不**：本任务仅改 gflags 来源：新增 `find_package(gflags CONFIG REQUIRED)`；链接目标 `gflags` → `gflags::gflags`。Qt 保持到 T15 移除。
- `src/CMakeLists.txt`：新增 `find_package(Blend2D CONFIG REQUIRED)`（暂不链接；M1-T4 才把 `Blend2D::Blend2D` 加入 link）。
- 顶层 `CMakeLists.txt`：删除 `add_subdirectory(3rdparty/gflags)`（:15）与 `add_subdirectory(3rdparty/googletest)`（:23）。
- `git rm -r 3rdparty/googletest 3rdparty/gflags`；`.gitmodules` 删除对应两段，仅剩 pybind11 条目。

**验证命令（可复制粘贴）:**
- `cmake --preset default` → 预期：configure 成功；GTest/Blend2D 由 vcpkg 提供（无 submodule 警告）
- `grep -n 'add_subdirectory(3rdparty' CMakeLists.txt` → 预期：仅剩 `add_subdirectory(3rdparty/pybind11)` 与 `add_subdirectory(src)`
- `cmake --build build -j$(nproc)` → 预期：成功
- `ctest --test-dir build --output-on-failure` → 预期：TestGerberRenderer 全绿（Qt 金标仍过）

**How（executor 自由）:**
- 换依赖来源的机械改动；链接目标名与 vcpkg 提供者不符时（如 gflags 别名差异），以 `find_package` 实际导出的目标为准修正链接名（契约不变：必须走 vcpkg 包，禁止手动路径）。

---

### Task T3: CI 增加 vcpkg 集成（保留 Qt）

**What（计划锁定，不可改）:**

**Files:**
- Modify: `.github/workflows/workflow.yml`

**变更点（锁定）:**
- `Install Qt` 步骤（jurplel/install-qt-action@v3）**保留**（Qt 移除在 T19）。
- checkout 后新增 vcpkg 设置步骤（如 `lukka/run-vcpkg@v11`，manifest 模式自动装依赖；vcpkg 版本须固定——用 action 默认快照或显式 `vcpkgGitCommitId`，不得漂移）。
- `Configure CMake` 步骤的 `cmake -B ...` 命令追加 `-DCMAKE_TOOLCHAIN_FILE=${{ github.workspace }}/vcpkg/scripts/buildsystems/vcpkg.cmake`（路径与 vcpkg action 输出一致）。
- 构建参数保持 `-DBUILD_TESTS=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=ON`。

**验证命令（可复制粘贴）:**
- 语法/行为验证：`python3 -c "import yaml,sys; yaml.safe_load(open('.github/workflows/workflow.yml')); print('yaml ok')"`（若本机无 PyYAML，可跳过，以 PR 中 Action 实际运行为准）→ 预期：`yaml ok`
- 本地不做 CI 全量跑；由 Creative 在 PR 触发后确认 build job 通过。

**How（executor 自由）:**
- vcpkg action 选型与版本固定方式按 CI 生态现状调整；`CMAKE_TOOLCHAIN_FILE` 路径必须以 action 输出真实路径为准。

---

### Task T4: Blend2DEngine 头文件 + 基础 path 方法（契约锚）

**What（计划锁定，不可改）:**

**Files:**
- Create: `src/engines/blend2d_engine.h`
- Create: `src/engines/blend2d_engine.cpp`
- Create: `tests/blend2d_engine_test.cpp`
- Modify: `src/CMakeLists.txt`（`target_link_libraries(gerber_engine ...)` 追加 `Blend2D::Blend2D`，Qt 链接保留）

**接口签名（锁定，防漂移）:**

```cpp
// —— src/engines/blend2d_engine.h
#pragma once
#include <cstdint>
#include <memory>
#include <utility>

#include <blend2d.h>

#include "engine/engine.h"

class Aperture;
class Gerber;
class BoundBox;

class Blend2DEngine : public Engine {
 public:
  static constexpr int kTimes = 10000;

  Blend2DEngine(BLImage &image, const BoundBox &bound_box, double offset);

  // 渲染入口（等价 QPainterEngine 的对外入口；Begin/EndRender 不对外暴露）
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
};
```

**测试规格（= 行为规格，tests/blend2d_engine_test.cpp 新增；executor 据此写测试，断言不改）:**
- 测试夹具：`BLImage img(64, 64, BLFormat::PRGB32); Blend2DEngine engine(img, BoundBox(0.001, 0.021, 0.021, 0.001), 0.0);`
- `MoveTo 后 CurrentOutlinePath 结构`: MoveTo({1.0, 2.0}) → path 顶点数 == 1；第 0 个顶点坐标 == {10000.0, 20000.0}（kTimes=10000 生效）
- `MoveTo+LineTo 后结构`: MoveTo({0.0, 0.0}); LineTo({3.0, 4.0}) → 顶点数 == 2；末顶点 == {30000.0, 40000.0}
- `CubicTo 后结构`: MoveTo({0.0, 0.0}); CubicTo({1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}) → 顶点数 == 4（起点 + 2 控制点 + 终点）；末顶点 == {30000.0, 30000.0}
- `CurrentPos`: 上一步后 `CurrentPos()` == {3.0, 3.0}（÷ kTimes 还原逻辑坐标）；仅 MoveTo({1.0, 2.0}) 后 == {1.0, 2.0}
- `CachePoint/CachedPoint`: CachePoint({7.5, -3.25}) → CachedPoint() == {7.5, -3.25}；默认（未 Cache 前）== {0.0, 0.0}
- 空引擎 `CurrentOutlinePath()` 顶点数 == 0

**验证命令（可复制粘贴）:**
- RED（头文件声明已存在、实现未写）：`cmake --build build -j$(nproc) 2>&1 | tail -30` → 预期：链接失败，报 `undefined reference to Blend2DEngine::MoveTo/LineTo/CubicTo/...`（因功能缺失而失败）
- GREEN：`cmake --build build -j$(nproc)` → 预期：构建成功
- `ctest --test-dir build --output-on-failure` → 预期：新增断言通过；原有 Qt 金标 21 用例仍绿

**How（executor 自由）:**
- 顶点计数/坐标获取用 BLPath 实际 API（getSize/getVertexData/getLastVertex 等），若与上述预期计数不符（如 getSize 含 moveTo 计数的差异），属断言适配，须以注释记录偏差；实现仅 MoveTo/LineTo/CubicTo/CurrentPos/CachePoint/CachedPoint 六方法 + CurrentOutlinePath 返回当前 path 拷贝；其余 Engine 方法可先留空实现（本任务不测则不改断言，后续任务补齐）。

---

### Task T5: 语义纯函数集（blend2d_semantics）

**What（计划锁定，不可改）:**

**Files:**
- Modify: `src/engines/blend2d_engine.h`（追加命名空间声明）
- Modify: `src/engines/blend2d_engine.cpp`（实现）
- Modify: `tests/blend2d_engine_test.cpp`（追加测试）

**接口签名（锁定）:**

```cpp
namespace blend2d_semantics {
// 逻辑窗口 ↔ 物理视口映射（等价 transformation.cpp:14-91 的算法；Qt 类型全部替换）
struct ViewTransform {
  double left, top, width, height;          // 逻辑窗口（bound_box.Scale(kTimes) 空间，height 为负值语义见 GetPainterWindow）
  double viewport_left, viewport_top,
         viewport_width, viewport_height;   // 物理视口
};
ViewTransform ComputeViewTransform(const BoundBox &scaled_box, double offset,
                                   int physical_width, int physical_height);

// step-and-repeat 平移序列（y 外层、x 内层，与 qpainter_engine.cpp:113-120 循环序一致）
std::vector<std::pair<double, double>> StepRepeatTranslations(
    int count_x, int count_y, double step_x, double step_y);

// 负片/clear 前景背景互换：negative ? background : foreground
BLRgba32 ResolveFillColor(bool negative, const BLRgba32 &background,
                          const BLRgba32 &foreground);

// 构造已按 kTimes 缩放坐标的矩形/圆形 path（AddRect/AddCircle 的构建内核）
BLPath MakeRectPath(double x, double y, double w, double h);
BLPath MakeCirclePath(double x, double y, double radius);
}
```

**测试规格（= 行为规格，断言不改）:**

`ComputeViewTransform`（box 输入为**已 Scaled(kTimes)** 的 BoundBox）：
- 输入：`BoundBox(0.0, 200000.0, 100000.0, 0.0)`（即 20×10），offset=0.005，物理 1600×1600；
  预期：`{left=0, top=150000, width=200000, height=200000, viewport_left=8, viewport_top=8, viewport_width=1584, viewport_height=1584}`
  （物理宽=(1-2×0.005)×1600=1584；scale_x=1584/200000=0.00792 < scale_y=1584/100000=0.01584 → 左侧贴合、Y 方向留 margin=100000×(2-1)/2=50000）
- 输入：正方形 `BoundBox(0.0, 100000.0, 100000.0, 0.0)`，offset=0.0，物理 64×64；
  预期：`{left=0, top=100000, width=100000, height=100000, viewport_left=0, viewport_top=0, viewport_width=64, viewport_height=64}`

`StepRepeatTranslations`：
- 输入：count_x=2, count_y=3, step_x=10.0, step_y=20.0 → 预期（顺序锁定，y 外 x 内）：
  `{(0,0),(10,0),(0,20),(10,20),(0,40),(10,40)}`
- 输入：count_x=1, count_y=1 → 预期：`{(0,0)}`
- 输入：count_x=0 或 count_y=0 → 预期：空 vector（空 layer 跳过语义）

`ResolveFillColor`：
- (negative=false, bg=0xFF000000, fg=0xFFFFFFFF) → 0xFFFFFFFF
- (negative=true,  bg=0xFF000000, fg=0xFFFFFFFF) → 0xFF000000

`MakeRectPath`：
- AddRect 语义同 qpainter_engine.cpp:194-198：矩形左下角 (x,y)，宽 w 高 h，顶点 ×kTimes。
- 输入：x=0.001, y=0.001, w=0.010, h=0.010 → path 顶点数 == 4；顶点合计覆盖 `{(10,10),(110,10),(110,110),(10,110)}`（顺序及值精确；自选闭合方式，保证 4 顶点即可）

`MakeCirclePath`：
- 输入：x=0.010, y=0.010, radius=0.005 → path 顶点数 >= 8（圆用多段贝塞尔近似）；所有顶点到 (100,100) 的距离 == 50（±1e-6）

**验证命令（可复制粘贴）:**
- RED：先写测试 → `cmake --build build -j$(nproc)` → 预期：编译/链接失败（`blend2d_semantics` 未定义，功能缺失）
- GREEN：`cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure` → 预期：全部通过且全量（含 Qt 金标）绿

**How（executor 自由）:**
- ComputeViewTransform 按 transformation.cpp:14-91 公式逐行翻译成非 Qt 类型；禁止将 `transformation.{h,cpp}` 保留为独立模块。

---

### Task T6: AddRect / AddCircle + Begin/EndDrawOutline

**What（计划锁定，不可改）:**

**Files:**
- Modify: `src/engines/blend2d_engine.cpp`
- Modify: `tests/blend2d_engine_test.cpp`

**测试规格（= 行为规格，断言不改）:**

夹具：`BLImage img(64, 64, PRGB32); Blend2DEngine engine(img, BoundBox(0.001, 0.021, 0.021, 0.001), 0.0);`（映射见 T5：窗口 kTimes 空间 (10..210)，物理 0..64，offset 0：x_dev=(x−10)×0.32，y_dev=(210−y)×0.32）

- `AddRect 立即填充`: engine.AddRect(0.005, 0.005, 0.010, 0.010)（逻辑 kTimes 50..150 → 设备 x 12.8..44.8、y 19.2..51.2）→ 像素 (32,32) == 0xFFFFFFFF（前景白）；像素 (0,0) == 0x00000000（背景黑，先 `engine.DrawBackground()` 或先铺黑）；(8,8) 在矩形外 == 黑；调用后 `CurrentOutlinePath()` 顶点数 == 0（立即清空语义，同 qpainter_engine.cpp:197）
- `AddCircle 立即填充`: engine.AddCircle(0.010, 0.010, 0.005)（圆心 kTimes (110,110)→设备 (32,32)，半径 50 单位→16px）→ 像素 (32,32) == 白；(32,2)（中心上方 30px，半径外）== 黑；调用后 CurrentOutlinePath 空
- `Begin/EndDrawOutline 累积填充`: BeginDrawOutline(); MoveTo({0.003,0.003}); LineTo({0.017,0.003}); LineTo({0.017,0.017}); LineTo({0.003,0.017}); EndDrawOutline() → 在 EndDrawOutline **之前** `CurrentOutlinePath()` 顶点数 == 4（未清空）；EndDrawOutline 后顶点数 == 0（已填充并清空）；像素 (10,10)（矩形内）== 白；(50,50)（矩形外）== 黑
- 负片参与先不测（T8 覆盖），本任务 fill 色恒为前景白（默认 negative=false）

**验证命令（可复制粘贴）:**
- RED：先写测试 → `cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure 2>&1 | grep -A5 "blend2d\|AddRect\|AddCircle"` → 预期：新断言 FAIL（未实现）
- GREEN：实现后 `cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure` → 预期：全绿

**How（executor 自由）:**
- AddRect/AddCircle 内部 = MakeRectPath/MakeCirclePath 追加/合并到当前 path 后 `fillPath` 并清空；EndDrawOutline = 填充当前 path 并清空；BeginDrawOutline = 设当前填充色（negative ? bg : fg）；BLContext 生命周期自管（构造即 begin，析构 end）。像素探针避开 AA 边缘（探针取形状中心）。

---

### Task T7: DrawAperture + Begin/EndDrawStroke（含夹具）

**What（计划锁定，不可改）:**

**Files:**
- Modify: `src/engines/blend2d_engine.cpp`
- Modify: `tests/blend2d_engine_test.cpp`
- Create: `tests/test_data/gerber/gerber_files/blend2d_unit_circle_line.gbr`

**声明式产物（夹具全文，保留）:**

`tests/test_data/gerber/gerber_files/blend2d_unit_circle_line.gbr`：
```
G04 Blend2D unit fixture: circle aperture flash + stroke line*
%FSLAX24Y24*%
%MOMM*%
%ADD10C,0.4*%
D10*
X10000Y10000D03*
G01*
X20000Y20000D02*
M02*
```
（语义：D10=圆孔 Ø0.4mm；flash 于 (1.0000,1.0000)；从 (1,1) 到 (2,2) 的 stroke 线，线宽 0.4mm。若解析器对语法细节有出入，executor 以解析器实际接受为准修正夹具文本——测试夹具修正，语义不变。）

**测试规格（= 行为规格，断言不改）:**

- 夹具解析前置断言：`GerberParser(TestData + "gerber_files/blend2d_unit_circle_line.gbr")` → `GetGerber()` 成功；`GetBBox()` 的 Right≈2.2、Top≈2.2（EXPECT_NEAR，0.01 容差）
- `DrawAperture 单个 flash 落位`: 取 `gerber->GetAperture(10)`；构造引擎（bbox = gerber->GetBBox()，img = 512×512 PRGB32，offset=0.005，先 DrawBackground 黑）；`engine.DrawAperture(aperture, {1.0, 1.0})` → 用 T5 的 ComputeViewTransform 映射 (1.0,1.0) 得设备坐标 probe_center，断言 PixelAt(probe_center) == 白；距 probe_center 0.3mm 的对应像素 == 黑（flash 半径 0.2mm，避开 AA 边缘）
- `Begin/EndDrawStroke SolidCircle 描边`: 解析夹具后 `gerber->GetLayers()[0]->Draw(&engine)` 全程走通不崩 + 线中点设备坐标像素 == 白（线宽 0.4mm 覆盖中线）；线垂直方向 0.3mm 外 == 黑（Stroke→BeginDrawStroke→MoveTo/LineTo→EndDrawStroke 全链）
- `BeginDrawStroke 线宽取 BBox().Width()`: 结构层面不直接可断言 → 以"线中部像素为白、线缘外为黑"的几何探针验证（等价线宽 ≥ 0.4mm×kTimes 映射后覆盖中线）

**验证命令（可复制粘贴）:**
- RED：`cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure 2>&1 | grep -B2 -A8 "DrawAperture\|DrawStroke\|blend2d_unit"` → 预期：新断言 FAIL（DrawAperture/Stroke 未实现）
- GREEN：`cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure` → 预期：全绿

**How（executor 自由）:**
- DrawAperture：在 BLContext 上 save → translate(start×kTimes) → `aperture->Draw(this)`（aperture 内部 primitives 走本引擎 Add/Outline 流程填充）→ restore；不复刻 QPixmap 离屏缓存（性能优化非语义，等价性由像素探针保证）。
- BeginDrawStroke：aperture→SolidCircle() → stroke 样式 {color=negative?bg:fg, width=BBox().Width()×kTimes, BL_STROKE_CAP_ROUND, BL_STROKE_JOIN_ROUND}；SolidRectangle() → 走填充模式。EndDrawStroke：strokePath/fillPath 并清空 path。

---

### Task T8: RenderGerber（逐 layer + negative 互换）+ DrawBackground

**What（计划锁定，不可改）:**

**Files:**
- Modify: `src/engines/blend2d_engine.cpp`
- Modify: `tests/blend2d_engine_test.cpp`
- Create: `tests/test_data/gerber/gerber_files/blend2d_unit_positive.gbr`
- Create: `tests/test_data/gerber/gerber_files/blend2d_unit_negative.gbr`

**声明式产物（夹具全文，保留）:**

`blend2d_unit_positive.gbr`：
```
G04 Blend2D unit fixture: dark layer single flash*
%FSLAX24Y24*%
%MOMM*%
%LPD*%
%ADD10C,0.4*%
D10*
X10000Y10000D03*
M02*
```

`blend2d_unit_negative.gbr`：
```
G04 Blend2D unit fixture: clear (negative) layer single flash*
%FSLAX24Y24*%
%MOMM*%
%LPC*%
%ADD10C,0.4*%
D10*
X10000Y10000D03*
M02*
```
（语义：positive=D10 Ø0.4 黑层单 flash@(1,1)；negative=同几何 clear 层。若解析器 LPD/LPC 语法有出入，executor 按解析器实际接受形式修正夹具——测试夹具修正，正/负语义不变。）

**测试规格（= 行为规格，断言不改）:**

- `DrawBackground`: img 64×64；`engine.DrawBackground()` → 全图像素 == 0xFF000000；`engine.DrawBackground(BLRgba32(0xFFFFFFFF))` → 全图 == 0xFFFFFFFF（默认参数 + 显式色）
- `RenderGerber positive 层`: 解析 positive 夹具 → 512×512 引擎（bbox=gerber->GetBBox(), offset=0.005）→ DrawBackground 黑 → RenderGerber → 映射 (1.0,1.0) 设备像素 == 白；图像左下角（bbox 外）== 黑
- `RenderGerber negative 层互换`: negative 夹具同流程 → 映射 (1.0,1.0) 像素 == **黑**（负片以背景色填充 = 擦除语义，非白色）；且图像左上角（bbox 外）== 黑（背景未被破坏）
- `空 layer 跳过`: 手工构造 bbox 无内容夹具不可行则由 executor 用 `Gerber` 空对象（无层）调用 RenderGerber → 返回 0，不崩溃（空 layer 跳过）
- `SetConvertStroke2Fills(true)` 后对 circle_line 夹具 RenderGerber 不返回非零（parser 侧 StrokesToFillsConverter 路径走通；转换后输出以 T13 金标 stroke2fill 兜底）
- `RenderGerber 返回值`: 上述全部 == 0

**验证命令（可复制粘贴）:**
- RED：`cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure 2>&1 | grep -B2 -A8 "RenderGerber\|DrawBackground\|negative\|positive"` → 预期：新断言 FAIL
- GREEN：`cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure` → 预期：全绿

**How（executor 自由）:**
- RenderGerber 骨架照 qpainter_engine.cpp:100-134：`gerber->GetLayers()` 遍历；每 layer `negative_ = layer->IsNegative()`；非 copy layer 直接 `layer->Draw(this)`；copy layer 的平移循环本体在 T9 落地（本任务可先留 `if (layer->IsCopyLayer())` 分支按单次绘制处理，T9 再接入 StepRepeatTranslations 平移——**注意**：本任务断言不含 copy layer，T9 才测）。

---

### Task T9: step-and-repeat + context 变换 + inches/mm（M1 收尾）

**What（计划锁定，不可改）:**

**Files:**
- Modify: `src/engines/blend2d_engine.cpp`
- Modify: `tests/blend2d_engine_test.cpp`
- Create: `tests/test_data/gerber/gerber_files/blend2d_unit_steprepeat.gbr`
- Create: `tests/test_data/gerber/gerber_files/blend2d_unit_inches.gbr`

**声明式产物（夹具全文，保留）:**

`blend2d_unit_steprepeat.gbr`：
```
G04 Blend2D unit fixture: step-and-repeat 2x2 at 1mm*
%FSLAX24Y24*%
%MOMM*%
%SRX2Y2I1.0J1.0*%
%ADD10C,0.4*%
D10*
X10000Y10000D03*
%SR*%
M02*
```
（语义：SR 2×2，I/J 步长 1.0mm，块内 1 次 flash@(1,1) → 实际 4 个 flash 于 (1,1),(2,1),(1,2),(2,2)。若解析器 SR 语法有出入，executor 按解析器实际接受的 SR 扩展修正夹具——2×2@1mm 语义不变。）

`blend2d_unit_inches.gbr`：
```
G04 Blend2D unit fixture: inches units, flash at 1 inch*
%FSLAX24Y24*%
%MOIN*%
%ADD10C,0.4*%
D10*
X10000Y10000D03*
M02*
```
（语义：inch 制，D10=Ø0.4in，flash@(1.0000in,1.0000in)=(25.4mm,25.4mm)。）

**测试规格（= 行为规格，断言不改）:**

- `RenderGerber copy layer 平移`: steprepeat 夹具 → 512×512（bbox=gerber->GetBBox()，offset=0.005，DrawBackground 黑）→ RenderGerber → ComputeViewTransform 映射 4 个 flash 中心 (1,1),(2,1),(1,2),(2,2) 的设备坐标 → 4 像素全部 == 白；映射 (1.5,1.5)（4 单元间隙，距最近中心 0.707mm > 0.2mm 半径）== 黑
- `StepRepeatTranslations 与 RenderGerber 集成`（白盒等价）：对同一夹具 engine 内部平移次数 == count_x×count_y（本项可由 executor 以 `layer->IsCopyLayer()` 真实夹具 + 输出像素计数 ≥4 替代，确保不引入 mock）
- `context 变换生效`: 同一几何在两种 offset（0.005 与 0.02）渲染 → 各自映射的 flash 中心均为白（证明变换由 ComputeViewTransform 驱动而非硬编码）
- `inches 单位端到端`: inches 夹具 → `gerber->Unit() == UnitType::guInches`；`GetBBox().Right()` ≈ 30.48（25.4+5.08，EXPECT_NEAR 0.05）→ 渲染后映射 (25.4,25.4) 像素 == 白
- `RenderGerber 返回值` == 0（涉及 copy layer 全程）

**验证命令（可复制粘贴）:**
- RED：`cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure 2>&1 | grep -B2 -A8 "steprepeat\|inches\|StepRepeat"` → 预期：新断言 FAIL
- GREEN：`cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure` → 预期：全绿
- **M1 检查点（Task T9）**: `ctest --test-dir build --output-on-failure` → 预期：**全部测试通过**（新单测 5+ 组 + 旧 Qt 金标 21 用例全绿）；`git rev-parse HEAD` 记录为 E 基线

**How（executor 自由）:**
- copy layer 平移：`for y/count_y` 外、`for x/count_x` 内、每次 `ctx.save(); ctx.translate(x*step_x*kTimes, y*step_y*kTimes); layer->Draw(this); ctx.restore();` 或等价 context 级平移（与 qpainter_engine.cpp:113-120 循环序一致，偏移量 = StepRepeatTranslations(i×j) 输出 × kTimes）。
- context 应用 ComputeViewTransform：将窗口→视口仿射（含 Y 翻转：height 为负）以 BLMatrix2D translate/scale 组合 set 到 BLContext；路径顶点保持 kTimes 逻辑空间。

---

### Task T10: 删除 Python 支持层（构建 + 文件）

**What（计划锁定，不可改）:**

**Files:**
- Delete: `src/pygerber-parser/`（整个目录：CMakeLists.txt、pygerber-parser.cpp、pygerber-parser.py）
- Delete: `test_python_binding.py`
- Delete: `PYTHON_USAGE.md`
- Delete: `3rdparty/pybind11/`（子模块，`git rm -r`）
- Modify: `.gitmodules`（移除 pybind11 条目；条目清空后 `git rm .gitmodules`）
- Modify: `CMakeLists.txt`（顶层，删除 `add_subdirectory(3rdparty/pybind11)` :11）
- Modify: `src/CMakeLists.txt`（删除 `add_subdirectory(pygerber-parser)` :19）

**验证命令（可复制粘贴）:**
- `git rm -r 3rdparty/pybind11 src/pygerber-parser`；`git rm test_python_binding.py PYTHON_USAGE.md`
- `grep -rn 'pybind11\|pygerber' CMakeLists.txt src/CMakeLists.txt tests/CMakeLists.txt example/gerber2image/CMakeLists.txt` → 预期：无命中（vendored `src/parser/**` 除外，不查）
- `cmake --preset default` → 预期：configure 成功（不再触碰 pybind11/Python3）
- `cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure` → 预期：全绿（与 M1 后状态一致）
- `git submodule status` → 预期：无 pybind11 输出；`.gitmodules` 不存在或为空

**How（executor 自由）:**
- 纯删除动作，无实现本体。注意 `src/parser/**` 内引用的 pybind11 不在范围（vendored，不参与顶层构建）。

---

### Task T11: 删除 README 中 Python 绑定内容

**What（计划锁定，不可改）:**

**Files:**
- Modify: `README.md`
- Modify: `README_zh.md`

**变更点（锁定，README.md 实际行号）:**
- :24 `- **Python bindings**: Complete Python interface through pybind11` / zh :24 → 删除该条目
- :70 目录树 `└── pygerber-parser/  # Python bindings` / zh :70 → 删除该行
- :94 `- Python 3.6+ (optional, for Python bindings)` / zh :94 → 删除
- :164-187 `### Python API` 整节（含 import 示例、gerber2image/GerberParser 用法、PYTHON_USAGE.md 指引）→ 删除
- :244 `- [pybind11](...) - Python binding generator` / zh :244 → 删除致谢条目

**验证命令（可复制粘贴）:**
- `grep -n 'pybind11\|pygerber\|Python API\|Python 3.6' README.md README_zh.md` → 预期：无命中
- `grep -c 'pybind11\|pygerber' README.md README_zh.md` → 预期：0

**How（executor 自由）:**
- 纯文本删除；不触碰 Qt 相关段落（T18 处理）。zh 与 en 保持同步修改。

---

### Task T12: 图像容差比对工具 image_diff.h（契约定义）

**What（计划锁定，不可改）:**

**Files:**
- Create: `tests/image_diff.h`
- Modify: `tests/blend2d_engine_test.cpp`（追加 diff 工具单测）

**接口签名（锁定）:**

```cpp
#pragma once
#include <blend2d.h>

struct ImageDiffStats {
  double mean_abs_diff;  // 每像素"最大通道差"的均值（0..255）
  int    max_abs_diff;   // 所有像素最大通道差
  double diff_ratio;     // 最大通道差 > threshold 的像素占比（0..1）
};

// actual vs expected 按同尺寸逐像素比较（尺寸不等视为失败由调用方先断言）
ImageDiffStats CompareImages(const BLImage &actual, const BLImage &expected,
                             int threshold = 32);

// 读取 PRGB32/XRGB32 图像 (x,y) 像素（不透明像素两种格式数值一致）
uint32_t PixelAt(const BLImage &image, int x, int y);
```

**测试规格（= 行为规格，断言不改）：**

构造 4×4 全黑图 A（每像素 0xFF000000）与变体 B：
- B 全黑（与 A 全同）→ `{mean=0.0, max=0, ratio=0.0}`
- B 仅在 (0,0) 为白 0xFFFFFFFF → `{mean=255.0/16=15.9375, max=255, ratio=1.0/16=0.0625}`（threshold=32）
- B 在 (0,0) 通道差恰为 32（如 0xFF202020 vs 黑）→ threshold=32 时 `ratio=0.0`（严格大于才计入）；用 33（0xFF212121）→ `ratio=0.0625`
- PixelAt：A 的 (0,0) == 0xFF000000；白像素 == 0xFFFFFFFF

**验证命令（可复制粘贴）:**
- RED：`cmake --build build -j$(nproc)` 2>&1 | tail -5 → 预期：编译失败（image_diff.h 不存在 / CompareImages 未定义，功能缺失）
- GREEN：`cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure` → 预期：全绿

**How（executor 自由）:**
- 逐像素通道差取绝对值最大通道；mean 用 double 累加；ratio 用严格 > threshold 计数。像素读取对齐 PRGB32/XRGB32 内存布局。

---

### Task T13: 重写 gerber_renderer_test.cpp（Blend2D 金标 + 基线重生成 + 阈值调定）

**What（计划锁定，不可改）:**

**Files:**
- Modify: `tests/gerber_renderer_test.cpp`（整体重写）
- Delete: `tests/test_data/gerber/results/*.bmp`（21 个旧 QPainter 基线）
- Create: `tests/test_data/gerber/results/*.png`（19 个新 Blend2D 基线，本任务生成并提交）

**测试规格（= 行为规格，断言不改）:**

重写后用例清单（19 个；fixture 路径、bbox 元断言与现文件一致）：
1. `TestRenderFromGerber` — `2301113563-f-gtl`（保留 `EXPECT_FALSE(gerber->IsNegative())`、`EXPECT_EQ(gerber->Name(), "")`）
2. `TestConvertStroke2Fill` — `2301113563-f-gtl`，`engine.SetConvertStroke2Fills(true)`
3-8. `TestRenderGerberFile1..6` — `2301113987c.dat`、`2301113987c.rout`、`2301113987-c-gbl`、`-gbs`、`-gtl`、`-gts`
9-12. `TestRenderGerberFile7..10` — `2301115633.rout`、`2301115633lg.dat`、`2301115633lg.ld12`、`2301115633lg.ld21`（**去掉原 Scale(5.0)**，按默认视图渲染，基线随之重生成）
13-16. `TestRenderGerberFile11..14` — `2301115633-lg-gbl`、`-gbs`、`-gtl`、`-gts`
17. `TestRenderGerberFile15` — `hj.324v1.gts`
18. `TestRenderGerberFile16` — `BOTTOM.art`
19. `TestRenderGerberFile17` — `P20230731.gtl`

**删除用例**：`TestScale`、`TestMove`（交互语义不在移植范围；对应 `_scale1_2.bmp`、`_move.bmp` 随全量 bmp 删除）。

**渲染与比对（锁定）:**
- 渲染：`BLImage(1600, 1600, BLFormat::PRGB32)` + `Blend2DEngine(image, gerber->GetBBox(), 0.005)` → `DrawBackground()` → `RenderGerber(gerber)`
- 基线加载：`BLImage::readFromFile(TestData + "results/<name>.png")`
- 比对：`CompareImages(rendered, baseline, threshold)`（tests/image_diff.h，T12 契约）
- **基线重生成开关**：env `GERBER_TEST_REGENERATE=1` 时，各用例改为 `writeToFile` 输出 PNG 并跳过比对（用于一次性生成基线，不提交为断言路径）
- **初始阈值（实测调定前）**：`mean_abs_diff ≤ 8.0` 且 `max_abs_diff ≤ 64` 且 `diff_ratio ≤ 0.12`
- **阈值调定流程（本任务内闭环）**：① 以 REGEN=1 生成 19 个 PNG → ② 人工审核基线正确性（对照 Gerber 语义：几何形状/层数/空白区合理，非"跑通即采信"）→ ③ 正常模式首跑打印每文件实际偏差分布 → ④ 依实测把阈值收紧/放宽到合理余量并在文件内注释记录最终阈值与依据 → ⑤ 删除全部旧 `.bmp` → commit

**验证命令（可复制粘贴）:**
- 步骤 ①: `cmake --build build -j$(nproc)` && `GERBER_TEST_REGENERATE=1 ctest --test-dir build --output-on-failure` → 预期：PASS 且 `tests/test_data/gerber/results/*.png` 生成
- 步骤 ③（观察分布）: `ctest --test-dir build --output-on-failure` → 预期：首次可能 FAIL，输出中打印每文件 mean/max/ratio（FAIL 原因=阈值未调定，属预期）
- 步骤 ⑤ 最终: `ctest --test-dir build --output-on-failure` → 预期：PASS（19 用例 + M1 单测全部绿）
- `ls tests/test_data/gerber/results/` → 预期：仅 19 个 `.png`（无 `.bmp`）

**How（executor 自由）:**
- 测试文件结构：去掉 QApplication/QImage/QPainterEngine include 与 fixture 类；保留 TestData 宏用法；REGEN 开关用 `std::getenv`；偏差分布打印用 `std::cerr`。

---

### Task T14: tests/CMakeLists.txt 移除 Qt

**What（计划锁定，不可改）:**

**Files:**
- Modify: `tests/CMakeLists.txt`

**变更点（锁定）:**
- 删除 `find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)`（:4）与 `set(CMAKE_AUTOMOC ON)`（:6）
- 保留 `find_package(GTest CONFIG REQUIRED)` / `find_package(Blend2D CONFIG REQUIRED)`（T2 已加）
- 链接：`gerber_engine` + `GTest::gtest` + `GTest::gtest_main` + `Blend2D::Blend2D`（显式，不依赖传递）

**验证命令（可复制粘贴）:**
- `grep -n 'Qt\|AUTOMOC' tests/CMakeLists.txt` → 预期：无命中
- `cmake --preset default` && `cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure` → 预期：全绿（tests 不再编译任何 Qt 头。注：gerber_engine 在 M4-T16 前仍链 Qt，属预期）

**How（executor 自由）:**
- 机械改配置。

---

### Task T15: gerber2image 改造为 Blend2D 版

**What（计划锁定，不可改）:**

**Files:**
- Modify: `example/gerber2image/main.cpp`
- Modify: `example/gerber2image/CMakeLists.txt`

**接口/行为（锁定）:**
- 头文件：去除 `QApplication/QFileDialog/QPixmap` 及 `engines/qpainter_engine.h`；改含 `<blend2d.h>` 与 `"engines/blend2d_engine.h"`。保留 gflags 双 flag（`--gerber_file`、`--um_pixel`，沿用现默认值；`um_pixel` 现实现未使用，保持不变）。
- 主流程：尺寸计算沿用现 `width_pixel=2560`、`height_pixel=int(bbox.Height()/bbox.Width()*2560)`、`image(w*1.05, h*1.05)`；`BLImage::create(w, h, BLFormat::PRGB32)` → `engine.DrawBackground()`（显式黑底，确保 PNG 输出确定；与旧实现"未定义/透明底"差异为确定性修复，PR 描述注明）→ `engine.RenderGerber(gerber)` → `image.writeToFile((FLAGS_gerber_file + ".png").c_str())`
- 返回码：渲染成功 0；`BLResult != BL_SUCCESS` 或解析异常 → `std::cerr` 输出并返回 1
- CMakeLists：删除 Qt find_package、`set(CMAKE_AUTORCC ON)`；新增 `find_package(gflags CONFIG REQUIRED)`、`find_package(Blend2D CONFIG REQUIRED)`；`add_executable` 只含 `*.cpp`（去掉 qrc/rc 编译引用，资源文件留在目录不编译）；链接 `gerber_engine gflags::gflags Blend2D::Blend2D`

**验证命令（可复制粘贴）:**
- `cmake --preset default` && `cmake --build build -j$(nproc)` → 预期：成功
- `./build/example/gerber2image/gerber2image --gerber_file=tests/test_data/gerber/gerber_files/2301113563-f-gtl` → 预期：退出码 0；产物 `tests/test_data/gerber/gerber_files/2301113563-f-gtl.png` 存在且 `stat -c%s` > 1000 字节
- 产物抽查（非金标）: 用 `tests/image_diff.h` 或 `BLImage::readFromFile` 读回 PNG，断言含非背景像素（有几何内容）

**How（executor 自由）:**
- PNG codec 用 Blend2D 内建；失败分支逐级返回。

---

### Task T16: 删除 Qt 引擎 + src/CMakeLists 去 Qt

**What（计划锁定，不可改）:**

**Files:**
- Delete: `src/engines/qpainter_engine.{h,cpp}`、`src/engines/qgraphics_scene_engine.{h,cpp}`、`src/engines/transformation.{h,cpp}`
- Modify: `src/CMakeLists.txt`

**变更点（锁定）:**
- 删除 `find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)`（:1）、`set(CMAKE_AUTOMOC ON)`（:2）
- `target_link_libraries(gerber_engine ...)`（:15）改为 `PUBLIC Blend2D::Blend2D gerber_parser`（去掉 Qt6::* 三个）
- engines 源文件由 `file(GLOB engines/*.cpp)` 自然收敛为 blend2d_engine.cpp

**验证命令（可复制粘贴）:**
- `grep -rn 'Qt6\|QPainter\|QGraphics\|transformation' src/CMakeLists.txt src/engines/` → 预期：无命中（`src/engines/` 下仅剩 blend2d_engine.*）
- `cmake --preset default` && `cmake --build build -j$(nproc)` && `ctest --test-dir build --output-on-failure` → 预期：全绿（需 T15 已先改 gerber2image、T13 已重写 tests，二者不再引用 QPainterEngine）

**How（executor 自由）:**
- 纯删除 + 配置修改；删除前确认全仓库仅 `docs/` 与迁移文档提及这些符号。

---

### Task T17: 删除查看器示例 + 顶层 CMake 清理（含 clang-tidy Qt）

**What（计划锁定，不可改）:**

**Files:**
- Delete: `example/gerber_viewer/`（整个目录）
- Delete: `example/gerber_viewer_qgraphics/`（整个目录）
- Modify: `CMakeLists.txt`（顶层）

**变更点（锁定）:**
- 删除 `add_subdirectory(example/gerber_viewer)`（:18）与 `add_subdirectory(example/gerber_viewer_qgraphics)`（:19）
- clang-tidy 目标：删除 `find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)`（:31）与 `-I "${Qt6Widgets_INCLUDE_DIRS}"`（:44）（tidy 头文件路径改走 vcpkg toolchain 提供的 include）

**验证命令（可复制粘贴）:**
- `grep -n 'Qt6\|gerber_viewer' CMakeLists.txt` → 预期：无命中
- `cmake --preset default` && `cmake --build build -j$(nproc)` → 预期：成功（examples 仅 gerber2image）
- `ls example/` → 预期：仅 `gerber2image/`

**How（executor 自由）:**
- 纯删除 + 配置修改。

---

### Task T18: README/README_zh Qt 内容清理

**What（计划锁定，不可改）:**

**Files:**
- Modify: `README.md`
- Modify: `README_zh.md`

**变更点（锁定，README.md 实际行号）:**
- :9 Qt 徽章 `[![Qt](...Qt-6.0+-green...)](https://www.qt.io/)` → 删除或换 Blend2D 徽章（换徽章须用 Blend2D 官方图床，无法核实可用则删除）
- :23 “Qt-based” → “Blend2D-based”（zh 对应）
- :67-69 目录树 engines 条目 → 改为 `blend2d_engine.cpp/h  # Blend2D rendering engine`（去掉 qpainter/qgraphics/transformation 三行）
- :83-85 `Rendering Engine Features` → 改为 Blend2D 引擎描述（删 QPainter/QGraphicsScene 两条，写 "Blend2D engine: headless rendering, PNG export"）
- :93 `- Qt 6.0+` → `- Blend2D (via vcpkg)`（zh 对应；可并列 vcpkg 说明）
- :243 Qt 致谢 → 删除或换 Blend2D（Zlib 许可）

**验证命令（可复制粘贴）:**
- `grep -in '\bqt\b' README.md README_zh.md` → 预期：无命中

**How（executor 自由）:**
- 只清 Qt；Python 相关已在 T11 清完；中英两版同步。

---

### Task T19: CI workflow 移除 Qt

**What（计划锁定，不可改）:**

**Files:**
- Modify: `.github/workflows/workflow.yml`

**变更点（锁定）:**
- 删除 `Install Qt` 步骤（jurplel/install-qt-action@v3，:20-26）
- checkout 的 `submodules: 'true'` → `submodules: 'false'`（子模块已全部移除）
- 保留 vcpkg 步骤与 `-DCMAKE_TOOLCHAIN_FILE=...` configure 参数（T3 所加，不再需要 install-qt）
- 构建参数保持 `-DBUILD_TESTS=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=ON`

**验证命令（可复制粘贴）:**
- `grep -n 'install-qt\|jurplel' .github/workflows/workflow.yml` → 预期：无命中
- `grep -n 'vcpkg' .github/workflows/workflow.yml` → 预期：≥2 处（setup + toolchain）
- YAML 可解析（同 T3）；最终以 PR Action build job 通过为准

**How（executor 自由）:**
- 机械改 YAML。

---

### Task T20: 全量验收（M4 检查点）

**What（计划锁定，不可改）:**

**Files:**
- 只读验收，无产物修改（如发现残留再修）

**验收清单（逐项命令锁定）:**
1. Qt 残留：`grep -rin '\bqt\b' --exclude-dir=.git --exclude-dir=docs .` → 预期：无命中（docs/ 含迁移文档与本文档，豁免）
2. `find_package(Qt6)` 残留：`grep -rn 'find_package(Qt6' --include='CMakeLists.txt' .` → 预期：无命中（`src/parser/**` 不在排查范围外，实际上层构建无引用）
3. Python 绑定残留：`grep -rin 'pybind11\|pygerber' --exclude-dir=.git --exclude-dir=docs .` → 预期：仅 `src/parser/`（vendored）命中；仓库根/CMake/README/tests 零命中
4. 配置：`cmake --preset default` → 预期：configure 成功且日志无 Qt6 请求
5. 构建：`cmake --build build -j$(nproc)` → 预期：成功
6. 测试：`ctest --test-dir build --output-on-failure` → 预期：全绿（19 金标 + M1 单测 + 无 TestScale/TestMove）
7. 子模块：`git submodule status` → 预期：空/无输出；`.gitmodules` 不存在
8. 目录终态：`ls src/engines/` → 仅 blend2d_engine.{h,cpp}；`ls example/` → 仅 gerber2image/；`ls 3rdparty/` → 空或不存在
9. 基线终态：`ls tests/test_data/gerber/results/` → 仅 19 个 `.png`，无 `.bmp`
10. 工具冒烟：`./build/example/gerber2image/gerber2image --gerber_file=tests/test_data/gerber/gerber_files/2301113563-f-gtl` → 退出码 0，PNG 生成

**How（executor 自由）:**
- 若 1/2/3 命中残留：最小修复（删行/改词）后重跑对应 grep 直至干净；修复不属于新功能。

---

## Phase 3: 自检记录

- **规范覆盖**：设计文档 5 个里程碑全部映射——M0=T1-T3；M1=T4-T9；M2=T10-T11；M3=T12-T14；M4=T15-T20。删除清单 6 项全覆盖（引擎 6 文件 T16、两个查看器 T17、gerber2image 改造 T15、Python 层 T10、tests 改造 T13、子模块/.gitmodules T2/T10/T20）。README/CI/CMake 清理 T11/T17/T18/T19。
- **占位符扫描**：WHAT 部分无 TBD/TODO/“适当错误处理”/“类似任务 N”；接口签名、测试规格（具体输入→具体输出数值）、验证命令（可复制粘贴含预期）全部锁定。HOW 各部分有意留白（实现本体交由 executor 在测试反馈下书写），非缺陷。
- **类型一致性**：`Blend2DEngine` 方法名/签名与 `engine.h` 纯虚接口逐一对应；`blend2d_semantics` 六个纯函数在 T5 定义、T6-T9 引用，命名一致；`ImageDiffStats/CompareImages/PixelAt` 在 T12 定义、T13 使用，签名一致。
- **自包含**：每任务含完整 Fixture/文件/命令，实现者可独立执行。
- **可并行性判定**：按真实文件集——M2(F)∥M3(G) 成立（F 触 src/CMakeLists+顶层+README+删除文件，G 触 tests/*）；M1 与 M2 均触 `src/CMakeLists.txt` → 串行；M4-I 四任务文件集两两不相交 → 可并行。标注与设计文档依赖关系一致。
- **测试粒度**：按设计文档授权——Engine 映射以 BLPath 结构断言为主（T4/T6），语义逻辑以纯函数输入→输出单测为主（T5/T9），像素金标降级为 M3 过渡验收（T13，基线重生成 + 容差比对 + 阈值实测调定 + 人工审核）。

---

## 测试与验证命令总览

**通用构建/测试（每批后执行）**
```bash
export VCPKG_ROOT=<vcpkg 安装目录>
cmake --preset default
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

**关键验收命令**
```bash
# 无 Qt（docs/ 豁免）
grep -rin '\bqt\b' --exclude-dir=.git --exclude-dir=docs .
grep -rn 'find_package(Qt6' --include='CMakeLists.txt' .
# 无 Python 绑定（vendored src/parser/ 豁免）
grep -rin 'pybind11\|pygerber' --exclude-dir=.git --exclude-dir=docs .
# 子模块清空
git submodule status
# 基线终态
ls tests/test_data/gerber/results/
```

**每任务专属命令** 见各任务「验证命令」节（均为可复制粘贴的单行/多行命令，含预期输出）。

---

## 输出汇总

- 计划路径: `docs/plans/2026-09-14-blend2d-migration.md`
- 任务数量: 20（T1-T20）
- 批次: A:T1[契约批] / B:T2+T3[机械批,可并行] / C:T4+T5[契约批,串行] / D:T6+T7[机械批,串行] / E:T8+T9[机械批,串行,检查点T9] / F:T10+T11[机械批,可并行] / G:T12+T13+T14[契约批T12→机械,串行] / H:T15[机械批] / I:T16+T17+T18+T19[机械批,可并行] / J:T20[机械批,检查点T20]
- 关键依赖: A→B→C→D→E→{F∥G}→H→I→J；F∥G 可并行；M1 与 M2 因 src/CMakeLists.txt 串行