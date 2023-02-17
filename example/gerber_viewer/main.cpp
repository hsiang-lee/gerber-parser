#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsView>

#include "engines/qpainter_engine.h"
#include "gerber_parser/gerber_parser.h"
#include "gerber/gerber.h"

class GerberWidget : public QGraphicsView
{
public:
    GerberWidget()
    {
        auto file = QFileDialog::getOpenFileName(this);
        auto parser = std::make_unique<GerberParser>(file.toLocal8Bit().toStdString());

        try
        {
            gerber_ = parser->GetGerber();
            engine_ = std::make_unique<QPainterEngine>(this, gerber_->GetBBox(), BoundBox(0.025, 0.025, 0.025, 0.025));
            engine_->RenderGerber(gerber_);
        }
        catch (const std::exception &e)
        {
            gerber_ = std::make_shared<Gerber>();
            engine_ = std::make_unique<QPainterEngine>(this, BoundBox(0, 0, 1.0, 1.0), BoundBox(0.025, 0.025, 0.025, 0.025));
            QMessageBox::warning(this, "Message", e.what(), QMessageBox::Ok);
        }
    }

private:
    std::unique_ptr<QPainterEngine> engine_;
    std::shared_ptr<Gerber> gerber_;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GerberWidget wnd;
    wnd.showMaximized();
    wnd.show();

    auto s = std::min(wnd.width() / wnd.sceneRect().width(), wnd.height() / wnd.sceneRect().height());
    wnd.scale(s, s);

    return app.exec();
}
