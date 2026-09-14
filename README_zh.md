# Gerber Parser - 开源Gerber文件解析与渲染库

**[English Version](README.md)**

<div align="center">

![项目Logo](img/logo.png)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey.svg)](https://github.com/hsiang-lee/gerber-parser)

**高性能的C++ Gerber文件解析和渲染库，采用解析与渲染分离的架构设计**

</div>

## 🎯 项目特色

- **解析与渲染分离**：核心解析器与渲染引擎完全解耦，便于扩展和定制
- **Blend2D渲染引擎**：提供高性能2D渲染后端，支持无头图像导出
- **高性能**：优化的解析算法和内存管理
- **跨平台**：基于Blend2D框架，原生支持Linux。需要Windows/macOS？[联系我们](mailto:leehsiang@hotmail.com)获取有偿跨平台支持。

## 💝 支持项目

<div align="center">

| | |
|---|---|
| 🚀 | **这个项目为您节省了数周的Gerber解析开发时间。** |
| 💰 | 您的捐赠将直接用于功能开发和项目维护。 |

<br>

![捐赠二维码](img/donate.jpg)

<br>

**⭐ 给项目Star** · **🐛 提交Issue** · **📖 完善文档** · **🔄 分享给他人**

</div>

## 📸 渲染示例

<div align="center">

### Gerber文件渲染效果

![Gerber渲染示例](img/gerber.png)
*Gerber文件解析和渲染效果展示*

</div>

## 🏗️ 项目架构

### 核心模块

```
src/
├── parser/           # Gerber文件解析器
│   ├── gerber_parser/ # 解析器核心实现
│   ├── engine/       # 解析引擎接口
│   └── parser/       # 各种Gerber代码解析器
└── engines/          # 渲染引擎
    └── blend2d_engine.cpp/h     # Blend2D渲染引擎
```

### 解析器特性

- 支持完整的Gerber文件格式（RS-274X）
- 解析各种孔径类型：圆形、矩形、多边形、椭圆形、宏定义
- 支持G代码、D代码、M代码等Gerber指令
- 提供边界框计算和坐标变换
- 错误处理和日志记录

### 渲染引擎特性

- **Blend2D引擎**：无头渲染，内置编解码器导出PNG
- 可扩展的渲染接口，便于添加新的渲染后端

## 🚀 快速开始

### 系统要求

- CMake 3.20+
- C++17兼容编译器（GCC 7+, Clang 5+, MSVC 2019+）
- Blend2D（通过vcpkg安装，详见vcpkg.json）

### 构建项目

```bash
# 克隆项目
git clone https://github.com/hsiang-lee/gerber-parser.git
cd gerber-parser

# 初始化子模块
git submodule update --init --recursive

# 创建构建目录
mkdir build && cd build

# 配置项目
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
make -j$(nproc)
```

### 运行示例

项目提供了一个示例程序：

#### 1. Gerber转图像工具

```bash
# 将Gerber文件转换为PNG图像
./example/gerber2image/gerber2image --gerber_file="path/to/gerber/file" --um_pixel=5
```

## 📖 API使用示例

### C++ API

```cpp
#include <blend2d.h>
#include "gerber_parser/gerber_parser.h"
#include "engines/blend2d_engine.h"

// 解析Gerber文件
auto parser = std::make_shared<GerberParser>("path/to/gerber/file");
auto gerber = parser->GetGerber();

// 获取边界框信息
const auto& bbox = gerber->GetBBox();
std::cout << "Width: " << bbox.Width() << " Height: " << bbox.Height() << std::endl;

// 使用Blend2D渲染并导出PNG
BLImage image(800, 600, BL_FORMAT_PRGB32);
Blend2DEngine engine(image, bbox, 0.05);
engine.DrawBackground();
const int ret = engine.RenderGerber(gerber);
if (ret != 0) return ret;
image.writeToFile("output.png");
```

## 🔧 开发指南

### 添加新的渲染引擎

1. 继承`RenderEngine`基类
2. 实现`RenderGerber`方法
3. 在`engines/`目录中添加新的引擎文件
4. 更新CMakeLists.txt文件

### 扩展解析器功能

1. 在`src/parser/gerber_parser/`目录中添加新的解析器
2. 实现相应的解析逻辑
3. 更新解析器工厂类

## 🧪 测试

项目包含完整的测试套件：

```bash
# 启用测试构建
cmake .. -DBUILD_TESTS=ON

# 运行测试
make test
```

测试数据位于`tests/test_data/gerber/`目录中。

## 🤝 贡献指南

我们欢迎各种形式的贡献！请参考以下步骤：

1. Fork本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建Pull Request

### 代码规范

- 遵循项目中的.clang-format配置
- 使用有意义的变量和函数名
- 添加适当的注释和文档
- 确保所有测试通过

## 📄 许可证

本项目采用MIT许可证 - 详见[LICENSE](LICENSE)文件。

## 🙏 致谢

感谢以下开源项目的支持：

- [Blend2D](https://blend2d.com/) - 高性能2D渲染库（Zlib许可）
- [Google Test](https://github.com/google/googletest) - C++测试框架
- [gflags](https://github.com/gflags/gflags) - 命令行参数解析

## 📞 联系方式

- 项目主页：https://github.com/hsiang-lee/gerber-parser.git
- Issues：https://github.com/hsiang-lee/gerber-parser/issues
- 邮箱：leehsiang@hotmail.com

---

<div align="center">

**Gerber Parser** - 让PCB文件处理变得更简单！

</div>