#include <gtest/gtest.h>
#include "gerber/gerber_parser.h"
#include "gerber/gerber_level.h"

TEST(GerberTest, TestParseFile1)
{
	GerberParser parser(std::string(TestData) + "2301113563-f-avi/2301113563-e-gbs");
	auto gerber = parser.GetGerber();
	EXPECT_EQ(gerber->Unit(), UnitType::Type::guMillimeters);
	EXPECT_EQ(gerber->GetBBox(), BoundBox(-40.710000000000001, 198.31, 36.710000000000001, -36.710000000000001));
	EXPECT_EQ(gerber->Name(), "");
	EXPECT_FALSE(gerber->IsNegative());

	auto levels = gerber->Levels();
	EXPECT_EQ(levels.size(), 4);
	EXPECT_EQ((*(++levels.begin()))->GetName(), "2301113563-e-gbs.sub2");
	EXPECT_DOUBLE_EQ((*(++levels.begin()))->step_x_, 12.800000000000001);
	EXPECT_DOUBLE_EQ((*(++levels.begin()))->step_y_, 12.800000000000001);
	EXPECT_EQ((*(++levels.begin()))->count_x_, 6);
	EXPECT_EQ((*(++levels.begin()))->count_y_, 5);
	EXPECT_EQ((*(++levels.begin()))->GetBoundBox(), BoundBox(43.344889999999999, 50.255110000000002, -22.14489, -29.055109999999996));
	EXPECT_FALSE((*(++levels.begin()))->IsNegative());
	EXPECT_EQ((*(++levels.begin()))->GetUnit(), UnitType::Type::guMillimeters);
	EXPECT_EQ((*(++levels.begin()))->RenderCommands().size(), 367);

	auto renders = (*(++levels.begin()))->RenderCommands();
	auto iter = renders.begin();
	++iter;
	++iter;
	++iter;
	EXPECT_EQ((*iter)->command_, RenderCommand::Command::cLine);
	EXPECT_DOUBLE_EQ((*iter)->X, 43.714999999999996);
	EXPECT_DOUBLE_EQ((*iter)->Y, -28.337990000000001);
}

TEST(GerberTest, TestParseFile2)
{
	GerberParser parser(std::string(TestData) + "gerber_files/sigb.gbr");
	auto gerber = parser.GetGerber();
	EXPECT_EQ(gerber->Unit(), UnitType::Type::guMillimeters);
	EXPECT_EQ(gerber->GetBBox(), BoundBox(-33.390000000000001, 244.91000000000000, 212.24000000000001, -23.059999999999999));
	EXPECT_EQ(gerber->Name(), "");
	EXPECT_FALSE(gerber->IsNegative());

	auto levels = gerber->Levels();
	EXPECT_EQ(levels.size(), 3);
	EXPECT_EQ(levels.front()->GetName(), "sigb_scpt.gbr");
	EXPECT_DOUBLE_EQ(levels.front()->step_x_, 0.0);
	EXPECT_DOUBLE_EQ(levels.front()->step_y_, 0.0);
	EXPECT_EQ(levels.front()->count_x_, 1);
	EXPECT_EQ(levels.front()->count_y_, 1);
	EXPECT_EQ(levels.front()->GetBoundBox(), BoundBox(-33.390000000000001, 244.91000000000000, 212.24000000000001, -23.059999999999999));
	EXPECT_FALSE(levels.front()->IsNegative());
	EXPECT_EQ(levels.front()->GetUnit(), UnitType::Type::guMillimeters);
	EXPECT_EQ(levels.front()->RenderCommands().size(), 375396);

	auto renders = levels.front()->RenderCommands();
	auto iter = renders.begin();
	++iter;
	++iter;
	++iter;
	EXPECT_EQ((*iter)->command_, RenderCommand::Command::cBeginLine);
	EXPECT_DOUBLE_EQ((*iter)->X, -0.45768999999999993);
	EXPECT_DOUBLE_EQ((*iter)->Y, 50.990940000000002);
}

TEST(GerberTest, TestParseFile3)
{
	GerberParser parser(std::string(TestData) + "gerber_files/lth_1-3.gbr");
	auto gerber = parser.GetGerber();
	EXPECT_EQ(gerber->Unit(), UnitType::Type::guMillimeters);
	EXPECT_EQ(gerber->GetBBox(), BoundBox(-32.044300000000000, 243.56430000000003, 210.78607000000000, -21.606069999999999));
	EXPECT_EQ(gerber->Name(), "");
	EXPECT_FALSE(gerber->IsNegative());

	auto levels = gerber->Levels();
	EXPECT_EQ(levels.size(), 1);
	EXPECT_EQ(levels.front()->GetName(), "lth_1-3_scpt.gbr");
	EXPECT_DOUBLE_EQ(levels.front()->step_x_, 0.0);
	EXPECT_DOUBLE_EQ(levels.front()->step_y_, 0.0);
	EXPECT_EQ(levels.front()->count_x_, 1);
	EXPECT_EQ(levels.front()->count_y_, 1);
	EXPECT_EQ(levels.front()->GetBoundBox(), BoundBox(-32.044300000000000, 243.56430000000003, 210.78607000000000, -21.606069999999999));
	EXPECT_FALSE(levels.front()->IsNegative());
	EXPECT_EQ(levels.front()->GetUnit(), UnitType::Type::guMillimeters);
	EXPECT_EQ(levels.front()->RenderCommands().size(), 3934);

	auto renders = levels.front()->RenderCommands();
	auto iter = renders.begin();
	++iter;
	++iter;
	++iter;
	EXPECT_EQ((*iter)->command_, RenderCommand::Command::cFlash);
	EXPECT_DOUBLE_EQ((*iter)->X, -3.7799999999999998);
	EXPECT_DOUBLE_EQ((*iter)->Y, 58.826800000000006);
}

