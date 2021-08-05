#include <gtest/gtest.h>
#include "gerber/gerber.h"
#include "gerber/gerber_enums.h"


TEST(GerberTest, TestParseFile1) {
	Gerber gerber(std::string(TestData) + "2301113563-e-gbs");
	EXPECT_EQ(gerber.Unit(), GERBER_UNIT::guMillimeters);
	EXPECT_EQ(gerber.GetBBox(), BoundBox(-40.710000000000001, 198.31, 36.710000000000001, -36.710000000000001));
	EXPECT_EQ(gerber.Name(), "");
	EXPECT_FALSE(gerber.IsNegative());

	auto levels = gerber.Levels();
	EXPECT_EQ(levels.size(), 4);
	EXPECT_EQ((*(++levels.begin()))->name_, "2301113563-e-gbs.sub2");
	EXPECT_DOUBLE_EQ((*(++levels.begin()))->StepX, 12.800000000000001);
	EXPECT_DOUBLE_EQ((*(++levels.begin()))->StepY, 12.800000000000001);
	EXPECT_EQ((*(++levels.begin()))->CountX, 6);
	EXPECT_EQ((*(++levels.begin()))->CountY, 5);
	EXPECT_EQ((*(++levels.begin()))->bound_box_, BoundBox(43.344889999999999, 50.255110000000002, -22.14489, -29.055109999999996));
	EXPECT_DOUBLE_EQ((*(++levels.begin()))->X, 47.954920000000001);
	EXPECT_DOUBLE_EQ((*(++levels.begin()))->Y, -27.655099999999997);
	EXPECT_DOUBLE_EQ((*(++levels.begin()))->I, 0.0);
	EXPECT_DOUBLE_EQ((*(++levels.begin()))->J, 0.0);
	EXPECT_FALSE((*(++levels.begin()))->negative_);
	EXPECT_FALSE((*(++levels.begin()))->relative_);
	EXPECT_FALSE((*(++levels.begin()))->incremental_);
	EXPECT_TRUE((*(++levels.begin()))->multi_quadrant_);
	EXPECT_EQ((*(++levels.begin()))->interpolation_, GERBER_INTERPOLATION::giLinear);
	EXPECT_EQ((*(++levels.begin()))->exposure_, GERBER_EXPOSURE::geOff);
	EXPECT_EQ((*(++levels.begin()))->units_, GERBER_UNIT::guMillimeters);
	EXPECT_EQ((*(++levels.begin()))->RenderCommands().size(), 367);

	auto renders = (*(++levels.begin()))->RenderCommands();
	auto iter = renders.begin();
	++iter;
	++iter;
	++iter;
	EXPECT_EQ((*iter)->command_, RenderCommand::GerberCommand::gcLine);
	EXPECT_DOUBLE_EQ((*iter)->X, 43.714999999999996);
	EXPECT_DOUBLE_EQ((*iter)->Y, -28.337990000000001);
}

