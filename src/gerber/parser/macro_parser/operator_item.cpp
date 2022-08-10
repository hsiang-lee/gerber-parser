#include "operator_item.h"


OperatorItem::OperatorItem(const std::shared_ptr<OperatorItem>& left, const std::shared_ptr<OperatorItem>& right, Type opera) :
	left_(left),
	right_(right),
	operator_(opera)
{
	Index = 0;
	Value = 0;
}

OperatorItem::~OperatorItem() {
}

OperatorItem::Type OperatorItem::Opera() const {
	return operator_;
}

std::shared_ptr<OperatorItem> OperatorItem::Left() {
	return left_;
}

std::shared_ptr<OperatorItem> OperatorItem::Right() {
	return right_;
}
