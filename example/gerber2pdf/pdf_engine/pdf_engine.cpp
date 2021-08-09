//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of Gerber2PDF
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//==============================================================================

#include "pdf_engine/pdf_engine.h"
#include "gerber_render.h"


using namespace std;


LAYER::LAYER() : light(1.0, 1.0, 1.0, 0.0) {
	convert_strokes2fills = false;
	form = 0;
}


LAYER::~LAYER() {
	if (form) delete   form;
}


PdfEngine::PAGE::PAGE(bool use_cmyk) {
	this->Page = new pdfPage(use_cmyk);
	this->Contents = new pdfContents(use_cmyk);
}


PdfEngine::PAGE::~PAGE() {
	delete Page;
	delete Contents;
}


PdfEngine::PdfEngine() : PdfEngine(std::make_shared<Pdf>()) {

}

PdfEngine::PdfEngine(std::shared_ptr<Pdf> pdf) :
	render_(std::make_shared<GerberRender>(this)),
	pdf_(pdf),
	light_(1.0, 1.0, 1.0, 0.0),
	dark_(0.0, 0.0, 0.0, 1.0)
{
	convert_strokes2fills_ = false;
	use_cmyk_ = false;

	page_size_ = PS_Tight;

	mirror_ = false;
	negative_ = false;

	opaque_stack_.push_front(new pdfOpaque("Opaque"));

	page_count_ = 0;
	combine_ = false;
	new_page_ = true;
	the_page_left_ = 1e100;
	the_page_bottom_ = 1e100;
	the_page_right_ = -1e100;
	the_page_top_ = -1e100;

	the_contents_ = nullptr;

	opaque_stack_.front()->Opacity(1.0);
	pdf_->AddIndirect(opaque_stack_.front());
}


PdfEngine::~PdfEngine() {
	for (const auto each : pages_) {
		delete each;
	}

	for (const auto each : outline_) {
		delete each;
	}

	for (const auto each : opaque_stack_) {
		delete each;
	}

	for (const auto each : layers_) {
		delete each;
	}

	for (const auto each : aperture_stack_) {
		delete each;
	}

	for (const auto each : level_stack_) {
		delete each;
	}
}

void PdfEngine::SetColour(bool negative)
{
	if (negative != negative_) {
		current_pdf_form_->Opaque(opaque_stack_.front());
		if (light_.cmyk()) {
			current_pdf_form_->StrokeColour(light_.c(), light_.m(), light_.y(), light_.k());
			current_pdf_form_->FillColour(light_.c(), light_.m(), light_.y(), light_.k());
		}
		else {
			current_pdf_form_->StrokeColour(light_.r(), light_.g(), light_.b());
			current_pdf_form_->FillColour(light_.r(), light_.g(), light_.b());
		}
	}
}

LAYER* PdfEngine::NewLayer(
	const std::string& file_name,
	bool convert_strokes2fills,
	Color& light
) {
	static int gerber_count = 0;

	LAYER* layer = new LAYER;

	int j;
	for (j = 0; file_name[j]; j++);

	layer->file_name = new char[j + 1];

	for (j = 0; file_name[j]; j++)
		layer->file_name[j] = file_name[j];

	layer->file_name[j] = 0;
	layer->convert_strokes2fills = convert_strokes2fills;
	layer->light = light;

	string form_name;
	form_name.assign(1, 'G');
	form_name.append(to_string(++gerber_count));
	layer->form = new pdfForm(form_name.c_str(), use_cmyk_);
	layers_.push_front(layer);

	return layer;
}

