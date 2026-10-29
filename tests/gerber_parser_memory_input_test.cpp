#include <gtest/gtest.h>
#include <vector>

#include "gerber_parser/gerber_parser.h"

TEST(GerberParserMemoryInputTest, ParsesMinimalGerberBuffer) {
  const std::vector<char> data{'M', '0', '2', '*'};
  GerberParser parser(data);
  EXPECT_NE(parser.GetGerber(), nullptr);
}
