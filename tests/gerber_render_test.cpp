#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "engine/qt_engine.h"
#include "gerber_render.h"
#include <QImage>
#include <QApplication>


TEST(GerbRenderTest, TestRenderFromGerber) {
	int argc = 0;
	char* argv[1];
	QApplication app(argc, argv);

	auto image = std::make_unique<QImage>(2000, 2000, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QtEngine>(image.get());
	GerberRender render(engine.get());

	auto gerber = std::make_shared<Gerber>(std::string(TestData) + "2301113563-f-gtl");
	render.RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113563-f-gtl.bmp");

	QImage expected(QString(TestData) + "results/2301113563-f-gtl.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST(GerbRenderTest, TestScale) {
	int argc = 0;
	char* argv[1];
	QApplication app(argc, argv);

	auto image = std::make_unique<QImage>(2000, 2000, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QtEngine>(image.get());
	GerberRender render(engine.get());

	auto gerber = std::make_shared<Gerber>(std::string(TestData) + "2301113563-f-gtl");

	engine->Scale(1.2, 0.9, 0.8);
	render.RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113563-f-gtl_scale1_2.bmp");

	QImage expected(QString(TestData) + "results/2301113563-f-gtl_scale1_2.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
}

TEST(GerbRenderTest, TestMove) {
	int argc = 0;
	char* argv[1];
	QApplication app(argc, argv);

	auto image = std::make_unique<QImage>(2000, 2000, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QtEngine>(image.get());
	GerberRender render(engine.get());

	auto gerber = std::make_shared<Gerber>(std::string(TestData) + "2301113563-f-gtl");

	engine->Move(400, 600);
	render.RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113563-f-gtl_move.bmp");

	QImage expected(QString(TestData) + "results/2301113563-f-gtl_move.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_EQ(gerber->GetBBox(), BoundBox(-42.900000000000006, 200.50000000000000, 38.700000000000003, -38.700000000000003));
}

TEST(GerbRenderTest, TestSelect) {
	int argc = 0;
	char* argv[1];
	QApplication app(argc, argv);

	auto image = std::make_unique<QImage>(2000, 2000, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QtEngine>(image.get());
	GerberRender render(engine.get());

	auto gerber = std::make_shared<Gerber>(std::string(TestData) + "2301113563-f-gtl");

	engine->Select(2500, 2500);
	render.RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113563-f-gtl_select.bmp");

	QImage expected(QString(TestData) + "results/2301113563-f-gtl_select.bmp");
	EXPECT_EQ(*image, expected);
}


TEST(GerbRenderTest, TestConvertStroke2Fill) {
	int argc = 0;
	char* argv[1];
	QApplication app(argc, argv);

	auto image = std::make_unique<QImage>(2000, 2000, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QtEngine>(image.get());
	engine->convert_strokes2fills_ = true;
	GerberRender render(engine.get());

	auto gerber = std::make_shared<Gerber>(std::string(TestData) + "2301113563-f-gtl");

	engine->Select(2500, 2500);
	render.RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113563-f-gtl_stroke2fill.bmp");

	QImage expected(QString(TestData) + "results/2301113563-f-gtl_stroke2fill.bmp");
	EXPECT_EQ(*image, expected);
}
