#include <gtest/gtest.h>
#include "gerber_file.h"
#include "gerber_parser/gerber_parser.h"
#include "gerber/gerber_layer.h"
#include "aperture/aperture.h"
#include "gerber/gerber_primitive.h"

#include <fstream>

TEST(GerberTest, TestParseFile1)
{
    GerberParser parser(std::string(TestData) + "2301113563-f-avi/2301113563-e-gbs");
    auto gerber = parser.GetGerber();
    EXPECT_EQ(gerber->Unit(), UnitType::Type::guMillimeters);
    EXPECT_EQ(gerber->GetBBox(), BoundBox(-40.710000000000001, 198.31, 36.710000000000001, -37.335703026746806));
    EXPECT_EQ(gerber->Name(), "");
    EXPECT_FALSE(gerber->IsNegative());

    const auto &layers = gerber->GetLayers();
    EXPECT_EQ(layers.size(), 4);
    EXPECT_EQ((*(++layers.begin()))->GetName(), "2301113563-e-gbs.sub2");
    EXPECT_DOUBLE_EQ((*(++layers.begin()))->step_x_, 12.800000000000001);
    EXPECT_DOUBLE_EQ((*(++layers.begin()))->step_y_, 12.800000000000001);
    EXPECT_EQ((*(++layers.begin()))->count_x_, 6);
    EXPECT_EQ((*(++layers.begin()))->count_y_, 5);
    EXPECT_EQ((*(++layers.begin()))->GetBBox(), BoundBox(43.344889999999999, 50.255110000000002, -22.14489, -29.055109999999996));
    EXPECT_FALSE((*(++layers.begin()))->IsNegative());
    EXPECT_EQ((*(++layers.begin()))->GetUnit(), UnitType::Type::guMillimeters);
    EXPECT_EQ((*(++layers.begin()))->Primitives().size(), 46);

    auto renders = (*(++layers.begin()))->Primitives();
    auto iter = renders.begin();
    ++iter;
    ++iter;
    ++iter;
    ++iter;
    auto cmd = dynamic_cast<Single*>(*iter);
    EXPECT_EQ(cmd->type_, Primitive::Type::cOutline);
    EXPECT_EQ(cmd->start_, std::make_pair(0.0, 0.0));
}

TEST(GerberTest, TestParseFile2)
{
    GerberParser parser(std::string(TestData) + "gerber_files/sigb.gbr");
    auto gerber = parser.GetGerber();
    EXPECT_EQ(gerber->Unit(), UnitType::Type::guMillimeters);
    EXPECT_EQ(gerber->GetBBox(), BoundBox(-33.390000000000001, 244.91000000000000, 212.24000000000001, -33.339200000000005));
    EXPECT_EQ(gerber->Name(), "");
    EXPECT_FALSE(gerber->IsNegative());

    const auto &layers = gerber->GetLayers();
    EXPECT_EQ(layers.size(), 3);
    EXPECT_EQ(layers.front()->GetName(), "sigb_scpt.gbr");
    EXPECT_DOUBLE_EQ(layers.front()->step_x_, 0.0);
    EXPECT_DOUBLE_EQ(layers.front()->step_y_, 0.0);
    EXPECT_EQ(layers.front()->count_x_, 1);
    EXPECT_EQ(layers.front()->count_y_, 1);
    EXPECT_EQ(layers.front()->GetBBox(), BoundBox(-33.390000000000001, 244.91000000000000, 212.24000000000001, -33.339200000000005));
    EXPECT_FALSE(layers.front()->IsNegative());
    EXPECT_EQ(layers.front()->GetUnit(), UnitType::Type::guMillimeters);
    EXPECT_EQ(layers.front()->Primitives().size(), 95768);

    auto renders = layers.front()->Primitives();
    auto iter = renders.begin();
    ++iter;
    ++iter;
    ++iter;
    ++iter;
    ++iter;
    auto cmd = dynamic_cast<Single*>(*iter);
    EXPECT_EQ(cmd->type_, Primitive::Type::cOutline);
    EXPECT_EQ(cmd->start_, std::make_pair(0.0, 0.0));
}

