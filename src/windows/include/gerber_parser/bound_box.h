#pragma once
#include <utility>


class GerberApi BoundBox {
	double left_;
	double bottom_;
	double right_;
	double top_;

public:
	BoundBox(double left = 1.0e8, double right = -1.0e8, double top = -1.0e8, double bottom = 1.0e8);

	double Left() const;
	double Right() const;
	double Top() const;
	double Bottom() const;

	std::pair<double, double> Center() const;
	double Width() const;
	double Height() const;

	void UpdateBox(double l, double r, double t, double b);
	void UpdateBox(const BoundBox& box);

	void Scale(double times);
	BoundBox Scaled(double times) const;

	bool operator == (const BoundBox& another) const;
};