TEST(GerberTest, TestParseFile2) {
	Gerber gerber(std::string(TestData) + "lth_1-3.gbr");
	EXPECT_EQ(gerber.Unit(), GERBER_UNIT::guMillimeters);
	EXPECT_EQ(gerber.GetBBox(), BoundBox(-32.044300000000000, 243.56430000000003, 210.78607000000000, -21.606069999999999));
	EXPECT_EQ(gerber.Name(), "");
	EXPECT_FALSE(gerber.IsNegative());

	auto levels = gerber.Levels();
	EXPECT_EQ(levels.size(), 1);
	EXPECT_EQ(levels.front()->name_, "lth_1-3_scpt.gbr");
	EXPECT_DOUBLE_EQ(levels.front()->StepX, 0.0);
	EXPECT_DOUBLE_EQ(levels.front()->StepY, 0.0);
	EXPECT_EQ(levels.front()->CountX, 1);
	EXPECT_EQ(levels.front()->CountY, 1);
	EXPECT_EQ(levels.front()->bound_box_, BoundBox(-32.044300000000000, 243.56430000000003, 210.78607000000000, -21.606069999999999));
	EXPECT_DOUBLE_EQ(levels.front()->X, 50.605729999999994);
	EXPECT_DOUBLE_EQ(levels.front()->Y, 202.39965999999998);
	EXPECT_DOUBLE_EQ(levels.front()->I, 0.0);
	EXPECT_DOUBLE_EQ(levels.front()->J, 0.0);
	EXPECT_FALSE(levels.front()->negative_);
	EXPECT_FALSE(levels.front()->relative_);
	EXPECT_FALSE(levels.front()->incremental_);
	EXPECT_TRUE(levels.front()->multi_quadrant_);
	EXPECT_EQ(levels.front()->interpolation_, GERBER_INTERPOLATION::giLinear);
	EXPECT_EQ(levels.front()->exposure_, GERBER_EXPOSURE::geOff);
	EXPECT_EQ(levels.front()->units_, GERBER_UNIT::guMillimeters);
	EXPECT_EQ(levels.front()->RenderCommands().size(), 3934);

	auto renders = levels.front()->RenderCommands();
	auto iter = renders.begin();
	++iter;
	++iter;
	++iter;
	EXPECT_EQ((*iter)->command_, RenderCommand::GerberCommand::gcFlash);
	EXPECT_DOUBLE_EQ((*iter)->X, -3.7799999999999998);
	EXPECT_DOUBLE_EQ((*iter)->Y, 58.826800000000006);
}

TEST(GerberTest, TestParseFile3) {
	Gerber gerber(std::string(TestData) + "susb.gbr");
	EXPECT_EQ(gerber.Unit(), GERBER_UNIT::guMillimeters);
	EXPECT_EQ(gerber.GetBBox(), BoundBox(-30.780000000000001, 242.30000000000001, 205.45999999999998, -16.280000000000001));
	EXPECT_EQ(gerber.Name(), "");
	EXPECT_FALSE(gerber.IsNegative());

	auto levels = gerber.Levels();
	EXPECT_EQ(levels.size(), 1);
	EXPECT_EQ(levels.front()->name_, "susb_scpt.gbr");
	EXPECT_DOUBLE_EQ(levels.front()->StepX, 0.0);
	EXPECT_DOUBLE_EQ(levels.front()->StepY, 0.0);
	EXPECT_EQ(levels.front()->CountX, 1);
	EXPECT_EQ(levels.front()->CountY, 1);
	EXPECT_EQ(levels.front()->bound_box_, BoundBox(-30.780000000000001, 242.30000000000001, 205.45999999999998, -16.280000000000001));
	EXPECT_DOUBLE_EQ(levels.front()->X, 200.02000000000001);
	EXPECT_DOUBLE_EQ(levels.front()->Y, 205.45999999999998);
	EXPECT_DOUBLE_EQ(levels.front()->I, 0.0);
	EXPECT_DOUBLE_EQ(levels.front()->J, 0.0);
	EXPECT_FALSE(levels.front()->negative_);
	EXPECT_FALSE(levels.front()->relative_);
	EXPECT_FALSE(levels.front()->incremental_);
	EXPECT_TRUE(levels.front()->multi_quadrant_);
	EXPECT_EQ(levels.front()->interpolation_, GERBER_INTERPOLATION::giClockwiseCircular);
	EXPECT_EQ(levels.front()->exposure_, GERBER_EXPOSURE::geOff);
	EXPECT_EQ(levels.front()->units_, GERBER_UNIT::guMillimeters);
	EXPECT_EQ(levels.front()->RenderCommands().size(), 576);

	auto renders = levels.front()->RenderCommands();
	auto iter = renders.begin();
	++iter;
	++iter;
	++iter;
	EXPECT_EQ((*iter)->command_, RenderCommand::GerberCommand::gcArc);
	EXPECT_DOUBLE_EQ((*iter)->X, -23.668299999999999);
	EXPECT_DOUBLE_EQ((*iter)->Y, 1.0);
}
