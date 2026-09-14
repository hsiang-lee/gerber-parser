#pragma once

// 共享测试辅助（批级质量遍提取：parse 夹具的样板此前在 blend2d_engine_test.cpp
// 与 gerber_renderer_test.cpp 重复出现，统一为单一入口，后续向代码级测试迁移时
// 复用/替换点收敛于此）。

#include <memory>
#include <string>

#include "gerber/gerber.h"
#include "gerber_parser/gerber_parser.h"

inline std::shared_ptr<Gerber> ParseTestGerberFile(const std::string &name) {
  auto parser = std::make_shared<GerberParser>(std::string(TestData) +
                                               "gerber_files/" + name);
  return parser->GetGerber();
}
