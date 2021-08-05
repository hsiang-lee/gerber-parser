#include "gerber_level.h"
#include "gerber_aperture.h"
#include "plotter.h"
#include <cassert>
#include <glog/logging.h>


constexpr double kPi = 3.141592653589793238463;


GerberLevel::GerberLevel(std::shared_ptr<GerberLevel> previous_level, GERBER_UNIT units) :
	bound_box_(1e3, -1e3, -1e3, 1e3) {
	plotter_ = std::make_unique<Plotter>(*this);

	CountX = CountY = 1;
	StepX = StepY = 0.0;

	negative_ = false;
	relative_ = false;
	incremental_ = false;
	multi_quadrant_ = false;

	X = Y = I = J = 0.0;

	this->units_ = units;
	exposure_ = geOff;
	interpolation_ = giLinear;

	if (previous_level) {
		SetName(previous_level->name_);

		exposure_ = previous_level->exposure_;
		plotter_->SetCurrentApeture(previous_level->plotter_->GetCurrentAperture());

		multi_quadrant_ = previous_level->multi_quadrant_;
		interpolation_ = previous_level->interpolation_;
		plotter_->SetPreX(previous_level->plotter_->GetPreX());
		plotter_->SetPreY(previous_level->plotter_->GetPreY());
		X = previous_level->X;
		Y = previous_level->Y;

		CountX = previous_level->CountX;
		CountY = previous_level->CountY;
		StepX = previous_level->StepX;
		StepY = previous_level->StepY;

		negative_ = previous_level->negative_;
		plotter_->SetInPath(previous_level->plotter_->IsInPath());
	}
}

GerberLevel::~GerberLevel() {
}

bool GerberLevel::IsCopyLayer()
{
	return CountX > 1 || CountY > 1;
}

double GerberLevel::GetRight() const
{
	if (CountX > 1) {
		return bound_box_.Left() + StepX * (CountX - 1) + bound_box_.Right() - bound_box_.Left();
	}

	return bound_box_.Right();
}

double GerberLevel::GetTop() const
{
	if (CountY > 1) {
		return bound_box_.Bottom() + StepY * (CountY - 1) + bound_box_.Top() - bound_box_.Bottom();
	}

	return bound_box_.Top();
}

void GerberLevel::SetName(const std::string& name) {
	name_ = name;
}

void GerberLevel::Add(std::shared_ptr<RenderCommand> command) {
	render_commands_.push_back(command);
}

std::shared_ptr<RenderCommand> GerberLevel::AddNew(RenderCommand::GerberCommand command) {
	auto tmp = std::make_shared<RenderCommand>(command);
	Add(tmp);
	return tmp;
}

void GerberLevel::ApertureSelect(std::shared_ptr<GerberAperture> aperture, unsigned line_number) {
	plotter_->ApertureSelect(aperture, line_number);
}

void GerberLevel::OutlineBegin(unsigned LineNumber) {
	plotter_->OutlineBegin(LineNumber);
}

void GerberLevel::OutlineEnd(unsigned LineNumber) {
	plotter_->OutlineEnd(LineNumber);
}

void GerberLevel::Do(unsigned line_number) {
	plotter_->Do(line_number);
}

std::vector<std::shared_ptr<RenderCommand>> GerberLevel::RenderCommands() const {
	return render_commands_;
}

void GerberLevel::Segment::Add(std::shared_ptr<RenderCommand> command_) {
	closed_ = false;
	command_list_.push_back(command_);
}

bool GerberLevel::Segment::IsClosed() {
	if (closed_) {
		return true;
	}

	if (command_list_.empty()) {
		return false;
	}

	assert(command_list_.front()->command_ == RenderCommand::gcBeginLine);
	assert(command_list_.back()->command_ == RenderCommand::gcLine || command_list_.back()->command_ == RenderCommand::gcArc);

	if (command_list_.back()->End.X == command_list_.front()->X &&
		command_list_.back()->End.Y == command_list_.front()->Y) {
		closed_ = true;
		return true;
	}

	return false;
}

void GerberLevel::Segment::Reverse() {
	if (IsClosed()) {
		return; // Don't bother reversing closed loops
	}

	if (command_list_.empty()) {
		return;
	}

	auto old_list = command_list_;
	command_list_.clear();

	auto begin_line = old_list.front();

	assert(begin_line->command_ == RenderCommand::gcBeginLine);

	double x = begin_line->X;
	double y = begin_line->Y;

	auto begin = old_list.begin();
	for (auto iter = ++begin; iter != old_list.end(); ++iter) {
		auto command = *iter;

		switch (command->command_) {
		case RenderCommand::gcLine:
			command->X = x;
			command->Y = y;
			std::swap(x, command->End.X);
			std::swap(y, command->End.Y);
			command_list_.push_front(command);
			break;

		case RenderCommand::gcArc:
			command->A *= -1;
			std::swap(x, command->End.X);
			std::swap(y, command->End.Y);
			command_list_.push_front(command);
			break;

		default:
			assert(false);
			break;
		}
	}

	begin_line->X = x;
	begin_line->Y = y;
	command_list_.push_front(begin_line);
}

void GerberLevel::Segment::Isolate() {
	if (prev_) {
		prev_->next_ = next_;
	}
	if (next_) {
		next_->prev_ = prev_;
	}

	next_ = prev_ = nullptr;
}


void GerberLevel::NewSegment() {
	Segment* tmp = new Segment;

	if (last_segment_) {
		last_segment_->next_ = tmp;
	}
	else {
		segment_list_ = tmp;
	}

	tmp->prev_ = last_segment_;
	last_segment_ = tmp;
}


