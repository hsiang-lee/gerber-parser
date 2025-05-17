#include "bound_box.h"

constexpr double kTwo = 2.0;

BoundBox::BoundBox(double left, double right, double top, double bottom) : left_(left),
                                                                 right_(right),
                                                                 top_(top),
                                                                 bottom_(bottom)
{
}

double BoundBox::Left() const
{
    return left_;
}

double BoundBox::Right() const
{
    return right_;
}

double BoundBox::Top() const
{
    return top_;
}

double BoundBox::Bottom() const
{
    return bottom_;
}

std::pair<double, double> BoundBox::Center() const
{
    return {(left_ + right_) / kTwo, (top_ + bottom_) / kTwo};
}

double BoundBox::Width() const
{
    return Right() - Left();
}

double BoundBox::Height() const
{
    return Top() - Bottom();
}

void BoundBox::Update(double l, double r, double t, double b)
{
    if (left_ > l)
    {
        left_ = l;
    }

    if (bottom_ > b)
    {
        bottom_ = b;
    }

    if (right_ < r)
    {
        right_ = r;
    }

    if (top_ < t)
    {
        top_ = t;
    }
}

void BoundBox::Update(const BoundBox &box)
{
    Update(box.left_, box.right_, box.top_, box.bottom_);
}

void BoundBox::Scale(double times)
{
    left_ *= times;
    right_ *= times;
    top_ *= times;
    bottom_ *= times;
}

BoundBox BoundBox::Scaled(double times) const
{
    BoundBox tmp = *this;
    tmp.Scale(times);
    return tmp;
}

bool BoundBox::operator==(const BoundBox &another) const
{
    return left_ == another.left_ &&
           right_ == another.right_ &&
           top_ == another.top_ &&
           bottom_ == another.bottom_;
}
