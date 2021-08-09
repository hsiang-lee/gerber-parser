#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QFileDialog>

#include "gerber_render.h"
#include "engine/qt_engine.h"

class GerberWidget : public QWidget {
public:
	GerberWidget() {
		auto file = QFileDialog::getOpenFileName(this);

		engine_ = std::make_unique<QtEngine>(this);
		render_ = std::make_unique<GerberRender>(engine_.get());
		gerber_ = std::make_shared<Gerber>(file.toLocal8Bit().toStdString());
	}

protected:
	void paintEvent(QPaintEvent* e) override {
		render_->RenderGerber(gerber_);
	}

	void wheelEvent(QWheelEvent* e) override {
		engine_->Scale(e->delta() / 720.0, double(e->x()) / width(), double(e->y()) / height());
		QWidget::wheelEvent(e);

		update();
	}

	void mousePressEvent(QMouseEvent* e) override {
		pressed_ = true;
		start_x_ = e->x();
		start_y_ = e->y();
	}

	void mouseReleaseEvent(QMouseEvent* e) override {
		pressed_ = false;

		engine_->Select(e->x(), e->y());
		update();
	}

	void mouseMoveEvent(QMouseEvent* e) override {
		if (pressed_) {
			engine_->Move(e->x() - start_x_, e->y() - start_y_);

			start_x_ = e->x();
			start_y_ = e->y();

			update();
		}
	}


private:
	std::unique_ptr<QtEngine> engine_;
	std::unique_ptr<GerberRender> render_;
	std::shared_ptr<Gerber> gerber_;

	bool pressed_{ false };
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