int PdfEngine::Run(const std::string& file_name, const std::string& title) {
	current_layer = NewLayer(file_name, convert_strokes2fills_, light_);
	current_layer->title.assign(title);

	printf("\nInfo: Converting %s\n", file_name);

	auto gerber = std::make_shared<Gerber>(file_name);

	if (!gerber->Name().empty()) {
		current_layer->title.assign(gerber->Name());
	}
	else {
		int j = 0;
		for (j = current_layer->title.length() - 1; j >= 0 && current_layer->title[j] != '\\'; j--);
		current_layer->title.assign(current_layer->title.c_str() + j + 1);
	}

	current_layer->left_ = gerber->GetBBox().Left();
	current_layer->bottom = gerber->GetBBox().Bottom();
	current_layer->right = gerber->GetBBox().Right();
	current_layer->top = gerber->GetBBox().Top();
	current_layer->negative_ = gerber->IsNegative();
	current_layer->form->BBox.Set(current_layer->left_, current_layer->bottom, current_layer->right, current_layer->top);

	// Write the PDF
	bool page_added = false;
	if (pages_.empty() || new_page_ || !combine_) {
		auto page = new PAGE(use_cmyk_);
		pages_.push_front(page);
		page_added = true;
	}
	new_page_ = false;

	AddOutline(current_layer->title);

	if (page_added) {
		InitThePage();
	}

	double x = (current_layer->right + current_layer->left_) / 2.0;
	double y = (current_layer->top + current_layer->bottom) / 2.0;
	double w = 1.05 * (current_layer->right - current_layer->left_);
	double h = 1.05 * (current_layer->top - current_layer->bottom);
	double w2 = w / 2.0;
	double h2 = h / 2.0;

	CalculateThePageMediaBox(x, w2, y, h2);

	if (current_layer->negative_) {
		UpdateNegativeContent(x, w2, y, h2, w, h);
	}

	negative_ = current_layer->negative_;
	int ret = render_->RenderGerber(gerber);
	if (ret)
		return ret;

	pages_.front()->Page->Resources.AddForm(current_layer->form);
	pages_.front()->Page->Update();
	UpdateContents();

	convert_strokes2fills_ = false;

	return 0;
}

void PdfEngine::UpdateContents()
{
	if (dark_.cmyk()) {
		the_contents_->StrokeColour(dark_.c(), dark_.m(), dark_.y(), dark_.k());
		the_contents_->FillColour(dark_.c(), dark_.m(), dark_.y(), dark_.k());
	}
	else {
		the_contents_->StrokeColour(dark_.r(), dark_.g(), dark_.b());
		the_contents_->FillColour(dark_.r(), dark_.g(), dark_.b());
	}

	the_contents_->Push();
	if (dark_.a() == 1.0) {
		the_contents_->Opaque(opaque_stack_.front());
	}
	else {
		AddNewOpaque();
	}

	if (mirror_) {
		the_contents_->Scale(-1, 1);
	}

	the_contents_->Form(current_layer->form);
	the_contents_->Pop();
}

void PdfEngine::UpdateNegativeContent(double x, double w2, double y, double h2, double w, double h)
{
	the_contents_->Push();
	if (dark_.cmyk()) {
		the_contents_->StrokeColour(dark_.c(), dark_.m(), dark_.y(), dark_.k());
		the_contents_->FillColour(dark_.c(), dark_.m(), dark_.y(), dark_.k());
	}
	else {
		the_contents_->StrokeColour(dark_.r(), dark_.g(), dark_.b());
		the_contents_->FillColour(dark_.r(), dark_.g(), dark_.b());
	}

	if (dark_.a() == 1.0) {
		the_contents_->Opaque(opaque_stack_.front());
	}
	else {
		AddNewOpaque();
	}

	the_contents_->Rectangle(x - w2 - 1e6, y - h2 - 1e6, w + 2e6, h + 2e6);
	the_contents_->Fill();
	the_contents_->Pop();
}

void PdfEngine::AddNewOpaque()
{
	string name;
	name.assign(1, 'O'), name.append(to_string(opaque_stack_.size() + 1));
	auto opaque = new pdfOpaque(name.c_str());
	opaque->Opacity(dark_.a());

	pdf_->AddIndirect(opaque);
	pages_.front()->Page->Resources.AddOpaque(opaque);
	the_contents_->Opaque(opaque);

	opaque_stack_.push_front(opaque);
}

void PdfEngine::CalculateThePageMediaBox(double x, double w2, double y, double h2)
{
	if (mirror_) {
		if (the_page_left_ > -x - w2)
			the_page_left_ = -x - w2;

		if (the_page_right_ < -x + w2)
			the_page_right_ = -x + w2;
	}
	else {
		if (the_page_left_ > x - w2) the_page_left_ = x - w2;
		if (the_page_right_ < x + w2) the_page_right_ = x + w2;
	}

	if (the_page_bottom_ > y - h2)
		the_page_bottom_ = y - h2;

	if (the_page_top_ < y + h2)
		the_page_top_ = y + h2;

	pages_.front()->Page->MediaBox.Set_mm(
		the_page_left_,
		the_page_bottom_,
		the_page_right_,
		the_page_top_
	);
}

