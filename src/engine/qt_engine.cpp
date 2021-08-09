#include "qt_engine.h"
#include <QPainter>
#include <QPixmap>


QtEngine::QtEngine(QPaintDevice* device, const BoundBox& bound_box, const BoundBox& offset) :
	pic_(device),
	trans_(bound_box.Scaled(kTimes), offset)
{
	trans_.SetPhysicalSize(pic_->width(), pic_->height());
}

void QtEngine::Resize()
{
	trans_.SetPhysicalSize(pic_->width(), pic_->height());
}

void QtEngine::Scale(double delta, double center_x, double center_y)
{
	if (trans_.Scale(delta, center_x, center_y)) {
		apertures_.clear();
		select_x_ = 0;
		select_y_ = 0;
	}
}

void QtEngine::Select(int x, int y)
{
	select_x_ = x;
	select_y_ = y;
}

void QtEngine::Move(int delta_x, int delta_y)
{
	trans_.Move(delta_x, delta_y);
}

void QtEngine::BeginRender() {
	painter_ = CreatePainter(pic_);
	painter_->fillRect(0, 0, pic_->width(), pic_->height(), QColor(255, 255, 255));
	painter_->setWindow(trans_.GetPainterWindow());
	painter_->setClipRect(painter_->window());
	painter_->setViewport(trans_.GetPainterViewport());
	painter_->setRenderHint(QPainter::Antialiasing);

	path_.setFillRule(Qt::FillRule::OddEvenFill);
	selected_ = painter_->combinedTransform().inverted().map(QPoint(select_x_, select_y_));
}

std::shared_ptr<QPainter> QtEngine::CreatePainter(QPaintDevice* pic)
{
	return std::make_shared<QPainter>(pic);
}

void QtEngine::EndRender() {
	painter_ = nullptr;
	copy_painter_ = nullptr;
}

void QtEngine::BeginDraw(bool negative) {
	if (copy_level_) {
		current_painter_ = copy_painter_;
	}
	else {
		current_painter_ = painter_;
	}

	negative_ = negative;
}

void QtEngine::EndDraw() {
	current_painter_ = nullptr;
}

void QtEngine::BeginOutline() {
	path_.clear();
}

void QtEngine::EndOutline() {
	if (negative_) {
		current_painter_->setBrush(QColor(255, 255, 255));
		current_painter_->setPen(QPen(QColor(255, 255, 255, 0), trans_.TranslatePenWidth(1)));
	}
	else {
		current_painter_->setBrush(QColor(255, 0, 0));
		current_painter_->setPen(QPen(QColor(255, 0, 0, 0), trans_.TranslatePenWidth(1)));
	}

	if (path_.contains(selected_)) {
		current_painter_->setBrush(QColor(0, 255, 0));
	}

	current_painter_->drawPath(path_);
	current_painter_->setBrush(QColor(255, 0, 0, 0));
	path_.clear();
}

void QtEngine::FillEvenOdd() {
}

void QtEngine::Stroke() {
	if (negative_) {
		current_painter_->setBrush(QColor(255, 255, 255, 0));
	}
	else {
		current_painter_->setBrush(QColor(255, 0, 0, 0));
	}

	if (path_.contains(selected_)) {
		auto pen = current_painter_->pen();
		pen.setColor(QColor(0, 255, 0));
		current_painter_->setPen(pen);
	}

	current_painter_->drawPath(path_);
	path_.clear();
}

void QtEngine::Close() {}

