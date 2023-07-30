#include <QApplication>
#include <QFileDialog>

#include "engines/qpainter_engine.h"
#include "gerber_widget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GerberWidget wnd(nullptr);
    wnd.showMaximized();
    wnd.show();

    std::vector<QString> files;
    auto file = QFileDialog::getOpenFileName(nullptr);
    files.emplace_back(file);
    wnd.Refresh(files);

    return app.exec();
}