void PdfEngine::InitThePage()
{
	if (the_contents_)
		the_contents_->Deflate();

	the_contents_ = pages_.front()->Contents;
	pages_.front()->Page->Contents(the_contents_);
	pdf_->AddIndirect(pages_.front()->Page);
	pdf_->AddIndirect(the_contents_);
	pdf_pages_.AddChild(pages_.front()->Page);
	page_count_++;

	pages_.front()->Page->Resources.AddOpaque(opaque_stack_.front());

	the_contents_->LineCap(pdfContents::csRound);
	the_contents_->LineJoin(pdfContents::jsRound);
	the_contents_->Use_mm();

	the_page_left_ = 1e100;
	the_page_bottom_ = 1e100;
	the_page_right_ = -1e100;
	the_page_top_ = -1e100;

	if (light_.a() > 0.5) {
		the_contents_->Push();
		if (light_.cmyk()) {
			the_contents_->StrokeColour(light_.c(), light_.m(), light_.y(), light_.k());
			the_contents_->FillColour(light_.c(), light_.m(), light_.y(), light_.k());
		}
		else {
			the_contents_->StrokeColour(light_.r(), light_.g(), light_.b());
			the_contents_->FillColour(light_.r(), light_.g(), light_.b());
		}

		the_contents_->Opaque(opaque_stack_.front());

		the_contents_->Rectangle(-1e6, -1e6, 2e6, 2e6);
		the_contents_->Fill();
		the_contents_->Pop();
	}
}

void PdfEngine::AddOutline(const std::string& title)
{
	auto item = new pdfOutlineItems;
	item->Title.SetUnicode(title.c_str());
	item->DestFit(pages_.front()->Page);
	outlines_.AddChild(item);
	pdf_->AddIndirect(item);
	outline_.push_front(item);
}

void PdfEngine::Finish(const std::string& outpu_file_name) {
	if (the_contents_) the_contents_->Deflate();

	if (page_size_ != PS_Tight) {
		double left_ = 1e100;
		double bottom = 1e100;
		double right = -1e100;
		double top = -1e100;
		double width;
		double height;
		double delta;

		for (const auto page : pages_) {
			if (page->Page->MediaBox.left_.Value < page->Page->MediaBox.right_.Value &&
				page->Page->MediaBox.bottom_.Value < page->Page->MediaBox.top_.Value) {
				if (left_ > page->Page->MediaBox.left_.Value)
					left_ = page->Page->MediaBox.left_.Value;
				if (bottom > page->Page->MediaBox.bottom_.Value)
					bottom = page->Page->MediaBox.bottom_.Value;
				if (right < page->Page->MediaBox.right_.Value)
					right = page->Page->MediaBox.right_.Value;
				if (top < page->Page->MediaBox.top_.Value)
					top = page->Page->MediaBox.top_.Value;
			}
		}

		width = right - left_;
		height = top - bottom;

		// Centre on standard sizes
		if (page_size_ == PS_A3) {
			if (width > height) {
				width = 420 / 25.4 * 72.0;
				height = 297 / 25.4 * 72.0;
			}
			else {
				width = 297 / 25.4 * 72.0;
				height = 420 / 25.4 * 72.0;
			}
		}
		else if (page_size_ == PS_A4) {
			if (width > height) {
				width = 297 / 25.4 * 72.0;
				height = 210 / 25.4 * 72.0;
			}
			else {
				width = 210 / 25.4 * 72.0;
				height = 297 / 25.4 * 72.0;
			}
		}
		else if (page_size_ == PS_Letter) {
			if (width > height) {
				width = 11.0 * 72.0;
				height = 8.5 * 72.0;
			}
			else {
				width = 8.5 * 72.0;
				height = 11.0 * 72.0;
			}
		}

		delta = (width - (right - left_)) / 2.0;
		left_ -= delta;
		right += delta;

		delta = (height - (top - bottom)) / 2.0;
		bottom -= delta;
		top += delta;

		for (const auto page : pages_) {
			page->Page->MediaBox.Set(left_, bottom, right, top);
		}
	}

	if (page_count_) {
		pdf_->AddIndirect(&pdf_pages_);
		pdf_->AddIndirect(&outlines_);

		pdf_->Catalogue.SetPages(&pdf_pages_);
		pdf_->Catalogue.SetOutlines(&outlines_);

		pdf_->Author.Set("gerberrender");

		printf("\nInfo: Writing %s\n", outpu_file_name.c_str());
		pdf_->WritePDF(outpu_file_name);
	}
	else {
		printf("Info: No pages to write\n");
	}
}