TEST(GerberTest, TestParseFile4)
{
	GerberParser parser(std::string(TestData) + "gerber_files/111.gbr");
	auto gerber = parser.GetGerber();
	EXPECT_EQ(gerber->Unit(), UnitType::guInches);
	EXPECT_EQ(gerber->GetBBox(), BoundBox(-6.7393807645321662, 94.005399999999995, 148.53805699999998, -3.3578800000000006));
	EXPECT_EQ(gerber->Name(), "");
	EXPECT_FALSE(gerber->IsNegative());

	auto levels = gerber->Levels();
	EXPECT_EQ(levels.size(), 3);
	EXPECT_EQ(levels.front()->GetName(), "c272775-a-gbl");
	EXPECT_DOUBLE_EQ(levels.front()->step_x_, 0.0);
	EXPECT_DOUBLE_EQ(levels.front()->step_y_, 0.0);
	EXPECT_EQ(levels.front()->count_x_, 1);
	EXPECT_EQ(levels.front()->count_y_, 1);
	EXPECT_EQ(levels.front()->GetBoundBox(), BoundBox(-6.7393807645321662, 94.005399999999995, 148.53805699999998, -2.7390598000000002));
	EXPECT_FALSE(levels.front()->IsNegative());
	EXPECT_EQ(levels.front()->GetUnit(), UnitType::Type::guInches);
	EXPECT_EQ(levels.front()->RenderCommands().size(), 184249);

	auto renders = levels.front()->RenderCommands();
	auto iter = renders.begin();
	++iter;
	++iter;
	++iter;
	EXPECT_EQ((*iter)->command_, RenderCommand::Command::cFlash);
	EXPECT_DOUBLE_EQ((*iter)->X, 73.449179999999984);
	EXPECT_DOUBLE_EQ((*iter)->Y, 2.2504399999999993);
}

TEST(GerberTest, TestParseFile5)
{
	GerberParser parser(std::string(TestData) + "gerber_files/hj.324v1.gts");
	auto gerber = parser.GetGerber();
	EXPECT_EQ(gerber->Unit(), UnitType::guInches);
	EXPECT_EQ(gerber->GetBBox(), BoundBox(1.3982699999999999, 120.65076199999999, 98.830891999999992, 1.3677899999999998));
	EXPECT_EQ(gerber->Name(), "");
	EXPECT_FALSE(gerber->IsNegative());

	auto levels = gerber->Levels();
	EXPECT_EQ(levels.size(), 1);
	EXPECT_EQ(levels.front()->GetName(), "");
	EXPECT_DOUBLE_EQ(levels.front()->step_x_, 0.0);
	EXPECT_DOUBLE_EQ(levels.front()->step_y_, 0.0);
	EXPECT_EQ(levels.front()->count_x_, 1);
	EXPECT_EQ(levels.front()->count_y_, 1);
	EXPECT_EQ(levels.front()->GetBoundBox(), BoundBox(1.3982699999999999, 120.65076199999999, 98.830891999999992, 1.3677899999999998));
	EXPECT_FALSE(levels.front()->IsNegative());
	EXPECT_EQ(levels.front()->GetUnit(), UnitType::Type::guInches);
	EXPECT_EQ(levels.front()->RenderCommands().size(), 885);

	auto renders = levels.front()->RenderCommands();
	auto iter = renders.begin();
	++iter;
	++iter;
	++iter;
	EXPECT_EQ((*iter)->command_, RenderCommand::Command::cFlash);
	EXPECT_DOUBLE_EQ((*iter)->X, 100.125276);
	EXPECT_DOUBLE_EQ((*iter)->Y, 76.000102000000012);
}

TEST(GerberTest, TestParseFile6)
{
	GerberParser parser(std::string(TestData) + "Gerber_TopLayer.GTL");
	auto gerber = parser.GetGerber();
	EXPECT_EQ(gerber->Unit(), UnitType::guMillimeters);
	EXPECT_EQ(gerber->GetBBox(), BoundBox(-0.88243999999999989, 74.266999999999996, 17.241009999999999, -60.447420000000001));
	EXPECT_EQ(gerber->Name(), "");
	EXPECT_FALSE(gerber->IsNegative());

	auto levels = gerber->Levels();
	EXPECT_EQ(levels.size(), 14);
	EXPECT_EQ(levels.front()->GetName(), "");
	EXPECT_DOUBLE_EQ(levels.front()->step_x_, 0.0);
	EXPECT_DOUBLE_EQ(levels.front()->step_y_, 0.0);
	EXPECT_EQ(levels.front()->count_x_, 1);
	EXPECT_EQ(levels.front()->count_y_, 1);
	EXPECT_EQ(levels.front()->GetBoundBox(), BoundBox(-0.75691999999999993, 1.42798, 13.541749999999999, 10.488160000000001));
	EXPECT_FALSE(levels.front()->IsNegative());
	EXPECT_EQ(levels.front()->GetUnit(), UnitType::Type::guMillimeters);
	EXPECT_EQ(levels.front()->RenderCommands().size(), 79);

	auto renders = levels.front()->RenderCommands();
	auto iter = renders.begin();
	++iter;
	++iter;
	++iter;
	EXPECT_EQ((*iter)->command_, RenderCommand::Command::cLine);
	EXPECT_DOUBLE_EQ((*iter)->X, -0.72998999999999992);
	EXPECT_DOUBLE_EQ((*iter)->Y, 10.52017);
}
