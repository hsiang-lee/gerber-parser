#pragma once
#include <memory>

class OperatorItem {
public:
	enum Type {
		kAdd,
		kSubtract,
		kMultiply,
		kDivide,

		kVariable,
		kLiteral
	};

	union {
		int Index; // Variable Index
		double Value; // Literal Value
	};

	OperatorItem(const std::shared_ptr<OperatorItem>& left, const std::shared_ptr<OperatorItem>& right, Type opera);
	virtual ~OperatorItem();

	Type Opera() const;
	std::shared_ptr<OperatorItem> Left();
	std::shared_ptr<OperatorItem> Right();

private:
	Type operator_;
	std::shared_ptr<OperatorItem> left_;
	std::shared_ptr<OperatorItem> right_;
};
