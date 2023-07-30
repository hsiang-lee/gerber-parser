#include <gtest/gtest.h>
#include "engines/qpainter_engine.h"
#include "gerber/gerber.h"
#include "gerber_parser/gerber_parser.h"

#include <QImage>
#include <QApplication>

class GerberRendererTest : public testing::Test {
 protected:
	 GerberRendererTest() {
    if (!app_) {
      int argc = 0;
      char *argv[1];
      app_ = std::make_unique<QApplication>(argc, argv);
    }
  }

	 static std::unique_ptr<QApplication> app_;
};

std::unique_ptr<QApplication> GerberRendererTest::app_;

TEST_F(GerberRendererTest, TestRenderFromGerber)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301113563-f-gtl");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113563-f-gtl.bmp");

	QImage expected(QString(TestData) + "results/2301113563-f-gtl.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestScale)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301113563-f-gtl");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->Scale(1.2, 0.9, 0.8);
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113563-f-gtl_scale1_2.bmp");

	QImage expected(QString(TestData) + "results/2301113563-f-gtl_scale1_2.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
}

TEST_F(GerberRendererTest, TestMove)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301113563-f-gtl");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);

	engine->DrawBackground();
	engine->Move(400, 600);
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113563-f-gtl_move.bmp");

	QImage expected(QString(TestData) + "results/2301113563-f-gtl_move.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_EQ(gerber->GetBBox(), BoundBox(-42.900000000000006, 200.50000000000000, 38.700000000000003, -38.700000000000003));
}

TEST_F(GerberRendererTest, TestConvertStroke2Fill)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301113563-f-gtl");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground(); 
	engine->SetConvertStroke2Fills(true);

	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113563-f-gtl_stroke2fill.bmp");

	QImage expected(QString(TestData) + "results/2301113563-f-gtl_stroke2fill.bmp");
	EXPECT_EQ(*image, expected);
}

TEST_F(GerberRendererTest, TestRenderGerberFile1)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301113987c.dat");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113987c.dat.bmp");

	QImage expected(QString(TestData) + "results/2301113987c.dat.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile2)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301113987c.rout");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113987c.rout.bmp");

	QImage expected(QString(TestData) + "results/2301113987c.rout.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile3)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301113987-c-gbl");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113987-c-gbl.bmp");

	QImage expected(QString(TestData) + "results/2301113987-c-gbl.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile4)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301113987-c-gbs");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113987-c-gbs.bmp");

	QImage expected(QString(TestData) + "results/2301113987-c-gbs.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile5)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301113987-c-gtl");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113987-c-gtl.bmp");

	QImage expected(QString(TestData) + "results/2301113987-c-gtl.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile6)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301113987-c-gts");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301113987-c-gts.bmp");

	QImage expected(QString(TestData) + "results/2301113987-c-gts.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile7)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301115633.rout");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301115633.rout.bmp");

	QImage expected(QString(TestData) + "results/2301115633.rout.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile8)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301115633lg.dat");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301115633lg.dat.bmp");

	QImage expected(QString(TestData) + "results/2301115633lg.dat.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile9)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301115633lg.ld12");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->Scale(5.0);
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301115633lg.ld12.bmp");

	QImage expected(QString(TestData) + "results/2301115633lg.ld12.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile10)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301115633lg.ld21");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground();
	engine->Scale(5.0);
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301115633lg.ld21.bmp");

	QImage expected(QString(TestData) + "results/2301115633lg.ld21.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile11)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301115633-lg-gbl");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground(); 
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301115633-lg-gbl.bmp");

	QImage expected(QString(TestData) + "results/2301115633-lg-gbl.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile12)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301115633-lg-gbs");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground(); 
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301115633-lg-gbs.bmp");

	QImage expected(QString(TestData) + "results/2301115633-lg-gbs.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile13)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301115633-lg-gtl");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground(); 
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301115633-lg-gtl.bmp");

	QImage expected(QString(TestData) + "results/2301115633-lg-gtl.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile14)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/2301115633-lg-gts");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground(); 
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/2301115633-lg-gts.bmp");

	QImage expected(QString(TestData) + "results/2301115633-lg-gts.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile15)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/hj.324v1.gts");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground(); 
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/hj.324v1.gts.bmp");

	QImage expected(QString(TestData) + "results/hj.324v1.gts.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}

TEST_F(GerberRendererTest, TestRenderGerberFile16)
{
	auto parser = std::make_shared<GerberParser>(std::string(TestData) + "gerber_files/BOTTOM.art");
	auto gerber = parser->GetGerber();

	auto image = std::make_unique<QImage>(1600, 1600, QImage::Format::Format_RGB32);
	auto engine = std::make_unique<QPainterEngine>(image.get(), gerber->GetBBox(), 0.005);
	engine->DrawBackground(); 
	engine->RenderGerber(gerber);

	//image->save(QString(TestData) + "results/BOTTOM.art.bmp");

	QImage expected(QString(TestData) + "results/BOTTOM.art.bmp");
	EXPECT_EQ(*image, expected);

	EXPECT_FALSE(gerber->IsNegative());
	EXPECT_EQ(gerber->Name(), "");
}
