#include <QApplication>
#include <QFileDialog>
#include <QIcon>
#include <QBitmap>

#include "gerber_render.h"
#include "engine/qt_engine.h"

#include <gflags/gflags.h>

DEFINE_string(gerber_file, "", "The path of gerber file you want to export.");
DEFINE_double(um_pixel, 5, "How much um/pixel.Default value is 5um/pixel");

int main(int argc, char* argv[]) {
	gflags::SetUsageMessage("Usage: gerber2image --gerber_file=\"path/to/gerber/file\" --um_pixel=5");
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	QApplication app(argc, argv);

	auto gerber = std::make_shared<Gerber>(FLAGS_gerber_file);

	auto width = gerber->GetBBox().Right() - gerber->GetBBox().Left();
	auto height = gerber->GetBBox().Top() - gerber->GetBBox().Bottom();
	auto unit = gerber->Unit();
	if (unit == GERBER_UNIT::guInches) {
		width *= 25.4;
		height *= 25.4;
	}

	const auto width_pixel = width * 1000 / FLAGS_um_pixel;
	const auto height_pixel = height * 1000 / FLAGS_um_pixel;

	int width_img = 0;
	int height_img = 0;
	if (width_pixel > height_pixel) {
		height_img = 20000 * height_pixel / width_pixel;
		width_img = 20000;
	}
	else {
		width_img = 20000 * width_pixel / height_pixel;
		height_img = 20000;
	}

	auto image = std::make_unique<QBitmap>(width_img * 1.05, height_img * 1.05);
	auto engine = std::make_unique<QtEngine>(image.get());
	GerberRender render(engine.get());

	int height_scale = (height_pixel - 1) / height_img + 1;
	int width_scale = (width_pixel - 1) / width_img + 1;

	engine->Scale(std::max(width_scale, height_scale) - 1);

	for (int i = 0; i < height_scale; ++i) {
		for (int j = 0; j < width_scale; ++j) {
			render.RenderGerber(gerber);

			auto image_file = QString(FLAGS_gerber_file.c_str()) + '_' + QString("%1").arg(i) + '_' + QString("%1").arg(j) + ".bmp";
			image->save(image_file);

			engine->Move(-width_img, 0);
		}

		engine->Move(width_img * width_scale, -height_img);
	}

	return 0;
}
