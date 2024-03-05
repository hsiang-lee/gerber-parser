#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <QApplication>
#include <QPixmap>

#include <iostream>

#include "gerber_parser/bound_box.h"
#include "gerber/gerber.h"
#include "gerber_parser/gerber_parser.h"
#include "engines/qpainter_engine.h"


std::vector<std::uint8_t> RenderGerber2Image(const std::string& gerber_file, int width, int height)
{
    int argc = 1;
    char arg1[] = "pygerber-parser";
    char* argv[] = {arg1};
	QApplication app(argc, argv);

	if (gerber_file.empty()) {
          std::cout << "null gerber file path." << std::endl;
          return {};
	}

	try {
          auto parser = std::make_shared<GerberParser>(gerber_file);
          auto gerber = parser->GetGerber();
          auto image = std::make_unique<QPixmap>(width, height);
          auto engine = std::make_unique<QPainterEngine>(
              image.get(), gerber->GetBBox(), 0.00);
          engine->RenderGerber(gerber);

          auto img = image->toImage();
          return std::vector<std::uint8_t>(img.bits(), img.bits() + img.sizeInBytes());
    }
    catch (const std::exception& e) {
          std::cout << e.what() << std::endl;
          return {};
    }
}

PYBIND11_MODULE(pygerber_parser, m) {
    m.def("gerber2image", &RenderGerber2Image, "render gerber file to image data");
}