TEST(GerberTest, TestParseFile3)
{
    GerberParser parser(std::string(TestData) + "gerber_files/lth_1-3.gbr");
    auto gerber = parser.GetGerber();
    EXPECT_EQ(gerber->Unit(), UnitType::Type::guMillimeters);
    EXPECT_EQ(gerber->GetBBox(), BoundBox(-32.044300000000000, 243.56430000000003, 210.78607000000000, -21.606069999999999));
    EXPECT_EQ(gerber->Name(), "");
    EXPECT_FALSE(gerber->IsNegative());

    EXPECT_EQ(gerber->GetAperture(10)->Type(), Aperture::tCircle);
    EXPECT_EQ(gerber->GetAperture(11)->Type(), Aperture::tCircle);
    EXPECT_EQ(gerber->GetAperture(12)->Type(), Aperture::tMacro);
    EXPECT_EQ(gerber->GetAperture(13)->Type(), Aperture::tCircle);

    const auto &layers = gerber->GetLayers();
    EXPECT_EQ(layers.size(), 1);
    EXPECT_EQ(layers.front()->GetName(), "lth_1-3_scpt.gbr");
    EXPECT_DOUBLE_EQ(layers.front()->step_x_, 0.0);
    EXPECT_DOUBLE_EQ(layers.front()->step_y_, 0.0);
    EXPECT_EQ(layers.front()->count_x_, 1);
    EXPECT_EQ(layers.front()->count_y_, 1);
    EXPECT_EQ(layers.front()->GetBBox(), BoundBox(-32.044300000000000, 243.56430000000003, 210.78607000000000, -21.606069999999999));
    EXPECT_FALSE(layers.front()->IsNegative());
    EXPECT_EQ(layers.front()->GetUnit(), UnitType::Type::guMillimeters);
    EXPECT_EQ(layers.front()->Primitives().size(), 2006);

    auto renders = layers.front()->Primitives();
    auto iter = renders.begin();
    ++iter;
    ++iter;
    ++iter;
    auto cmd = dynamic_cast<Single*>(*iter);
    EXPECT_EQ(cmd->type_, Primitive::Type::cFlash);
    EXPECT_EQ(cmd->start_, std::make_pair(-3.7799999999999998, 57.526800000000001));
}

TEST(GerberTest, TestParseFile4)
{
    GerberParser parser(std::string(TestData) + "gerber_files/111.gbr");
    auto gerber = parser.GetGerber();
    EXPECT_EQ(gerber->Unit(), UnitType::guInches);
    EXPECT_EQ(gerber->GetBBox(), BoundBox(-6.7393807758038617, 94.003620775803867, 154.60929656806547, -6.3528165585474454));
    EXPECT_EQ(gerber->Name(), "");
    EXPECT_FALSE(gerber->IsNegative());

    const auto &layers = gerber->GetLayers();
    EXPECT_EQ(layers.size(), 3);
    EXPECT_EQ(layers.front()->GetName(), "c272775-a-gbl");
    EXPECT_DOUBLE_EQ(layers.front()->step_x_, 0.0);
    EXPECT_DOUBLE_EQ(layers.front()->step_y_, 0.0);
    EXPECT_EQ(layers.front()->count_x_, 1);
    EXPECT_EQ(layers.front()->count_y_, 1);
    EXPECT_EQ(layers.front()->GetBBox(), BoundBox(-6.7393807758038617, 94.003620775803867, 148.53805699999998, -6.3528165585474454));
    EXPECT_FALSE(layers.front()->IsNegative());
    EXPECT_EQ(layers.front()->GetUnit(), UnitType::Type::guInches);
    EXPECT_EQ(layers.front()->Primitives().size(), 58480);

    auto renders = layers.front()->Primitives();
    auto iter = renders.begin();
    ++iter;
    ++iter;
    ++iter;
    auto cmd = dynamic_cast<Single*>(*iter);
    EXPECT_EQ(cmd->type_, Primitive::Type::cFlash);
    EXPECT_EQ(cmd->start_, std::make_pair(56.405779999999993, 3.09626));
}

TEST(GerberTest, TestParseFile5)
{
    GerberParser parser(std::string(TestData) + "gerber_files/hj.324v1.gts");
    auto gerber = parser.GetGerber();
    EXPECT_EQ(gerber->Unit(), UnitType::guInches);
    EXPECT_EQ(gerber->GetBBox(), BoundBox(1.3982699999999999, 120.65076199999999, 98.830891999999992, 1.3677899999999998));
    EXPECT_EQ(gerber->Name(), "");
    EXPECT_FALSE(gerber->IsNegative());

    const auto &layers = gerber->GetLayers();
    EXPECT_EQ(layers.size(), 1);
    EXPECT_EQ(layers.front()->GetName(), "");
    EXPECT_DOUBLE_EQ(layers.front()->step_x_, 0.0);
    EXPECT_DOUBLE_EQ(layers.front()->step_y_, 0.0);
    EXPECT_EQ(layers.front()->count_x_, 1);
    EXPECT_EQ(layers.front()->count_y_, 1);
    EXPECT_EQ(layers.front()->GetBBox(), BoundBox(1.3982699999999999, 120.65076199999999, 98.830891999999992, 1.3677899999999998));
    EXPECT_FALSE(layers.front()->IsNegative());
    EXPECT_EQ(layers.front()->GetUnit(), UnitType::Type::guInches);
    EXPECT_EQ(layers.front()->Primitives().size(), 853);

    auto renders = layers.front()->Primitives();
    auto iter = renders.begin();
    ++iter;
    ++iter;
    ++iter;
    auto cmd = dynamic_cast<Single*>(*iter);
    EXPECT_EQ(cmd->type_, Primitive::Type::cFlash);
    EXPECT_EQ(cmd->start_, std::make_pair(43.874943999999992, 76.000102000000012));
}

