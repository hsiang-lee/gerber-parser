#pragma once
#include <map>
#include <memory>
#include "transformation.h"

#include <QPainterPath>

class Aperture;
class Gerber;
class GerberLevel;
class RenderCommand;

class BoundBox;
class QPainter;
class QPixmap;
class QPaintDevice;

class QtEngine {
public:
	QtEngine(QPaintDevice* device, const BoundBox& bound_box, const BoundBox& offset);

	void Resize();
	void Scale(double delta, double center_x = 0.0, double center_y = 0.0);
	void Move(int delta_x, int delta_y);
	void SetConvertStroke2Fills(bool value);

	int RenderGerber(const std::shared_ptr<Gerber>&);

	void AddAperture(int code);

	void BeginRender();
	void EndRender();

	void BeginDraw(std::shared_ptr<GerberLevel> level);
	void EndDraw(std::shared_ptr<GerberLevel> level);
	void CopyLayer(std::shared_ptr<GerberLevel> level);

	void PreparePenAndBrush(bool negative);
	void PreparePenAndBrushForStroke(bool negative, double line_width);

	virtual std::shared_ptr<QPainter> CreatePainter(QPaintDevice* pic);

private:
	QPaintDevice* pic_;
	std::shared_ptr<QPainter> painter_;

	std::shared_ptr<QPainter> aperture_painter_;

	std::shared_ptr<QPixmap> copy_level_;
	std::shared_ptr<QPainter> copy_painter_;

	std::shared_ptr<QPainter> current_painter_;

	static constexpr int kTimes = 10000;

	Transformation trans_;

	struct ApertureImg
	{
		ApertureImg(std::shared_ptr<Aperture> aperture, std::shared_ptr<QPixmap> pixmap) :
			aperture_(aperture), pixmap_(pixmap) {

		}

		ApertureImg() = default;

		std::shared_ptr<Aperture> aperture_;
		std::shared_ptr<QPixmap> pixmap_;
	};
	std::map<int, ApertureImg> aperture_imgs_;
	ApertureImg aperture_img_;

	friend class GerberLevel;

	bool outline_path_{ false };
	bool convert_strokes2fills_{ false };

	double rect_x_{ 0.0 };
	double rect_y_{ 0.0 };

	uint32_t count_{ 0 };
};
