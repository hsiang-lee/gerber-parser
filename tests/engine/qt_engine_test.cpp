#include <gtest/gtest.h>
#include "engine/qt_engine.h"
#include <QPixmap>
#include <QPainter>
#include <QApplication>


class TestingQtEngine : public QtEngine {
public:
	using QtEngine::QtEngine;
	using QtEngine::BeginRender;

	std::shared_ptr<QPainter> CreatePainter(QPaintDevice* pic) override {
		if (!painter_) {
			painter_ = std::make_shared<QPainter>(pic);
		}

		return painter_;
	}

	std::shared_ptr<QPainter> painter_;
};


TEST(QtEngineTest, TestBeginRenderDefault) {
	int argc = 0;
	char* argv[1];
	QApplication app(argc, argv);

	QPixmap pixmap(1000, 1000);
	TestingQtEngine engine(&pixmap);

	engine.BeginRender(BoundBox(-100.0, 150.0, 100.0, -150.0));

	EXPECT_EQ(engine.painter_->viewport(), QRect(25, 25, 950, 950));
	EXPECT_EQ(engine.painter_->window(), QRect(-1000000, 1000000, 2500000, -2500000));
}
