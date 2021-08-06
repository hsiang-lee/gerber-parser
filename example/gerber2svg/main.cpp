#include <QApplication>
#include <QFileDialog>
#include <QIcon>
#include <QBitmap>
#include <QSvgGenerator>

#include "gerber_render.h"
#include "engine/qt_engine.h"

#include <gflags/gflags.h>

DEFINE_string(gerber_file, "", "The path of gerber file you want to export.");

int main(int argc, char* argv[]) {
	gflags::SetUsageMessage("Usage: gerber2svg --gerber_file=\"path/to/gerber/file\"");
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	QApplication app(argc, argv);

	auto gerber = std::make_shared<Gerber>(FLAGS_gerber_file);

	auto svg = std::make_unique<QSvgGenerator>();
	svg->setFileName(QString(FLAGS_gerber_file.c_str()) + ".svg");
	svg->setSize(QSize(500, 500));
	svg->setViewBox(QRect(0, 0, 500, 500));

	auto engine = std::make_unique<QtEngine>(svg.get());
	GerberRender render(engine.get());
	render.RenderGerber(gerber);

	return 0;
}
