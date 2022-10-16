#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>

#include <exception>

#include "gerber_parser/gerber_file.h"
#include "gerber_parser/gerber.h"
#include "gerber_parser/gerber_parser.h"
#include "engine/qt_engine.h"

class GerberWidget : public QWidget {
public:
	GerberWidget() {
		auto file = QFileDialog::getOpenFileName(this);
		auto parser = std::make_unique<GerberParser>(file.toLocal8Bit().toStdString());;
		
		try {
			gerber_ = parser->GetGerber();
            engine_ = std::make_unique<QtEngine>(this, gerber_->GetBBox(), BoundBox(0.025, 0.025, 0.025, 0.025));
		}
		catch (const std::exception& e) {
            gerber_ = std::make_shared<Gerber>();
            engine_ = std::make_unique<QtEngine>(this, BoundBox(0, 0, 1.0, 1.0), BoundBox(0.025, 0.025, 0.025, 0.025));
			QMessageBox::warning(this, "Message", e.what(), QMessageBox::Ok);
		}
	}

protected:
	void paintEvent(QPaintEvent* e) override {
		engine_->RenderGerber(gerber_);
	}

	void wheelEvent(QWheelEvent* e) override {
		engine_->Scale(e->delta() / 720.0, double(e->x()) / width(), double(e->y()) / height());
		QWidget::wheelEvent(e);

		update();
	}

	//void mousePressEvent(QMouseEvent* e) override {
	//	pressed_ = true;
	//	start_x_ = e->x();
	//	start_y_ = e->y();
	//}

	//void mouseReleaseEvent(QMouseEvent* e) override {
	//	pressed_ = false;

	//	engine_->Select(e->x(), e->y());
	//	update();
	//}

	//void mouseMoveEvent(QMouseEvent* e) override {
	//	if (pressed_) {
	//		engine_->Move(e->x() - start_x_, e->y() - start_y_);

	//		start_x_ = e->x();
	//		start_y_ = e->y();

	//		update();
	//	}
	//}

	void resizeEvent(QResizeEvent* event) override {
		engine_->Resize();
		QWidget::resizeEvent(event);
	}


private:
	std::unique_ptr<QtEngine> engine_;
	std::shared_ptr<Gerber> gerber_;

	int start_x_{ 0 };
	int start_y_{ 0 };
};

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	GerberWidget wnd;
	wnd.showMaximized();
	wnd.show();

	return app.exec();
}
