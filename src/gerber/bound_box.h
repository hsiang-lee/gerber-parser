#pragma once
#include <utility>


class BoundBox {
	double left_;
	double bottom_;
	double right_;
	double top_;

public:
	BoundBox(double left, double right, double top, double bottom);

	double Left() const;
	double Right() const;
	double Top() const;
	double Bottom() const;

	std::pair<double, double> Center() const;

	void UpdateBox(double l, double r, double t, double b);

	void Scale(double times);

	bool operator == (const BoundBox& another) const;
};
