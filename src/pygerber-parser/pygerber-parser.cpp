#include <QApplication>
#include <QPixmap>

#include <iostream>

#include "gerber_parser/bound_box.h"
#include "gerber/gerber.h"
#include "gerber_parser/gerber_parser.h"
#include "engines/qpainter_engine.h"

extern "C" {

typedef struct ImageData_t {
    uchar* data;
    int size;
}ImageData;

ImageData RenderGerber2Image(char* gerber_file, int width, int height)
{
    int argc = 1;
    char arg1[] = "pygerber-parser";
    char* argv[] = {arg1};
	QApplication app(argc, argv);

    std::string file_path = std::string(gerber_file);
	if (file_path.empty()) {
          std::cout << "null gerber file path." << std::endl;
          return {};
	}

	try {
          auto parser = std::make_shared<GerberParser>(file_path);
          auto gerber = parser->GetGerber();
          auto image = std::make_unique<QPixmap>(width, height);
          auto engine = std::make_unique<QPainterEngine>(
              image.get(), gerber->GetBBox(), 0.00);
          engine->RenderGerber(gerber);

          auto img = image->toImage();
          ImageData result;
          result.size = img.sizeInBytes();
          auto* data = (uchar*)malloc(result.size);
          memcpy(data, img.bits(), result.size);
          result.data = data;

          return result;
    }
    catch (const std::exception& e) {
          std::cout << e.what() << std::endl;
          return {};
    }
}

}
