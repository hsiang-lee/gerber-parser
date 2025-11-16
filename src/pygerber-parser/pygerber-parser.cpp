#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/functional.h"

#include <QApplication>
#include <QPixmap>
#include <memory>

#include "gerber_parser/bound_box.h"
#include "gerber/gerber.h"
#include "gerber_parser/gerber_parser.h"
#include "engines/qpainter_engine.h"

namespace py = pybind11;

// 全局QApplication实例
static std::unique_ptr<QApplication> g_app = nullptr;

void initialize_qt() {
    static bool initialized = false;
    if (!initialized) {
        int argc = 1;
        char arg1[] = "pygerber-parser";
        char* argv[] = {arg1, nullptr};
        g_app = std::make_unique<QApplication>(argc, argv);
        initialized = true;
    }
}

std::vector<std::uint8_t> RenderGerber2Image(const std::string& gerber_file, int width, int height) {
    initialize_qt();
    
    if (gerber_file.empty()) {
        throw std::runtime_error("null gerber file path.");
    }

    try {
        auto parser = std::make_shared<GerberParser>(gerber_file);
        auto gerber = parser->GetGerber();
        
        // 计算合适的图像尺寸
        const auto& bbox = gerber->GetBBox();
        double aspect_ratio = bbox.Height() / bbox.Width();
        
        if (width <= 0 || height <= 0) {
            width = 800;
            height = static_cast<int>(width * aspect_ratio);
        }
        
        auto image = std::make_unique<QPixmap>(width, height);
        auto engine = std::make_unique<QPainterEngine>(
            image.get(), bbox, 0.05);
        engine->RenderGerber(gerber);

        // 转换为RGBA格式
        auto img = image->toImage().convertToFormat(QImage::Format_RGBA8888);
        
        // 返回图像数据
        const uchar* bits = img.bits();
        return std::vector<std::uint8_t>(bits, bits + img.sizeInBytes());
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to render Gerber file: ") + e.what());
    }
}

// Gerber文件信息结构
struct GerberInfo {
    std::string filename;
    double width;
    double height;
    int layer_count;
    bool is_valid;
    std::string error_message;
};

GerberInfo GetGerberInfo(const std::string& gerber_file) {
    initialize_qt();
    
    GerberInfo info;
    info.filename = gerber_file;
    info.is_valid = false;
    
    try {
        auto parser = std::make_shared<GerberParser>(gerber_file);
        auto gerber = parser->GetGerber();
        
        const auto& bbox = gerber->GetBBox();
        info.width = bbox.Width();
        info.height = bbox.Height();
        info.layer_count = 1; // 简化处理，实际需要从gerber对象获取
        info.is_valid = true;
        info.error_message = "";
    }
    catch (const std::exception& e) {
        info.error_message = e.what();
    }
    
    return info;
}

// Python绑定的GerberParser类包装
class PyGerberParser {
public:
    PyGerberParser(const std::string& filename) 
        : parser_(std::make_shared<GerberParser>(filename)) {
        initialize_qt();
    }
    
    bool is_valid() const {
        try {
            auto gerber = parser_->GetGerber();
            return true;
        }
        catch (...) {
            return false;
        }
    }
    
    std::vector<double> get_bounds() const {
        auto gerber = parser_->GetGerber();
        const auto& bbox = gerber->GetBBox();
        return {bbox.Left(), bbox.Top(), bbox.Right(), bbox.Bottom()};
    }
    
    double get_width() const {
        auto gerber = parser_->GetGerber();
        return gerber->GetBBox().Width();
    }
    
    double get_height() const {
        auto gerber = parser_->GetGerber();
        return gerber->GetBBox().Height();
    }
    
    std::vector<std::uint8_t> render_to_image(int width = 800, int height = 600) {
        return RenderGerber2Image(parser_->GetFileName(), width, height);
    }
    
private:
    std::shared_ptr<GerberParser> parser_;
};

PYBIND11_MODULE(pygerber_parser, m) {
    m.doc() = "Python bindings for Gerber Parser";
    
    // 基本函数绑定
    m.def("gerber2image", &RenderGerber2Image, 
          "Render Gerber file to image data",
          py::arg("gerber_file"), 
          py::arg("width") = 800, 
          py::arg("height") = 600);
    
    m.def("get_gerber_info", &GetGerberInfo,
          "Get basic information about Gerber file",
          py::arg("gerber_file"));
    
    // GerberInfo结构绑定
    py::class_<GerberInfo>(m, "GerberInfo")
        .def_readonly("filename", &GerberInfo::filename)
        .def_readonly("width", &GerberInfo::width)
        .def_readonly("height", &GerberInfo::height)
        .def_readonly("layer_count", &GerberInfo::layer_count)
        .def_readonly("is_valid", &GerberInfo::is_valid)
        .def_readonly("error_message", &GerberInfo::error_message)
        .def("__repr__", [](const GerberInfo& info) {
            return "GerberInfo(filename='" + info.filename + 
                   "', width=" + std::to_string(info.width) + 
                   ", height=" + std::to_string(info.height) + 
                   ", layer_count=" + std::to_string(info.layer_count) + 
                   ", is_valid=" + (info.is_valid ? "True" : "False") + ")";
        });
    
    // PyGerberParser类绑定
    py::class_<PyGerberParser>(m, "GerberParser")
        .def(py::init<const std::string&>(), py::arg("filename"))
        .def("is_valid", &PyGerberParser::is_valid)
        .def("get_bounds", &PyGerberParser::get_bounds)
        .def("get_width", &PyGerberParser::get_width)
        .def("get_height", &PyGerberParser::get_height)
        .def("render_to_image", &PyGerberParser::render_to_image,
             py::arg("width") = 800, py::arg("height") = 600)
        .def("__repr__", [](const PyGerberParser& parser) {
            return "GerberParser(filename='" + 
                   (parser.is_valid() ? "valid" : "invalid") + "')";
        });
}