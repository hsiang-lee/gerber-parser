#pragma once
#include <memory>

class Gerber;
class QString;
class BoundBox;

void ExportGerber(std::shared_ptr<Gerber> gerber, const BoundBox& box, int img_w, int img_h, int pixel_w, int pixel_h);
