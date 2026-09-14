#include <blend2d.h>

#include <iostream>
#include <memory>
#include <string>

#include "gerber_parser/bound_box.h"
#include "gerber/gerber.h"
#include "gerber_parser/gerber_parser.h"
#include "engines/blend2d_engine.h"

#include <gflags/gflags.h>

DEFINE_string(gerber_file, "", "The path of gerber file you want to export.");
DEFINE_double(um_pixel, 5, "How much um/pixel.Default value is 5um/pixel");

int main(int argc, char *argv[])
{
	gflags::SetUsageMessage("Usage: gerber2image --gerber_file=\"path/to/gerber/file\" --um_pixel=5");
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	if (FLAGS_gerber_file.empty()) {
          std::cout << "Usage: gerber2image --gerber_file=\"path/to/gerber/file\" --um_pixel=5" << std::endl;
          return 0;
	}

	try {
          auto parser = std::make_shared<GerberParser>(FLAGS_gerber_file);
          auto gerber = parser->GetGerber();
          const auto width_pixel = 2560;
          const auto height_pixel = int(double(gerber->GetBBox().Height()) /
                                        gerber->GetBBox().Width() * 2560);

          // 注：本机 blend2d 枚举成员为 C 风格 BL_FORMAT_PRGB32（计划中
          //     BLFormat::PRGB32 为 API 适配偏差，语义等价，同测试代码）。
          BLImage image(static_cast<int>(width_pixel * 1.05),
                        static_cast<int>(height_pixel * 1.05),
                        BL_FORMAT_PRGB32);
          Blend2DEngine engine(image, gerber->GetBBox(), 0.05);
          engine.DrawBackground();  // 显式黑底，确保 PNG 输出确定
          const int render_result = engine.RenderGerber(gerber);
          if (render_result != 0) {
            std::cerr << "RenderGerber failed with code " << render_result
                      << std::endl;
            return 1;
          }
          const std::string image_file = FLAGS_gerber_file + ".png";
          const BLResult write_result = image.writeToFile(image_file.c_str());
          if (write_result != BL_SUCCESS) {
            std::cerr << "Failed to write PNG file: " << image_file
                      << " (result=" << write_result << ")" << std::endl;
            return 1;
          }
    }
    catch (const std::exception& e) {
          std::cerr << e.what() << std::endl;
          return 1;
    }

	return 0;
}