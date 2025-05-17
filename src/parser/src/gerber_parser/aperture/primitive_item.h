#pragma once
#include <vector>
#include <memory>

class GerberMacro;
class OperatorItem;

class PrimitiveItem {
public:
	enum Type {
		kComment = 0,
		kCircle = 1,
		kLineVector = 2,
		kLineVector2 = 20,
		kLineCenter = 21,
		kLineLowerLeft = 22,
		kEndOfFile = 3,
		kOutline = 4,
		kPolygon = 5,
		kMoire = 6,
		kThermal = 7,
		kAssignment = -1,
	};

	std::vector<std::shared_ptr<OperatorItem>> modifiers_;
	int index_;

	PrimitiveItem();
	~PrimitiveItem();

	virtual bool Render(GerberMacro& macro) = 0;

protected:
	double Evaluate(const std::shared_ptr<OperatorItem>& root, const std::vector<double>& modifiers);
    void RenderLine(
        double x1, double y1,
        double x2, double y2,
        double x3, double y3,
        double x4, double y4,
        double x_r, double y_r,
        double angle, GerberMacro& macro
    );

    Type type_;
	static constexpr double kPi = 3.141592653589793238463;
};

class CommentPrimitiveItem : public PrimitiveItem {
public:
    CommentPrimitiveItem() {
        type_ = kComment;
    }
    
    bool Render(GerberMacro& macro) override;
};

class CirclePrimitiveItem : public PrimitiveItem {
public:
    CirclePrimitiveItem() {
        type_ = kCircle;
    }
    
    bool Render(GerberMacro& macro) override;
};

class LineVectorPrimitiveItem : public PrimitiveItem {
public:
    LineVectorPrimitiveItem() {
        type_ = kLineVector;
    }
    
    bool Render(GerberMacro& macro) override;
};

class LineCenterPrimitiveItem : public PrimitiveItem {
public:
    LineCenterPrimitiveItem() {
        type_ = kLineCenter;
    }
    
    bool Render(GerberMacro& macro) override;
};

class LineLowerLeftPrimitiveItem : public PrimitiveItem {
public:
    LineLowerLeftPrimitiveItem() {
        type_ = kLineLowerLeft;
    }
    
    bool Render(GerberMacro& macro) override;
};

class EndOfFilePrimitiveItem : public PrimitiveItem {
public:
    EndOfFilePrimitiveItem() {
        type_ = kEndOfFile;
    }
    
    bool Render(GerberMacro& macro) override;
};

class OutlinePrimitiveItem : public PrimitiveItem {
public:
    OutlinePrimitiveItem() {
        type_ = kOutline;
    }
    
    bool Render(GerberMacro& macro) override;
};

class PolygonPrimitiveItem : public PrimitiveItem {
public:
    PolygonPrimitiveItem() {
        type_ = kPolygon;
    }
    
    bool Render(GerberMacro& macro) override;
};

class MoirePrimitiveItem : public PrimitiveItem {
public:
    MoirePrimitiveItem() {
        type_ = kMoire;
    }
    
    bool Render(GerberMacro& macro) override;
};

class ThermalPrimitiveItem : public PrimitiveItem {
public:
    ThermalPrimitiveItem() {
        type_ = kThermal;
    }
    
    bool Render(GerberMacro& macro) override;
};

class AssignmentPrimitiveItem : public PrimitiveItem {
public:
    AssignmentPrimitiveItem() {
        type_ = kAssignment;
    }
    
    bool Render(GerberMacro& macro) override;
};
