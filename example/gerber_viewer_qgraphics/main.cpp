#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsView>

#include "engines/qgraphics_scene_engine.h"
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
            engine_ = std::make_unique<QGraphicsSceneEngine>(gerber_->GetBBox(), 0.025);
            engine_->RenderGerber(gerber_);

            setScene(engine_->scene());
        }
        catch (const std::exception &e)
        {
            gerber_ = std::make_shared<Gerber>();
            engine_ = std::make_unique<QGraphicsSceneEngine>(BoundBox(0, 0, 1.0, 1.0), 0.025);
            QMessageBox::warning(this, "Message", e.what(), QMessageBox::Ok);
        }
    }

protected:
    void wheelEvent(QWheelEvent* event) override {
        const auto delta = event->angleDelta();
        scale(1 + delta.y() / 1000.0, 1 + delta.y() / 1000.0);
    }

private:
    std::unique_ptr<QGraphicsSceneEngine> engine_;
    std::shared_ptr<Gerber> gerber_;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GerberWidget wnd;
    wnd.showMaximized();
    wnd.show();

    const auto s = std::min(wnd.width() / wnd.sceneRect().width(), wnd.height() / wnd.sceneRect().height());
    wnd.scale(s, s);

    return app.exec();
}
