#include "gerber_render.h"
#include "engine/engine.h"

#include <glog/logging.h>


GerberRender::GerberRender(Engine* engine) :
	engine_(engine)
{
}

GerberRender::~GerberRender() {
}


int GerberRender::RenderLayer(std::shared_ptr<GerberLevel> level) {
	engine_->BeginDraw(level->negative_);

	if (engine_->convert_strokes2fills_) {
		level->ConvertStrokesToFills();
	}

	auto renders = level->RenderCommands();
	for (const auto& render : renders) {
		if (auto ret = Draw(render)) {
			engine_->EndDraw();
			return ret;
		}
	}

	engine_->EndDraw();
	return 0;
}

int GerberRender::Draw(std::shared_ptr<RenderCommand> render)
{
	switch (render->command_) {
	case RenderCommand::gcRectangle:
		engine_->DrawRectangle(render->X, render->Y, render->W, render->H);
		break;

	case RenderCommand::gcCircle:
		engine_->DrawCircle(render->X, render->Y, render->W / 2.0);
		break;

	case RenderCommand::gcBeginLine:
		if (outline_path_) {
			engine_->BeginLine(render->X, render->Y);
		}
		else if (solid_circle_) {
			engine_->BeginSolidCircleLine(render->X, render->Y, line_width_);
		}
		else if (solid_rectangle_) {
			rect_x_ = render->X;
			rect_y_ = render->Y;
		}
		else {
			LOG(ERROR) << "Error: Only solid circular or rectangular apertures can be used for paths";
			return 1;
		}
		break;

	case RenderCommand::gcLine:
		if (outline_path_ || solid_circle_) {
			engine_->DrawLine(render->X, render->Y);
		}
		else if (solid_rectangle_) {
			engine_->DrawRectLine(
				rect_x_, rect_y_,
				render->X, render->Y,
				rect_w_, rect_h_
			);
			rect_x_ = render->X;
			rect_y_ = render->Y;

		}
		else {
			LOG(ERROR) << "Error: Only solid circular or rectangular apertures can be used for paths";
			return 2;
		}
		break;

	case RenderCommand::gcArc:
		if (outline_path_ || solid_circle_) {
			engine_->DrawArc(render->X, render->Y, render->A);
		}
		else {
			LOG(ERROR) << "Error: Only solid circular apertures can be used for arcs";
			return 3;
		}
		break;

	case RenderCommand::gcFlash:
		if (auto ret = engine_->Flash(render->X, render->Y))
			return ret;

		break;

	case RenderCommand::gcClose:
		engine_->Close();
		break;

	case RenderCommand::gcStroke:
		engine_->Stroke();
		break;

	case RenderCommand::gcFill:
		engine_->FillEvenOdd();
		break;

	case RenderCommand::gcBeginOutline:
		engine_->BeginOutline();
		outline_path_ = true;
		break;

	case RenderCommand::gcEndOutline:
		engine_->EndOutline();
		outline_path_ = false;
		break;

	case RenderCommand::gcApertureSelect: {
		auto aperture = render->aperture_;
		if (!aperture) {
			LOG(ERROR) << "Error: Null Aperture";
			return 5;
		}

		solid_circle_ = aperture->SolidCircle();
		line_width_ = aperture->right_ - aperture->left_;

		solid_rectangle_ = aperture->SolidRectangle();
		rect_w_ = aperture->right_ - aperture->left_;
		rect_h_ = aperture->top_ - aperture->bottom_;

		if (!engine_->PrepareExistAperture(aperture->code_)) {
			engine_->NewAperture(aperture->left_, aperture->bottom_, aperture->right_, aperture->top_);
			DrawAperture(aperture->Render(), aperture->left_, aperture->bottom_, aperture->right_, aperture->top_);
			engine_->EndDrawNewAperture(aperture->code_);
		}

		break;
	}
	default:
		break;
	}
	return {};
}

void GerberRender::DrawAperture(
	std::vector<std::shared_ptr<RenderCommand>> renders,
	double left,
	double bottom,
	double right,
	double top
) {
	struct OBJECT {
		std::vector<std::shared_ptr<RenderCommand>> commands;
	};
	std::list<OBJECT*> objects;

	auto object = new OBJECT;
	objects.push_front(object);
	for (auto render : renders) {
		object->commands.push_back(render);

		if (
			render->command_ == RenderCommand::gcStroke ||
			render->command_ == RenderCommand::gcFill ||
			render->command_ == RenderCommand::gcErase
			) {
			object = new OBJECT;
			objects.push_front(object);
		}
	}

	engine_->PrepareDrawAperture();

	for (const auto object : objects) {
		for (auto render : object->commands) {
			switch (render->command_) {
			case RenderCommand::gcRectangle:
				engine_->DrawApertureRect(render->X, render->Y, render->W, render->H);
				break;

			case RenderCommand::gcCircle:
				engine_->DrawAperatureCircle(render->X, render->Y, render->W);
				break;

			case RenderCommand::gcBeginLine:
				engine_->BeginApertureLine(render->X, render->Y);
				break;

			case RenderCommand::gcLine:
				engine_->DrawApertureLine(render->X, render->Y);
				break;

			case RenderCommand::gcArc:
				engine_->DrawApertureArc(render->X, render->Y, render->A);
				break;

			case RenderCommand::gcClose:
				engine_->ApertureClose();
				break;

			case RenderCommand::gcStroke:
				engine_->ApertureStroke();
				break;

			case RenderCommand::gcFill:
				engine_->ApertureFill();
				break;

			case RenderCommand::gcErase:
				engine_->ApertureErase(left, bottom, top, right);
				break;

			default:
				LOG(ERROR) << "Error: Unrecognised Aperture Render Command " << render->command_;
				break;
			}
		}
		delete object;
	}

	engine_->EndDrawAperture();
}

int GerberRender::RenderGerber(std::shared_ptr<Gerber> gerber)
{
	engine_->BeginRender();

	auto levels = gerber->Levels();
	for (const auto& level : levels) {
		if (auto ret = RenderLevel(level)) {
			return ret;
		}
	}

	engine_->EndRender();

	return 0;
}

int GerberRender::RenderLevel(std::shared_ptr<GerberLevel> level)
{
	if (level->IsCopyLayer()) {
		engine_->PrepareCopyLayer(level->bound_box_.Left(), level->bound_box_.Bottom(), level->bound_box_.Right(), level->bound_box_.Top());
	}
	else {
		engine_->Prepare2Render();
	}

	auto result = RenderLayer(level);
	if (result) {
		return result;
	}

	if (level->IsCopyLayer()) {
		engine_->CopyLayer(level->CountX, level->CountY, level->StepX, level->StepY);
	}

	return 0;
}