TEST(GerberTest, TestParseFile6)
{
    GerberParser parser(std::string(TestData) + "Gerber_TopLayer.GTL");
    auto gerber = parser.GetGerber();
    EXPECT_EQ(gerber->Unit(), UnitType::guMillimeters);
    EXPECT_EQ(gerber->GetBBox(), BoundBox(-0.88243999999999989, 74.266999999999996, 17.241009999999999, -60.447420000000001));
    EXPECT_EQ(gerber->Name(), "");
    EXPECT_FALSE(gerber->IsNegative());

    EXPECT_EQ(gerber->GetAperture(10)->Type(), Aperture::tCircle);
    EXPECT_EQ(gerber->GetAperture(16)->Type(), Aperture::tRectangle);
    EXPECT_EQ(gerber->GetAperture(18)->Type(), Aperture::tRectangle);
    EXPECT_EQ(gerber->GetAperture(20)->Type(), Aperture::tRectangle);

    const auto &layers = gerber->GetLayers();
    EXPECT_EQ(layers.size(), 14);
    EXPECT_EQ(layers.front()->GetName(), "");
    EXPECT_DOUBLE_EQ(layers.front()->step_x_, 0.0);
    EXPECT_DOUBLE_EQ(layers.front()->step_y_, 0.0);
    EXPECT_EQ(layers.front()->count_x_, 1);
    EXPECT_EQ(layers.front()->count_y_, 1);
    EXPECT_EQ(layers.front()->GetBBox(), BoundBox(-0.75691999999999993, 1.42798, 13.541749999999999, 10.488160000000001));
    EXPECT_FALSE(layers.front()->IsNegative());
    EXPECT_EQ(layers.front()->GetUnit(), UnitType::Type::guMillimeters);
    EXPECT_EQ(layers.front()->Primitives().size(), 1);

    auto renders = layers.front()->Primitives();
    auto iter = renders.begin();
    auto cmd = dynamic_cast<Single*>(*iter);
    EXPECT_EQ(cmd->type_, Primitive::Type::cOutline);
    EXPECT_EQ(cmd->start_, std::make_pair(0.0, 0.0));
}

TEST(GerberTest, TestParseFile7)
{
    GerberParser parser(std::string(TestData) + "10ls");
    auto gerber = parser.GetGerber();
    EXPECT_EQ(gerber->Unit(), UnitType::guInches);
    EXPECT_EQ(gerber->Name(), "");
    EXPECT_FALSE(gerber->IsNegative());

    const auto &layers = gerber->GetLayers();
    EXPECT_EQ(layers.size(), 5);
    EXPECT_EQ(layers.front()->GetName(), "10ls");
    EXPECT_DOUBLE_EQ(layers.front()->step_x_, 0.0);
    EXPECT_DOUBLE_EQ(layers.front()->step_y_, 0.0);
    EXPECT_EQ(layers.front()->count_x_, 1);
    EXPECT_EQ(layers.front()->count_y_, 1);
    EXPECT_EQ(layers.front()->GetBBox(), BoundBox(0.65572639999999993, 296.91426519999999, 242.48325899999998, 0.45981619999999995));
    EXPECT_FALSE(layers.front()->IsNegative());
    EXPECT_EQ(layers.front()->GetUnit(), UnitType::Type::guInches);
    EXPECT_EQ(layers.front()->Primitives().size(), 179887);

    auto renders = layers.front()->Primitives();
    auto iter = renders.begin();
    ++iter;
    ++iter;
    ++iter;
    auto cmd = dynamic_cast<Single*>(*iter);
    EXPECT_EQ(cmd->type_, Primitive::Type::cOutline);
    EXPECT_EQ(cmd->start_, std::make_pair(0.0, 0.0));
}

TEST(GerberTest, TestParseFile8)
{
    GerberParser parser(std::string(TestData) + "DD1672-A2C-Glass.fph");
    auto gerber = parser.GetGerber();
    EXPECT_EQ(gerber->Unit(), UnitType::guMillimeters);
    EXPECT_EQ(gerber->Name(), "");
    EXPECT_FALSE(gerber->IsNegative());

    const auto &layers = gerber->GetLayers();
    EXPECT_EQ(layers.size(), 3);
    EXPECT_EQ(layers.front()->GetName(), "");
    EXPECT_DOUBLE_EQ(layers.front()->step_x_, 0.0);
    EXPECT_DOUBLE_EQ(layers.front()->step_y_, 0.0);
    EXPECT_EQ(layers.front()->count_x_, 1);
    EXPECT_EQ(layers.front()->count_y_, 1);

    auto box = layers.front()->GetBBox();

    EXPECT_EQ(layers.front()->GetBBox(), BoundBox(100000, -100000, -100000, 100000));
    EXPECT_TRUE(layers.back()->IsNegative());
    EXPECT_EQ(layers.back()->GetUnit(), UnitType::Type::guMillimeters);
    EXPECT_EQ(layers.back()->Primitives().size(), 53280);

    auto renders = layers.back()->Primitives();
    auto iter = renders.end();
    --iter;
    --iter;
    --iter;
    --iter;
    auto cmd = dynamic_cast<Single*>(*iter);
    EXPECT_EQ(cmd->type_, Primitive::Type::cStroke);
    EXPECT_EQ(cmd->start_, std::make_pair(0.0, 0.0));
}
