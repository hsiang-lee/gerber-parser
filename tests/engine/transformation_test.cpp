#include <gtest/gtest.h>
#include "engine/transformation.h"


TEST(TransformationTest, TestGetScale) {
	Transformation trans(BoundBox(-1, 1, 1, -1), BoundBox(0.025, 0.025, 0.025, 0.025));
	trans.SetPhysicalSize(200, 100);

	EXPECT_DOUBLE_EQ(trans.TranslateLogicCoord(1.0), 47.5);
}

TEST(TransformationTest, TestGetPainterWindow) {
	Transformation trans(BoundBox(-100, 100, 100, -100), BoundBox(0.025, 0.025, 0.025, 0.025));
	trans.SetPhysicalSize(200, 100);

	const auto window = trans.GetPainterWindow();
	EXPECT_EQ(window, QRect(-200, 100, 400, -200));
}

TEST(TransformationTest, TestGetPainterViewport) {
	Transformation trans(BoundBox(-100, 100, 100, -100), BoundBox(0.025, 0.025, 0.025, 0.025));
	trans.SetPhysicalSize(200, 100);

	const auto viewport = trans.GetPainterViewport();
	EXPECT_EQ(viewport, QRect(5, 2, 190, 95));
}

TEST(TransformationTest, TestScaleAtCenter) {
	Transformation trans(BoundBox(-100, 100, 100, -100), BoundBox(0.025, 0.025, 0.025, 0.025));
	trans.SetPhysicalSize(200, 100);

	trans.Scale(1.0, 0.5, 0.5);

	const auto window = trans.GetPainterWindow();
	EXPECT_EQ(window, QRect(-100, 50, 200, -100));
}

TEST(TransformationTest, TestScaleAtBottomRight) {
	Transformation trans(BoundBox(-100, 100, 100, -100), BoundBox(0.025, 0.025, 0.025, 0.025));
	trans.SetPhysicalSize(200, 100);

	trans.Scale(1.0, 1.0, 1.0);

	const auto window = trans.GetPainterWindow();
	EXPECT_EQ(window, QRect(0, 0, 200, -100));
}

TEST(TransformationTest, TestScaleAtTopRight) {
	Transformation trans(BoundBox(-100, 100, 100, -100), BoundBox(0.025, 0.025, 0.025, 0.025));
	trans.SetPhysicalSize(200, 100);

	trans.Scale(1.0, 1.0, 0);

	const auto window = trans.GetPainterWindow();
	EXPECT_EQ(window, QRect(0, 100, 200, -100));
}


TEST(TransformationTest, TestScaleWidthLargerThanHeight) {
	Transformation trans(BoundBox(-100, 100, 100, -100), BoundBox(0.025, 0.025, 0.025, 0.025));
	trans.SetPhysicalSize(200, 100);

	trans.Scale(1.0, 0.5, 1.0);

	const auto window = trans.GetPainterWindow();
	EXPECT_EQ(window, QRect(-100, 0, 200, -100));
}

TEST(TransformationTest, TestScaleHeightLargerThanWidth) {
	Transformation trans(BoundBox(-100, 100, 100, -100), BoundBox(0.025, 0.025, 0.025, 0.025));
	trans.SetPhysicalSize(100, 200);

	trans.Scale(1.0, 0.5, 1.0);

	const auto window = trans.GetPainterWindow();
	EXPECT_EQ(window, QRect(-50, 0, 100, -200));
}
