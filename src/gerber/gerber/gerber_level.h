#pragma once

#include <string>
#include <vector>
#include <list>
#include <memory>

#include "gerber_enums.h"
#include "gerber_command.h"
#include "bound_box.h"


extern bool gerber_warnings;

class GerberAperture;
class Plotter;

class GerberLevel {
private: // Standard private members and functions
	std::vector<std::shared_ptr<RenderCommand>> render_commands_;

	void Add(std::shared_ptr<RenderCommand> command);
	std::shared_ptr<RenderCommand> AddNew(RenderCommand::GerberCommand command);

private: // Specifically used by ConvertStrokesToFills()

	class Segment {
	private:
		bool closed_ = false; // true => closed; false => maybe closed

	public:
		std::list<std::shared_ptr<RenderCommand>> command_list_;
		Segment* prev_ = nullptr;
		Segment* next_ = nullptr;

		void Add(std::shared_ptr<RenderCommand> command_);
		bool IsClosed();
		void Reverse();
		void Isolate();
	};

	Segment* segment_list_ = nullptr;
	Segment* last_segment_ = nullptr;

	Segment* FindNeighbour(Segment* Current);

	void NewSegment();
	void ExtractSegments();
	void JoinSegments();
	void AddSegments();

	std::unique_ptr<Plotter> plotter_;
	friend class Plotter;

public: // Public interface
	GerberLevel(std::shared_ptr<GerberLevel> PreviousLevel, GERBER_UNIT Units);
	~GerberLevel();

	double GetRight() const;
	double GetTop() const;

	// Image bounding box
	BoundBox bound_box_;

	// Step-and-Repeat
	int    CountX, CountY;
	double StepX, StepY;

	void SetName(const std::string& name);

	std::string name_; // null for default level
	bool  negative_;
	bool  relative_;
	bool  incremental_;
	bool  multi_quadrant_;

	double X, Y, I, J;

	GERBER_UNIT          units_;
	GERBER_EXPOSURE      exposure_;
	GERBER_INTERPOLATION interpolation_;

	void ApertureSelect(std::shared_ptr<GerberAperture> aperture_, unsigned LineNumber);
	void OutlineBegin(unsigned LineNumber);
	void OutlineEnd(unsigned LineNumber);
	void Do(unsigned LineNumber);

	// Linked list of render commands
	// Memory freed automatically
	std::vector<std::shared_ptr<RenderCommand>> RenderCommands() const;

	// Forms a single area out of the various line and arc segments in the 
	// layer, typically used to obtain a solid board from an outline.
	void ConvertStrokesToFills();

	bool IsCopyLayer();
};