void PdfEngine::PrepareCopyLayer(double left, double bottom, double right, double top)
{
	std::string level_name;
	level_name.assign(1, 'L');
	level_name.append(std::to_string(level_stack_.size() + 1));
	current_pdf_form_ = new pdfForm(level_name.c_str(), use_cmyk_);

	current_pdf_form_->BBox.Set(
		left,
		bottom,
		right,
		top
	);

	current_pdf_form_->Resources.AddOpaque(opaque_stack_.front());
	apertures_.clear();
}

void PdfEngine::CopyLayer(int count_x, int count_y, double step_x, double step_y)
{
	current_layer->form->Push();

	for (int y = 0; y < count_y; y++) {
		for (int x = 0; x < count_x; x++) {
			current_layer->form->Form(current_pdf_form_);
			current_layer->form->Translate(step_x, 0.0);
		}

		current_layer->form->Translate(step_x * -count_x, step_y);
	}

	current_layer->form->Pop();

	current_layer->form->Resources.AddForm(current_pdf_form_);
	pdf_->AddIndirect(current_pdf_form_);

	current_pdf_form_->Deflate();
	current_pdf_form_->Update();

	level_stack_.push_front(current_pdf_form_);
}

void PdfEngine::Prepare2Render()
{
	current_layer->form->Resources.AddOpaque(opaque_stack_.front());
	current_pdf_form_ = current_layer->form;
}

void PdfEngine::EndDraw()
{
	current_pdf_form_->Pop();
}

void PdfEngine::BeginDraw(bool negative)
{
	current_pdf_form_->Push();
	SetColour(negative);
}

void PdfEngine::BeginOutline()
{
}

void PdfEngine::EndOutline()
{
}

void PdfEngine::FillEvenOdd()
{
	current_pdf_form_->FillEvenOdd();
}

void PdfEngine::Stroke()
{
	current_pdf_form_->Stroke();
}

void PdfEngine::Close()
{
	current_pdf_form_->Close();
}

void PdfEngine::DrawArc(double x, double y, double degree)
{
	current_pdf_form_->Arc(x, y, degree);
}

void PdfEngine::DrawLine(double x, double y)
{
	current_pdf_form_->Line(x, y);
}

void PdfEngine::BeginSolidCircleLine(double x, double y, double line_width)
{
	current_pdf_form_->LineWidth(line_width);
	current_pdf_form_->BeginLine(x, y);
}

void PdfEngine::BeginLine(double x, double y)
{
	current_pdf_form_->BeginLine(x, y);
}

void PdfEngine::DrawCircle(double x, double y, double r)
{
	current_pdf_form_->Circle(x, y, r);
}

void PdfEngine::DrawRectangle(double x, double y, double w, double h)
{
	current_pdf_form_->Rectangle(x, y, w, h);
}

void PdfEngine::DrawRectLine(
	double x1, double y1, // Start
	double x2, double y2, // End
	double w, double h   // Rect Width; Height
) {
	w /= 2.0;
	h /= 2.0;

	if (x2 > x1) {
		if (y2 > y1) {
			current_pdf_form_->BeginLine(x1 - w, y1 - h);
			current_pdf_form_->Line(x1 + w, y1 - h);
			current_pdf_form_->Line(x2 + w, y2 - h);
			current_pdf_form_->Line(x2 + w, y2 + h);
			current_pdf_form_->Line(x2 - w, y2 + h);
			current_pdf_form_->Line(x1 - w, y1 + h);

		}
		else if (y1 > y2) {
			current_pdf_form_->BeginLine(x1 - w, y1 - h);
			current_pdf_form_->Line(x2 - w, y2 - h);
			current_pdf_form_->Line(x2 + w, y2 - h);
			current_pdf_form_->Line(x2 + w, y2 + h);
			current_pdf_form_->Line(x1 + w, y1 + h);
			current_pdf_form_->Line(x1 - w, y1 + h);

		}
		else { // y1 = y2
			current_pdf_form_->BeginLine(x1 - w, y1 - h);
			current_pdf_form_->Line(x2 + w, y2 - h);
			current_pdf_form_->Line(x2 + w, y2 + h);
			current_pdf_form_->Line(x1 - w, y1 + h);
		}

	}
	else if (x1 > x2) {
		if (y2 > y1) {
			current_pdf_form_->BeginLine(x2 - w, y2 - h);
			current_pdf_form_->Line(x1 - w, y1 - h);
			current_pdf_form_->Line(x1 + w, y1 - h);
			current_pdf_form_->Line(x1 + w, y1 + h);
			current_pdf_form_->Line(x2 + w, y2 + h);
			current_pdf_form_->Line(x2 - w, y2 + h);

		}
		else if (y1 > y2) {
			current_pdf_form_->BeginLine(x2 - w, y2 - h);
			current_pdf_form_->Line(x2 + w, y2 - h);
			current_pdf_form_->Line(x1 + w, y1 - h);
			current_pdf_form_->Line(x1 + w, y1 + h);
			current_pdf_form_->Line(x1 - w, y1 + h);
			current_pdf_form_->Line(x2 - w, y2 + h);

		}
		else { // y1 = y2
			current_pdf_form_->BeginLine(x2 - w, y2 - h);
			current_pdf_form_->Line(x1 + w, y1 - h);
			current_pdf_form_->Line(x1 + w, y1 + h);
			current_pdf_form_->Line(x2 - w, y2 + h);
		}

	}
	else { // x1 = x2
		if (y2 > y1) {
			current_pdf_form_->BeginLine(x1 - w, y1 - h);
			current_pdf_form_->Line(x1 + w, y1 - h);
			current_pdf_form_->Line(x2 + w, y2 + h);
			current_pdf_form_->Line(x2 - w, y2 + h);

		}
		else { // y1 >= y2
			current_pdf_form_->BeginLine(x2 - w, y2 - h);
			current_pdf_form_->Line(x2 + w, y2 - h);
			current_pdf_form_->Line(x1 + w, y1 + h);
			current_pdf_form_->Line(x1 - w, y1 + h);
		}
	}

	current_pdf_form_->Close();
	current_pdf_form_->Fill();
}

