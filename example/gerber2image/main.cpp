#include <QApplication>
#include <QFileDialog>
#include <QPixmap>

#include <iostream>

#include "gerber_parser/bound_box.h"
#include "gerber/gerber.h"
#include "gerber_parser/gerber_parser.h"
#include "engines/qpainter_engine.h"

#include <gflags/gflags.h>

DEFINE_string(gerber_file, "", "The path of gerber file you want to export.");
DEFINE_double(um_pixel, 5, "How much um/pixel.Default value is 5um/pixel");

int main(int argc, char *argv[])
{
	gflags::SetUsageMessage("Usage: gerber2image --gerber_file=\"path/to/gerber/file\" --um_pixel=5");
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	QApplication app(argc, argv);

	if (FLAGS_gerber_file.empty()) {
          std::cout << "Usage: gerber2image --gerber_file=\"path/to/gerber/file\" --um_pixel=5" << std::endl;
          return 0;
	}

	try {
          auto parser = std::make_shared<GerberParser>(FLAGS_gerber_file);
          auto gerber = parser->GetGerber();
          const auto width_pixel = 2560;
          const auto height_pixel = int(double(gerber->GetBBox().Height()) /
                                        gerber->GetBBox().Width() * 2560);

          auto image = std::make_unique<QPixmap>(width_pixel * 1.05,
                                                 height_pixel * 1.05);
          auto engine = std::make_unique<QPainterEngine>(
              image.get(), gerber->GetBBox(), BoundBox(0.05, 0.05, 0.05, 0.05));
          engine->RenderGerber(gerber);
          auto image_file = QString(FLAGS_gerber_file.c_str()) + ".png";
          image->save(image_file);
    }
    catch (const std::exception& e) {
          std::cout << e.what() << std::endl;
    }

	return 0;
}
