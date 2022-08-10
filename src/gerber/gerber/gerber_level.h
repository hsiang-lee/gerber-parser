#pragma once

#include <list>
#include <vector>
#include <memory>
#include <string>

#include <QPainterPath>

#include "bound_box.h"
#include "gerber_enums.h"
#include "gerber/gerber.h"
#include "gerber_command.h"


class Plotter;
class Aperture;
class  QtEngine;

class QPainterPath;


class ApertureLevel;

class Primitive {
public:
	enum Type {
		kNormal,
		kStroke,
		kRectLine
	};

	Primitive() {
		path_.setFillRule(Qt::FillRule::OddEvenFill);
	}

	QPainterPath GetPath() {
		return path_;
	}

	QPainterPath path_;
	Type type_;
	double line_width_;
};


class StrokesToFillsConverter {
	class Segment {
	private:
		bool closed_ = false; // true => closed; false => maybe closed

	public:
		std::list<std::shared_ptr<RenderCommand>> command_list_;
		std::shared_ptr<Segment> prev_{ nullptr };
		std::shared_ptr<Segment> next_{ nullptr };

		void Add(const std::shared_ptr<RenderCommand>& command);
		bool IsClosed();
		void Reverse();
		void Isolate();
	};

public:
	StrokesToFillsConverter(GerberLevel& level);

	void NewSegment();
	void ExtractSegments();
	void JoinSegments();
	void AddSegments();
	std::shared_ptr<Segment> FindNeighbour(const std::shared_ptr<Segment>& current);

private:
	GerberLevel& level_;

	std::shared_ptr<Segment> segment_list_ = nullptr;
	std::shared_ptr<Segment> last_segment_ = nullptr;

};

class GerberLevel {
private:
	std::vector<std::shared_ptr<RenderCommand>> render_commands_{};

	void Add(const std::shared_ptr<RenderCommand>& command);

	friend class Plotter;
	friend class StrokesToFillsConverter;

	std::string name_;
	bool negative_;
	BoundBox bound_box_{ 0.0, 0.0, 0.0, 0.0 };
	UnitType::Type unit_{ UnitType::guMillimeters };

	std::shared_ptr<Primitive> primitive_;
	std::vector<std::shared_ptr<Primitive>> primitives_;
	QtEngine* engine_;

	int Draw(const std::shared_ptr<RenderCommand>& render);
	void EndDrawNewAperture(int code, const std::vector<std::unique_ptr<Primitive>>&);
	void BeginSolidCircleLine(double x, double y);

	bool PrepareExistAperture(int code);
	void NewAperture(std::shared_ptr<Aperture>);

	int Flash(double x, double y);

	void BeginOutline();
	void EndOutline();
	void FillEvenOdd();
	void Stroke();
	void Close();
	void DrawArcScaled(double x, double y, double degree);
	void DrawArc(double x, double y, double degree);
	void DrawLine(double x, double y);
	void BeginLine(double x, double y);
	void DrawCircle(double x, double y, double r);
	void DrawRectangle(double x, double y, double w, double h);
	void DrawRectLine(
		double x1, double y1,
		double x2, double y2,
		double w, double h
	);

	void ConvertStrokesToFills();

public:
	static constexpr int kTimes = 10000;

	GerberLevel(const std::shared_ptr<GerberLevel>& previous_level, UnitType::Type units);
	~GerberLevel();

	double GetRight() const;
	double GetTop() const;

	UnitType::Type GetUnit() const;
	void SetUnit(UnitType::Type type);

	// Step-and-Repeat
	int count_x_, count_y_;
	double step_x_, step_y_;
	bool IsCopyLayer() const;

	void NewPrimitive(Primitive::Type type, double line_width);
	std::vector<std::shared_ptr<Primitive>> GetPrimitives() const;

	std::string GetName() const;
	void SetName(const std::string& name);

	bool IsNegative() const;
	void SetNegative(bool);

	BoundBox GetBoundBox() const;

	std::vector<std::shared_ptr<RenderCommand>> RenderCommands() const;

	int Render(QtEngine* engine);
};
