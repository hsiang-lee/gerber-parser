#include "qt_engine.h"
#include "gerber/gerber_level.h"

#include <QPainter>



QtEngine::QtEngine(QPaintDevice* device, const BoundBox& bound_box, const BoundBox& offset) :
	pic_(device),
	trans_(bound_box.Scaled(kTimes), offset), aperture_img_(nullptr, nullptr)
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
		aperture_imgs_.clear();
	}
}

void QtEngine::Move(int delta_x, int delta_y)
{
	trans_.Move(delta_x, delta_y);
}

void QtEngine::SetConvertStroke2Fills(bool value)
{
	convert_strokes2fills_ = value;
}

void QtEngine::AddAperture(int code)
{
	aperture_imgs_[code] = aperture_img_;
}

void QtEngine::BeginRender() {
	painter_ = CreatePainter(pic_);
	painter_->fillRect(0, 0, pic_->width(), pic_->height(), QColor(255, 255, 255));
	painter_->setWindow(trans_.GetPainterWindow());
	painter_->setClipRect(painter_->window());
	painter_->setViewport(trans_.GetPainterViewport());
}

std::shared_ptr<QPainter> QtEngine::CreatePainter(QPaintDevice* pic)
{
	return std::make_shared<QPainter>(pic);
}

void QtEngine::EndRender() {
	painter_ = nullptr;
	copy_painter_ = nullptr;
	count_ = 0;
}

int QtEngine::RenderGerber(const std::shared_ptr<Gerber>& gerber)
{
	BeginRender();

	auto levels = gerber->Levels();
	for (const auto& level : levels) {
		BeginDraw(level);

		int ret = 0;
		if (level->GetPrimitives().empty()) {
			ret = level->Render(this);
		}

		EndDraw(level);
		if (level->IsCopyLayer()) {
			CopyLayer(level);
		}

		if (ret) {
			return ret;
		}

		count_ += 155;
	}

	EndRender();

	return 0;
}

void QtEngine::PreparePenAndBrush(bool negative) {
	current_painter_->setPen(QPen(QColor(0, 0, 0, 0), 1.0));

	if (negative) {
		current_painter_->setBrush(QColor(255, 255, 255));
	}
	else {
		current_painter_->setBrush(QColor(count_ % 256, (count_ + 53) % 256, (count_ + 125) % 256, 200));
	}
}

void QtEngine::PreparePenAndBrushForStroke(bool negative, double line_width) {
	current_painter_->setBrush(QColor(0, 0, 0, 0));

	auto pen = current_painter_->pen();
	pen.setColor(QColor(count_ % 256, (count_ + 53) % 256, (count_ + 125) % 256, 200));
	pen.setWidthF(line_width);
	current_painter_->setPen(pen);
}

void QtEngine::BeginDraw(std::shared_ptr<GerberLevel> level) {
	if (level->IsCopyLayer()) {
		copy_painter_ = nullptr;
		auto width = trans_.TranslateLogicCoord(level->GetBoundBox().Width() * kTimes);
		auto height = trans_.TranslateLogicCoord(level->GetBoundBox().Height() * kTimes);
		if (width < 1) {
			width = 1;
		}

		if (height < 1) {
			height = 1;
		}

		copy_level_ = std::make_shared<QPixmap>(width, height);
		copy_level_->fill(QColor(255, 255, 255, 0));
		copy_painter_ = std::make_shared<QPainter>(copy_level_.get());
		copy_painter_->setWindow(level->GetBoundBox().Left() * kTimes, level->GetBoundBox().Bottom() * kTimes,
			level->GetBoundBox().Width() * kTimes, level->GetBoundBox().Height() * kTimes);

		current_painter_ = copy_painter_;
	}
	else {
		copy_painter_ = nullptr;
		copy_level_ = nullptr;
		current_painter_ = painter_;
	}

	level->NewPrimitive(Primitive::kNormal, 1.0);
}

void QtEngine::EndDraw(std::shared_ptr<GerberLevel> level) {
	for (const auto& each : level->GetPrimitives()) {
		if (each->type_ == Primitive::kNormal) {
			PreparePenAndBrush(level->IsNegative());
			current_painter_->drawPath(each->GetPath());
			current_painter_->setBrush(QColor(0, 0, 0, 0));
		}
		else if (each->type_ == Primitive::kStroke) {
			PreparePenAndBrushForStroke(level->IsNegative(), each->line_width_);
			current_painter_->drawPath(each->GetPath());
		}
		else if (each->type_ == Primitive::kRectLine) {
			PreparePenAndBrush(level->IsNegative());
			current_painter_->drawPath(each->GetPath());
		}
        else if (each->type_ == Primitive::kApertureMacro) {
            current_painter_->drawPixmap(each->dst_, each->pic_, each->src_);
        }
	}

	current_painter_ = nullptr;
}

void QtEngine::CopyLayer(std::shared_ptr<GerberLevel> level) {
	const auto step_x = kTimes * level->step_x_;
	const auto step_y = kTimes * level->step_y_;

	painter_->save();

	const auto copy_left = level->GetBoundBox().Left() * kTimes;
	const auto copy_bottom = level->GetBoundBox().Bottom() * kTimes;
	const auto copy_width = level->GetBoundBox().Width() * kTimes;
	const auto copy_height = level->GetBoundBox().Height() * kTimes;

	for (int y = 0; y < level->count_y_; ++y) {
		for (int x = 0; x < level->count_x_; ++x) {
			painter_->drawPixmap(QRectF(copy_left, copy_bottom, copy_width, copy_height),
				*copy_level_, copy_level_->rect());
			painter_->translate(step_x, 0);
		}

		painter_->translate(-step_x * level->count_x_, step_y);
	}

	painter_->restore();
}
