#pragma once
#include <map>
#include <memory>
#include <QPainterPath>
#include "engine.h"
#include "transformation.h"

class QPainter;
class QPixmap;
class QPaintDevice;

class QtEngine : public Engine {
public:
	QtEngine(QPaintDevice* device, const BoundBox& bound_box, const BoundBox& offset);

	void Resize();
	void Scale(double delta, double center_x = 0.0, double center_y = 0.0);
	void Select(int x, int y);
	void Move(int delta_x, int delta_y);

protected:
	void BeginRender() override;
	void EndRender() override;
	void BeginDraw(bool negative) override;
	void EndDraw() override;
	void BeginOutline() override;
	void EndOutline() override;
	void FillEvenOdd() override;
	void Stroke() override;
	void Close() override;
	void DrawArcScaled(double x, double y, double degree);
	void DrawArc(double x, double y, double degree) override;
	void DrawLine(double x, double y) override;
	void BeginSolidCircleLine(double x, double y, double line_width) override;
	void BeginLine(double x, double y) override;
	void DrawCircle(double x, double y, double r) override;
	void DrawRectangle(double x, double y, double w, double h) override;
	void DrawRectLine(
		double x1, double y1, // Start
		double x2, double y2, // End
		double w, double h   // Rect Width; Height
	) override;
	void ApertureErase(double left, double bottom, double top, double right) override;
	void ApertureFill() override;
	void ApertureStroke() override;
	void ApertureClose() override;
	void DrawApertureArc(double x, double y, double angle) override;
	void DrawApertureLine(double x, double y) override;
	void BeginApertureLine(double x, double y) override;
	void DrawAperatureCircle(double x, double y, double w) override;
	void DrawApertureRect(double x, double y, double w, double h) override;
	void EndDrawAperture() override;
	void PrepareDrawAperture() override;
	void PrepareCopyLayer(double left, double bottom, double right, double top) override;
	void CopyLayer(int count_x, int count_y, double step_x, double step_y) override;
	void Prepare2Render() override;
	bool PrepareExistAperture(int code) override;
	int Flash(double x, double y) override;
	void EndDrawNewAperture(int code) override;
	void NewAperture(double left, double bottom, double right, double top) override;

	virtual std::shared_ptr<QPainter> CreatePainter(QPaintDevice* pic);

private:
	QPaintDevice* pic_;
	std::shared_ptr<QPainter> painter_;

	std::shared_ptr<QPixmap> aperture_;
	std::shared_ptr<QPainter> aperture_painter_;

	std::shared_ptr<QPixmap> copy_level_;
	std::shared_ptr<QPainter> copy_painter_;

	std::shared_ptr<QPainter> current_painter_;

	QPainterPath path_;

	double aperture_left_;
	double aperture_top_;
	double aperture_right_;
	double aperture_bottom_;

	double copy_left_;
	double copy_top_;
	double copy_right_;
	double copy_bottom_;

	static constexpr int kTimes = 10000;

	Transformation trans_;

	int select_x_{ 0 };
	int select_y_{ 0 };
	QPointF selected_;

	struct Aperture
	{
		double left_;
		double top;
		double right;
		double bottom;

		std::shared_ptr<QPixmap> pixmap;
	};
	std::map<int, Aperture> apertures_;

	bool negative_{ false };
};
