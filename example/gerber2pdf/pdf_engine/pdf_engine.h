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

#ifndef Engine_h
#define Engine_h


#ifdef __linux__
#include <unistd.h>
#endif


#include <map>
#include <string>
#include <memory>


#include <engine.h>
#include "pdf/pdf.h"
#include "tools/color.h"


class GerberRender;
using pdfFormArray = std::map<int, pdfForm*>;

struct LAYER {
	std::string file_name;
	std::string title;

	bool negative_;
	bool convert_strokes2fills;
	double left_, bottom, right, top;

	Color light;

	pdfForm* form;

	LAYER();
	~LAYER();
};

class PdfEngine : public Engine {
public:
	void EndDraw() override;
	void BeginDraw(bool negative) override;

	void BeginOutline() override;
	void EndOutline() override;

	void FillEvenOdd() override;
	void Stroke() override;
	void Close() override;
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
	void BeginRender() override;
	void EndRender() override;

	int Flash(double x, double y) override;
	void EndDrawNewAperture(int code) override;
	void NewAperture(double left, double bottom, double right, double top) override;

public: // Externally-controlled mode
	enum PageSize {
		PS_Tight = 0,
		PS_Extents,
		PS_A3,
		PS_A4,
		PS_Letter
	} page_size_;

	bool mirror_;
	bool combine_;
	bool new_page_;

	Color light_;
	Color dark_;

	bool use_cmyk_;


private: // Internal structures
	std::list<pdfOpaque*> opaque_stack_;
	std::list<LAYER*> layers_; // Stack of layer XObjects
	LAYER* current_layer;

	// These stacks keep track of objects to be deleted at the end
	struct PAGE {
		pdfPage* Page;
		pdfContents* Contents;

		PAGE(bool use_cmyk);
		~PAGE();
	};
	std::list<PAGE*> pages_;

	std::list<pdfOutlineItems*> outline_;


private: // Internal State
	std::shared_ptr<Pdf> pdf_;

	bool   negative_;

	int     page_count_;
	double  the_page_left_;
	double  the_page_bottom_;
	double  the_page_right_;
	double  the_page_top_;

	// PDF Variables and Structures
	pdfPages    pdf_pages_;    // Single level page tree
	pdfOutlines outlines_; // Single level outline tree


	pdfContents* the_contents_; // Contents of ThePage


	std::list<pdfForm*> aperture_stack_;
	pdfForm* current_aperture_;
	pdfFormArray apertures_;
	std::list<pdfForm*> level_stack_;
	pdfForm* current_pdf_form_;

	void InitThePage();
	void AddNewOpaque();
	void AddOutline(const std::string& title);
	void CalculateThePageMediaBox(double x, double w2, double y, double h2);
	void UpdateNegativeContent(double x, double w2, double y, double h2, double w, double h);
	void UpdateContents();

private: // Functions
	void SetColour(bool negative);

	LAYER* NewLayer(
		const std::string& file_name,
		bool convert_strokes2fills,
		Color& light
	);

	std::shared_ptr<GerberRender> render_;

public: // Functions
	explicit PdfEngine();
	explicit PdfEngine(std::shared_ptr<Pdf> pdf);
	~PdfEngine();

	// Call Run for each Gerber file in the list, then call Finish once
	int  Run(const std::string& file_name, const std::string& title);
	void Finish(const std::string& output_file_name);
};


#endif