void GerberLevel::ExtractSegments() {
	auto old_list = render_commands_;
	render_commands_.clear();

	bool is_outline = false;
	for (const auto& render : old_list) {
		switch (render->command_) {
		case RenderCommand::gcBeginLine:
			if (is_outline) {
				Add(render);
			}
			else {
				NewSegment();
				last_segment_->Add(render);
			}
			break;

		case RenderCommand::gcLine:
			if (is_outline) {
				Add(render);
			}
			else {
				assert(last_segment_, NewSegment());
				last_segment_->Add(render);
			}
			break;

		case RenderCommand::gcArc:
			if (is_outline) {
				Add(render);
			}
			else {
				assert(last_segment_, NewSegment());
				last_segment_->Add(render);
			}
			break;

		case RenderCommand::gcStroke:
			assert(is_outline == false);
			break;

		case RenderCommand::gcClose:
		case RenderCommand::gcFill:
			assert(is_outline == true);
			Add(render);
			break;

		case RenderCommand::gcBeginOutline:
			is_outline = true;
			Add(render);
			break;

		case RenderCommand::gcEndOutline:
			is_outline = false;
			Add(render);
			break;

		case RenderCommand::gcApertureSelect:
		case RenderCommand::gcFlash:
			break;

		case RenderCommand::gcRectangle:
		case RenderCommand::gcCircle:
		case RenderCommand::gcErase:
		default:
			assert(false);
			break;
		}
	}
}


GerberLevel::Segment* GerberLevel::FindNeighbour(Segment* current) {
	if (current->command_list_.empty()) {
		return nullptr;
	}

	if (current->IsClosed()) {
		return nullptr;
	}

	Segment* candidate = segment_list_;

	while (candidate) {
		if (candidate != current && !candidate->command_list_.empty() && !candidate->IsClosed()) {
			if (current->command_list_.back()->End.X == candidate->command_list_.front()->X &&
				current->command_list_.back()->End.Y == candidate->command_list_.front()->Y) {
				return candidate;
			}

			if (current->command_list_.back()->End.X == candidate->command_list_.back()->End.X &&
				current->command_list_.back()->End.Y == candidate->command_list_.back()->End.Y) {
				candidate->Reverse();
				return candidate;
			}
		}

		candidate = candidate->next_;
	}

	// No candidates found, but run the test again checking for near segments.
	// Points are considered the same if they are closer than 1 μm.
	// This is required because many Gerber generators make rounding errors.

	candidate = segment_list_;
	double dX, dY;

	while (candidate) {
		if (candidate != current && !candidate->command_list_.empty() && !candidate->IsClosed()) {
			dX = fabs(current->command_list_.back()->End.X - candidate->command_list_.front()->X);
			dY = fabs(current->command_list_.back()->End.Y - candidate->command_list_.front()->Y);
			if (dX < 1e-3 && dY < 1e-3) {
				if (gerber_warnings) {
					LOG(WARNING) << "Strokes2Fills - Warning: Joining segments that are close, but not coincident:";
				}
				return candidate;
			}
			dX = fabs(current->command_list_.back()->End.X - candidate->command_list_.back()->End.X);
			dY = fabs(current->command_list_.back()->End.Y - candidate->command_list_.back()->End.Y);
			if (dX < 1e-3 && dY < 1e-3) {
				candidate->Reverse();
				if (gerber_warnings) {
					printf(
						"Strokes2Fills - Warning: "
						"Joining segments that are close, but not coincident:\n"
						"    dX = %08.6lf mm (%07.5lf mil)\n"
						"    dY = %08.6lf mm (%07.5lf mil)\n",
						dX, dX / 25.4e-3,
						dY, dY / 25.4e-3
					);
				}
				return candidate;
			}
		}
		candidate = candidate->next_;
	}
	return 0;
}


void GerberLevel::JoinSegments() {
	auto current = segment_list_;

	while (current) {
		auto neighbour = FindNeighbour(current);

		if (neighbour) {
			if (segment_list_ == neighbour) {
				segment_list_ = segment_list_->next_;
			}

			if (last_segment_ == neighbour) {
				last_segment_ = last_segment_->prev_;
			}

			neighbour->Isolate();

			auto begin_line = neighbour->command_list_.front();
			neighbour->command_list_.pop_front();

			for (const auto& each : neighbour->command_list_) {
				current->command_list_.push_back(each);
			}
			delete neighbour;

		}
		else {
			current = current->next_;
		}
	}
}


void GerberLevel::AddSegments() {
	if (!segment_list_) {
		return;
	}

	AddNew(RenderCommand::gcBeginOutline);

	while (segment_list_) {
		assert(!segment_list_->command_list_.empty());
		assert(segment_list_->command_list_.front()->command_ == RenderCommand::gcBeginLine);

		for (const auto& each : segment_list_->command_list_) {
			render_commands_.push_back(each);
		}

		if (!segment_list_->IsClosed()) {
			AddNew(RenderCommand::gcClose);
		}

		auto tmp = segment_list_;
		segment_list_ = segment_list_->next_;
		tmp->Isolate();
		delete tmp;
	}
	last_segment_ = nullptr;

	AddNew(RenderCommand::gcFill);
	AddNew(RenderCommand::gcEndOutline);
}


void GerberLevel::ConvertStrokesToFills() {
	ExtractSegments();
	JoinSegments();
	AddSegments();
}