void QtEngine::DrawArcScaled(double x, double y, double degree) {
	if (fabs(path_.currentPosition().x() - x) < 1e-15 && fabs(path_.currentPosition().y() - y) < 1e-15) {
		return;
	}

	if (fabs(degree) > 45.001) { // Slightly large so that the circle has 4 segments
		degree /= 2.0;
		DrawArcScaled(x, y, degree);
		DrawArcScaled(x, y, degree);
		return;
	}

	const auto delta_x = path_.currentPosition().x() - x;
	const auto delta_y = path_.currentPosition().y() - y;
	double radius = sqrt(delta_y * delta_y + delta_x * delta_x);

	double t, t1, t2;
	double e, de, dt, rb, xb, yb;

	constexpr double kPi = 3.141592653589793;
	double b = degree * kPi / 180 / 2.0;

	double a1 = atan2(delta_y, delta_x);

	double a4 = a1 + degree * kPi / 180;
	double x4 = radius * cos(a4);
	double y4 = radius * sin(a4);

	double a5 = a1 + b;
	double x5 = radius * cos(a5);
	double y5 = radius * sin(a5);

	b = cos(b);
	double x6 = x5 / b;
	double y6 = y5 / b;

	// Best extimate
	t1 = 6 * x6 - 3 * delta_x - 3 * (x4);
	t2 = 6 * y6 - 3 * delta_y - 3 * (y4);
	if (fabs(t1) > fabs(t2)) {
		t = (8 * x5 - 4 * delta_x - 4 * (x4)) / t1;
	}
	else {
		t = (8 * y5 - 4 * delta_y - 4 * (y4)) / t2;
	}

	// Newton-Raphson to fit B(1/3) to the circle
	radius = radius * radius;

	dt = 1e-3;
	t2 = 1.0 / 3.0; // Fitting B(1/3) to the circle is close enough to the
	t1 = 1.0 - t2;  // optimum solution.

	double x2 = (1.0 - t) * delta_x + t * x6;
	double y2 = (1.0 - t) * delta_y + t * y6;
	double x3 = (1.0 - t) * (x4)+t * x6;
	double y3 = (1.0 - t) * (y4)+t * y6;
	xb = t1 * t1 * t1 * delta_x + 3.0 * t1 * t1 * t2 * (x2)+3.0 * t1 * t2 * t2 * (x3)+t2 * t2 * t2 * (x4);
	yb = t1 * t1 * t1 * delta_y + 3.0 * t1 * t1 * t2 * (y2)+3.0 * t1 * t2 * t2 * (y3)+t2 * t2 * t2 * (y4);
	rb = xb * xb + yb * yb;
	e = rb - radius;
	while (e > 1e-12) {
		x2 = (1.0 - (t + dt)) * delta_x + (t + dt) * x6;
		y2 = (1.0 - (t + dt)) * delta_y + (t + dt) * y6;
		x3 = (1.0 - (t + dt)) * (x4)+(t + dt) * x6;
		y3 = (1.0 - (t + dt)) * (y4)+(t + dt) * y6;
		xb = t1 * t1 * t1 * delta_x + 3.0 * t1 * t1 * t2 * (x2)+3.0 * t1 * t2 * t2 * (x3)+t2 * t2 * t2 * (x4);
		yb = t1 * t1 * t1 * delta_y + 3.0 * t1 * t1 * t2 * (y2)+3.0 * t1 * t2 * t2 * (y3)+t2 * t2 * t2 * (y4);
		rb = xb * xb + yb * yb;
		de = (rb - radius - e) / dt;

		t -= e / de; // Newton-Raphson

		x2 = (1.0 - t) * delta_x + t * x6;
		y2 = (1.0 - t) * delta_y + t * y6;
		x3 = (1.0 - t) * (x4)+t * x6;
		y3 = (1.0 - t) * (y4)+t * y6;
		xb = t1 * t1 * t1 * delta_x + 3.0 * t1 * t1 * t2 * (x2)+3.0 * t1 * t2 * t2 * (x3)+t2 * t2 * t2 * (x4);
		yb = t1 * t1 * t1 * delta_y + 3.0 * t1 * t1 * t2 * (y2)+3.0 * t1 * t2 * t2 * (y3)+t2 * t2 * t2 * (y4);
		rb = xb * xb + yb * yb;
		e = rb - radius;
	}

	path_.cubicTo(x2 + x, y2 + y, x3 + x, y3 + y, x4 + x, y4 + y);
}

void QtEngine::DrawArc(double x, double y, double degree) {
	x *= kTimes;
	y *= kTimes;
	DrawArcScaled(x, y, degree);
}

void QtEngine::DrawLine(double x, double y) {
	x *= kTimes;
	y *= kTimes;

	path_.lineTo(x, y);
}

void QtEngine::BeginSolidCircleLine(double x, double y, double line_width) {
	x *= kTimes;
	y *= kTimes;
	line_width *= kTimes;

	if (negative_) {
		auto pen = current_painter_->pen();
		pen.setColor(QColor(255, 255, 255));
		pen.setWidthF(line_width);
		current_painter_->setPen(pen);
	}
	else {
		auto pen = current_painter_->pen();
		pen.setColor(QColor(255, 0, 0));
		pen.setWidthF(line_width);
		current_painter_->setPen(pen);
	}

	path_.moveTo(x, y);
}

void QtEngine::BeginLine(double x, double y) {
	x *= kTimes;
	y *= kTimes;

	path_.moveTo(x, y);
}

void QtEngine::DrawCircle(double x, double y, double r) {
	x *= kTimes;
	y *= kTimes;
	r *= kTimes;

	path_.addEllipse(x - r, y - r, r * 2, r * 2);
}

void QtEngine::DrawRectangle(double x, double y, double w, double h) {
	x *= kTimes;
	y *= kTimes;
	w *= kTimes;
	h *= kTimes;

	path_.addRect(x, y, w, h);
}