void PdfEngine::ApertureErase(double left, double bottom, double top, double right)
{
	current_aperture_->BeginLine(left, bottom);
	current_aperture_->Line(left, top);
	current_aperture_->Line(right, top);
	current_aperture_->Line(right, bottom);
	current_aperture_->Close();
	current_aperture_->Clip();
}

void PdfEngine::ApertureFill()
{
	current_aperture_->Fill();
}

void PdfEngine::ApertureStroke()
{
	current_aperture_->Stroke();
}

void PdfEngine::ApertureClose()
{
	current_aperture_->Close();
}

void PdfEngine::DrawApertureArc(double x, double y, double angle)
{
	current_aperture_->Arc(x, y, angle);
}

void PdfEngine::DrawApertureLine(double x, double y)
{
	current_aperture_->Line(x, y);
}

void PdfEngine::BeginApertureLine(double x, double y)
{
	current_aperture_->BeginLine(x, y);
}

void PdfEngine::DrawAperatureCircle(double x, double y, double w)
{
	current_aperture_->Circle(x, y, w / 2.0);
}

void PdfEngine::DrawApertureRect(double x, double y, double w, double h)
{
	current_aperture_->Rectangle(x, y, w, h);
}

void PdfEngine::EndDrawAperture()
{
	current_aperture_->Pop();

	current_aperture_->Deflate();
}

void PdfEngine::PrepareDrawAperture()
{
	current_aperture_->Push();
	current_aperture_->LineCap(pdfContents::csRound);
	current_aperture_->LineJoin(pdfContents::jsRound);
}

int PdfEngine::Flash(double x, double y)
{
	if (current_aperture_) {
		current_pdf_form_->Push();
		current_pdf_form_->Translate(x, y);
		current_pdf_form_->Form(current_aperture_);
		current_pdf_form_->Pop();
	}
	else {
		printf("Error: No Aperture selected\n");
		current_pdf_form_->Pop();
		return 4;
	}

	return 0;
}

void PdfEngine::EndDrawNewAperture(int code)
{
	current_pdf_form_->Resources.AddForm(current_aperture_);
	pdf_->AddIndirect(current_aperture_);
	apertures_[code] = current_aperture_;
	aperture_stack_.push_front(current_aperture_);
}

void PdfEngine::NewAperture(double left, double bottom, double right, double top)
{
	std::string str;
	str.assign(1, 'D');
	str.append(std::to_string(aperture_stack_.size() + 1));
	current_aperture_ = new pdfForm(str.c_str(), use_cmyk_);
	current_aperture_->BBox.Set(left, bottom, right, top);
	current_aperture_->Update();
}

bool PdfEngine::PrepareExistAperture(int code)
{
	auto aperture_form = apertures_.find(code);
	if (aperture_form != apertures_.end()) {
		current_aperture_ = aperture_form->second;
		return true;
	}

	return false;
}

void PdfEngine::BeginRender()
{
	apertures_.clear();
}

void PdfEngine::EndRender()
{
	current_layer->form->Update();
	current_layer->form->Deflate();
	pdf_->AddIndirect(current_layer->form);
}
