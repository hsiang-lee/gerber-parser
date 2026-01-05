# Gerber Parser - Open Source Gerber File Parsing and Rendering Library

<div align="center">

![Project Logo](img/logo.png)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Qt](https://img.shields.io/badge/Qt-6.0+-green.svg)](https://www.qt.io/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](https://github.com/your-repo/gerber-parser)

**High-performance C++ Gerber file parsing and rendering library with separation architecture design between parsing and rendering**

</div>

**[中文版本 (Chinese Version)](README_zh.md)**

## 🎯 Project Features

- **Separation of parsing and rendering**: Core parser is completely decoupled from rendering engine for easy extension and customization
- **Multiple rendering engine support**: Provides QPainter and QGraphicsScene rendering backends
- **High performance**: Optimized parsing algorithms and memory management
- **Cross-platform**: Based on Qt framework, supports Windows, Linux, macOS
- **Python bindings**: Complete Python interface through pybind11

## 💝 Support the Project

If this project has been helpful to you, please consider supporting our development work. Your support will help us continuously improve the project:

[![Donate with PayPal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/ncp/payment/3ZRR9DFDYY3R8)

**Click the "Donate" button above** or support in other ways:
- ⭐ **Star this project** - Let more people see it
- 🐛 **Submit Issues** - Report problems or suggest features
- 📖 **Improve documentation** - Help enhance user guides
- 🔄 **Share with other developers** - Spread the open source spirit

## 📸 Rendering Examples

<div align="center">

### Gerber File Rendering Effect

![Gerber Rendering Example](img/gerber.png)
*Gerber file parsing and rendering effect demonstration*

</div>

## 🏗️ Project Architecture

### Core Modules

```
src/
├── parser/           # Gerber file parser
│   ├── gerber_parser/ # Parser core implementation
│   ├── engine/       # Parser engine interface
│   └── parser/       # Various Gerber code parsers
├── engines/          # Rendering engines
│   ├── qpainter_engine.cpp/h    # QPainter rendering engine
│   ├── qgraphics_scene_engine.cpp/h # QGraphicsScene rendering engine
│   └── transformation.cpp/h     # Coordinate transformation tools
└── pygerber-parser/  # Python bindings
```

### Parser Features

- Supports complete Gerber file format (RS-274X)
- Parses various aperture types: circular, rectangular, polygonal, elliptical, macro-defined
- Supports G codes, D codes, M codes, and other Gerber commands
- Provides bounding box calculation and coordinate transformation
- Error handling and logging

### Rendering Engine Features

- **QPainter engine**: Lightweight, suitable for image export and simple display
- **QGraphicsScene engine**: Feature-rich, supports interactive viewing and editing
- Extensible rendering interface, easy to add new rendering backends

## 🚀 Quick Start

### System Requirements

- CMake 3.20+
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2019+)
- Qt 6.0+
- Python 3.6+ (optional, for Python bindings)

### Build the Project

```bash
# Clone the project
git clone https://github.com/hsiang-lee/gerber-parser.git
cd gerber-parser

# Initialize submodules
git submodule update --init --recursive

# Create build directory
mkdir build && cd build

# Configure the project
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compile
make -j$(nproc)
```

### Run Examples

The project provides several example programs:

#### 1. Gerber to Image Tool

```bash
# Convert Gerber file to PNG image
./example/gerber2image/gerber2image --gerber_file="path/to/gerber/file" --um_pixel=5
```

#### 2. Gerber Viewer

```bash
# Launch interactive Gerber file viewer
./example/gerber_viewer/gerber_viewer
```

#### 3. QGraphicsScene Viewer

```bash
# Viewer using QGraphicsScene
./example/gerber_viewer_qgraphics/gerber_viewer_qgraphics
```

## 📖 API Usage Examples

### C++ API

```cpp
#include "gerber_parser/gerber_parser.h"
#include "engines/qpainter_engine.h"

// Parse Gerber file
auto parser = std::make_shared<GerberParser>("path/to/gerber/file");
auto gerber = parser->GetGerber();

// Get bounding box information
const auto& bbox = gerber->GetBBox();
std::cout << "Width: " << bbox.Width() << " Height: " << bbox.Height() << std::endl;

// Render using QPainter
QPixmap image(800, 600);
auto engine = std::make_unique<QPainterEngine>(&image, bbox, 0.05);
engine->RenderGerber(gerber);
image.save("output.png");
```

### Python API

```python
import pygerber_parser
import numpy as np
from PIL import Image

# Method 1: Using functional API
image_data = pygerber_parser.gerber2image("path/to/gerber/file", 800, 600)

# Convert to PIL image
arr = np.array(image_data, dtype=np.uint8).reshape((600, 800, 4))
img = Image.fromarray(arr, 'RGBA')
img.save("output.png")

# Method 2: Using object-oriented API
parser = pygerber_parser.GerberParser("path/to/gerber/file")
if parser.is_valid():
    print(f"Size: {parser.get_width()} x {parser.get_height()}")
    image_data = parser.render_to_image(800, 600)
    # Process image data...
```

For detailed Python usage guide, please refer to [PYTHON_USAGE.md](PYTHON_USAGE.md)

## 🔧 Development Guide

### Add a New Rendering Engine

1. Inherit from the `RenderEngine` base class
2. Implement the `RenderGerber` method
3. Add new engine files in the `engines/` directory
4. Update the CMakeLists.txt file

### Extend Parser Functionality

1. Add new parsers in the `src/parser/gerber_parser/` directory
2. Implement the corresponding parsing logic
3. Update the parser factory class

## 🧪 Testing

The project includes a complete test suite:

```bash
# Enable test building
cmake .. -DBUILD_TESTS=ON

# Run tests
make test
```

Test data is located in the `tests/test_data/gerber/` directory.

## 🤝 Contribution Guide

We welcome all forms of contributions! Please refer to the following steps:

1. Fork this project
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Create a Pull Request

### Code Standards

- Follow the .clang-format configuration in the project
- Use meaningful variable and function names
- Add appropriate comments and documentation
- Ensure all tests pass

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

Thanks to the following open source projects for their support:

- [Qt](https://www.qt.io/) - Cross-platform application framework
- [pybind11](https://github.com/pybind/pybind11) - Python binding generator
- [Google Test](https://github.com/google/googletest) - C++ testing framework
- [gflags](https://github.com/gflags/gflags) - Command line argument parsing

## 📞 Contact Information

- Project homepage: https://github.com/hsiang-lee/gerber-parser.git
- Issues: https://github.com/hsiang-lee/gerber-parser/issues
- Email: leehsiang@hotmail.com

---

<div align="center">

**Gerber Parser** - Making PCB file processing easier!

</div>