void QtEngine::DrawRectLine(double x1, double y1, double x2, double y2, double w, double h) {
	x1 *= kTimes;
	y1 *= kTimes;
	x2 *= kTimes;
	y2 *= kTimes;
	w *= kTimes;
	h *= kTimes;


	w /= 2.0;
	h /= 2.0;

	if (x2 > x1) {
		if (y2 > y1) {
			path_.moveTo(x1 - w, y1 - h);
			path_.lineTo(x1 + w, y1 - h);
			path_.lineTo(x2 + w, y2 - h);
			path_.lineTo(x2 + w, y2 + h);
			path_.lineTo(x2 - w, y2 + h);
			path_.lineTo(x1 - w, y1 + h);

		}
		else if (y1 > y2) {
			path_.moveTo(x1 - w, y1 - h);
			path_.lineTo(x2 - w, y2 - h);
			path_.lineTo(x2 + w, y2 - h);
			path_.lineTo(x2 + w, y2 + h);
			path_.lineTo(x1 + w, y1 + h);
			path_.lineTo(x1 - w, y1 + h);

		}
		else { // y1 = y2
			path_.moveTo(x1 - w, y1 - h);
			path_.lineTo(x2 + w, y2 - h);
			path_.lineTo(x2 + w, y2 + h);
			path_.lineTo(x1 - w, y1 + h);
		}

	}
	else if (x1 > x2) {
		if (y2 > y1) {
			path_.moveTo(x2 - w, y2 - h);
			path_.lineTo(x1 - w, y1 - h);
			path_.lineTo(x1 + w, y1 - h);
			path_.lineTo(x1 + w, y1 + h);
			path_.lineTo(x2 + w, y2 + h);
			path_.lineTo(x2 - w, y2 + h);

		}
		else if (y1 > y2) {
			path_.moveTo(x2 - w, y2 - h);
			path_.lineTo(x2 + w, y2 - h);
			path_.lineTo(x1 + w, y1 - h);
			path_.lineTo(x1 + w, y1 + h);
			path_.lineTo(x1 - w, y1 + h);
			path_.lineTo(x2 - w, y2 + h);

		}
		else { // y1 = y2
			path_.moveTo(x2 - w, y2 - h);
			path_.lineTo(x1 + w, y1 - h);
			path_.lineTo(x1 + w, y1 + h);
			path_.lineTo(x2 - w, y2 + h);
		}

	}
	else { // x1 = x2
		if (y2 > y1) {
			path_.moveTo(x1 - w, y1 - h);
			path_.lineTo(x1 + w, y1 - h);
			path_.lineTo(x2 + w, y2 + h);
			path_.lineTo(x2 - w, y2 + h);

		}
		else { // y1 >= y2
			path_.moveTo(x2 - w, y2 - h);
			path_.lineTo(x2 + w, y2 - h);
			path_.lineTo(x1 + w, y1 + h);
			path_.lineTo(x1 - w, y1 + h);
		}
	}

	if (negative_) {
		current_painter_->setBrush(QColor(255, 255, 255));
		current_painter_->setPen(QPen(QColor(255, 255, 255, 0), trans_.TranslatePenWidth(1)));
	}
	else {
		current_painter_->setBrush(QColor(255, 0, 0));
		current_painter_->setPen(QPen(QColor(255, 0, 0, 0), trans_.TranslatePenWidth(1)));
	}

	if (path_.contains(selected_)) {
		current_painter_->setBrush(QColor(0, 255, 0));
	}
	current_painter_->drawPath(path_);
	path_.clear();
}

void QtEngine::ApertureErase(double left, double bottom, double top, double right) {
	left *= kTimes;
	bottom *= kTimes;
}

void QtEngine::ApertureFill() {
	if (negative_) {
		aperture_painter_->setBrush(QColor(255, 255, 255));
		aperture_painter_->setPen(QPen(QColor(255, 255, 255, 0), trans_.TranslatePenWidth(1)));
	}
	else {
		aperture_painter_->setBrush(QColor(255, 0, 0));
		aperture_painter_->setPen(QPen(QColor(255, 0, 0, 0), trans_.TranslatePenWidth(1)));
	}

	if (path_.contains(selected_)) {
		aperture_painter_->setBrush(QColor(0, 255, 0));
	}
	aperture_painter_->drawPath(path_);
	path_.clear();
}

void QtEngine::ApertureStroke() {
	ApertureFill();
}

void QtEngine::ApertureClose() {
	ApertureFill();
}

void QtEngine::DrawApertureArc(double x, double y, double angle) {
	DrawArc(x, y, angle);
}

