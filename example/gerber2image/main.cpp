#include <QApplication>
#include <QFileDialog>
#include <QIcon>
#include <QBitmap>

#include "gerber_render.h"
#include "engine/qt_engine.h"

#include <gflags/gflags.h>
#include "main.h"

DEFINE_string(gerber_files, "", "The path of gerber files you want to export.If there are more than one file, separate them with ','.");
DEFINE_string(output_path, "", "Output path of rendered image files");
DEFINE_double(um_pixel, 5, "How much um/pixel.Default value is 5um/pixel");

int main(int argc, char* argv[]) {
	gflags::SetUsageMessage("Usage: gerber2image --gerber_files=\"path/to/gerber/file1, path/to/gerber/file2...\" --output_path=\"path/\" --um_pixel=5.\
 Tip: The separator of file path MUST be slash rather than backslash.");
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	QApplication app(argc, argv);

	QDir dir(FLAGS_output_path.c_str());
	if (!dir.exists()) {
		dir.mkpath(FLAGS_output_path.c_str());
	}

	QString files(FLAGS_gerber_files.c_str());
	auto file_list = files.split(',', Qt::SkipEmptyParts);

	std::vector<std::shared_ptr<Gerber>> gerbers;
	for (const auto file : file_list) {
		gerbers.push_back(std::make_shared<Gerber>(file.toLocal8Bit().toStdString()));
	}

	BoundBox box;
	for (const auto gerber : gerbers) {
		box.UpdateBox(gerber->GetBBox());
	}

	for (const auto gerber : gerbers) {
		auto width = box.Right() - box.Left();
		auto height = box.Top() - box.Bottom();
		auto unit = gerber->Unit();
		if (unit == GERBER_UNIT::guInches) {
			width *= 25.4;
			height *= 25.4;
		}

		const auto pixel_w = width * 1000 / FLAGS_um_pixel;
		const auto pixel_h = height * 1000 / FLAGS_um_pixel;

		int img_w = 0;
		int img_h = 0;
		if (pixel_w > pixel_h) {
			img_h = 20000 * pixel_h / pixel_w;
			img_w = 20000;
		}
		else {
			img_w = 20000 * pixel_w / pixel_h;
			img_h = 20000;
		}

		ExportGerber(gerber, box, img_w, img_h, pixel_w, pixel_h);
	}

	return 0;
}

void ExportGerber(std::shared_ptr<Gerber> gerber, const BoundBox& box, int img_w, int img_h, int pixel_w, int pixel_h)
{
	auto image = std::make_unique<QBitmap>(img_w, img_h);
	auto engine = std::make_unique<QtEngine>(image.get(), box, BoundBox(0.0, 0.0, 0.0, 0.0));
	GerberRender render(engine.get());

	int height_scale = (pixel_h - 1) / img_h + 1;
	int width_scale = (pixel_w - 1) / img_w + 1;

	engine->Scale(std::max(width_scale, height_scale) - 1);

	for (int i = 0; i < height_scale; ++i) {
		for (int j = 0; j < width_scale; ++j) {
			render.RenderGerber(gerber);

			auto file_name = QString(gerber->FileName().c_str()).split('/').last();
			auto image_file = QString(FLAGS_output_path.c_str()) + file_name + '_' + QString("%1").arg(i) + '_' + QString("%1").arg(j) + ".bmp";
			image->save(image_file);

			engine->Move(-img_w, 0);
		}

		engine->Move(img_w * width_scale, -img_h);
	}
}