void QtEngine::DrawApertureLine(double x, double y) {
	path_.lineTo(x * kTimes, y * kTimes);
}

void QtEngine::BeginApertureLine(double x, double y) {
	path_.moveTo(x * kTimes, y * kTimes);
}

void QtEngine::DrawAperatureCircle(double x, double y, double w) {
	path_.addEllipse((x - w / 2.0) * kTimes, (y - w / 2) * kTimes, w * kTimes, w * kTimes);
}

void QtEngine::DrawApertureRect(double x, double y, double w, double h) {
	x *= kTimes;
	y *= kTimes;
	w *= kTimes;
	h *= kTimes;

	path_.addRect(x, y, w, h);
}

void QtEngine::EndDrawAperture() {}

void QtEngine::PrepareDrawAperture() {
}

void QtEngine::PrepareCopyLayer(double left, double bottom, double right, double top) {
	left *= kTimes;
	right *= kTimes;
	top *= kTimes;
	bottom *= kTimes;

	copy_left_ = left;
	copy_top_ = top;
	copy_right_ = right;
	copy_bottom_ = bottom;

	copy_painter_ = nullptr;
	auto width = trans_.TranslateLogicCoord(right - left);
	auto height = trans_.TranslateLogicCoord(top - bottom);
	if (width < 1) {
		width = 1;
	}

	if (height < 1) {
		height = 1;
	}

	copy_level_ = std::make_shared<QPixmap>(width, height);
	copy_level_->fill(QColor(255, 255, 255, 0));
	copy_painter_ = std::make_shared<QPainter>(copy_level_.get());
	copy_painter_->setRenderHint(QPainter::Antialiasing);
	copy_painter_->setWindow(left, top, right - left, bottom - top);

	path_.clear();
}

void QtEngine::CopyLayer(int count_x, int count_y, double step_x, double step_y) {
	step_x *= kTimes;
	step_y *= kTimes;

	painter_->save();

	for (int y = 0; y < count_y; ++y) {
		for (int x = 0; x < count_x; ++x) {
			painter_->drawPixmap(QRectF(copy_left_, copy_bottom_, copy_right_ - copy_left_, copy_top_ - copy_bottom_), *copy_level_, copy_level_->rect());
			painter_->translate(step_x, 0);
		}

		painter_->translate(-step_x * count_x, step_y);
	}

	painter_->restore();
}

void QtEngine::Prepare2Render() {
	copy_painter_ = nullptr;
	copy_level_ = nullptr;
}

bool QtEngine::PrepareExistAperture(int code) {
	if (apertures_.find(code) != apertures_.end()) {
		aperture_left_ = apertures_[code].left_;
		aperture_right_ = apertures_[code].right;
		aperture_top_ = apertures_[code].top;
		aperture_bottom_ = apertures_[code].bottom;
		aperture_ = apertures_[code].pixmap;
		return true;
	}

	return false;
}

int QtEngine::Flash(double x, double y) {
	current_painter_->drawPixmap(
		QRectF(
			x * kTimes - (aperture_right_ - aperture_left_) / 2,
			y * kTimes - (aperture_top_ - aperture_bottom_) / 2,
			(aperture_right_ - aperture_left_),
			(aperture_top_ - aperture_bottom_)
		),
		*aperture_,
		aperture_->rect()
	);
	return 0;
}

void QtEngine::EndDrawNewAperture(int code) {
	Aperture aperture;
	aperture.left_ = aperture_left_;
	aperture.right = aperture_right_;
	aperture.top = aperture_top_;
	aperture.bottom = aperture_bottom_;
	aperture.pixmap = aperture_;

	apertures_[code] = aperture;

	aperture_painter_ = nullptr;
}

void QtEngine::NewAperture(double left, double bottom, double right, double top) {
	left *= kTimes;
	right *= kTimes;
	top *= kTimes;
	bottom *= kTimes;

	aperture_left_ = left;
	aperture_bottom_ = bottom;
	aperture_right_ = right;
	aperture_top_ = top;

	aperture_painter_ = nullptr;

	auto width = trans_.TranslateLogicCoord(right - left);
	auto height = trans_.TranslateLogicCoord(top - bottom);
	if (width < 1) {
		width = 1;
	}

	if (height < 1) {
		height = 1;
	}

	aperture_ = std::make_shared<QPixmap>(width, height);
	aperture_->fill(QColor(255, 255, 255, 0));
	aperture_painter_ = std::make_shared<QPainter>(aperture_.get());
	aperture_painter_->setRenderHint(QPainter::Antialiasing);
	aperture_painter_->setWindow(left, top, right - left, bottom - top);

	path_.clear();
}
