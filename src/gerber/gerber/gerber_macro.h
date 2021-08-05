#pragma once

#include <memory>
#include <string>
#include <vector>
#include "gerber_command.h"


class RenderCommand;
class GerberMacro {
private:
	enum PRIMITIVE {
		pComment = 0,
		pCircle = 1,
		pLineVector = 2,
		pLineVector2 = 20,
		pLineCenter = 21,
		pLineLowerLeft = 22,
		pEndOfFile = 3,
		pOutline = 4,
		pPolygon = 5,
		pMoire = 6,
		pThermal = 7,
		pAssignment = -1,
	};

	enum OPERATOR {
		opAdd,
		opSubtract,
		opMultiply,
		opDivide,

		opVariable,
		opLiteral
	};

	struct OPERATOR_ITEM {
		OPERATOR       Operator;
		OPERATOR_ITEM* left_;
		OPERATOR_ITEM* right_;

		union {
			int    Index; // Variable Index
			double Value; // Literal Value
		};

		OPERATOR_ITEM();
		~OPERATOR_ITEM();
	};

	struct PRIMITIVE_ITEM {
		PRIMITIVE       Primitive;
		OPERATOR_ITEM** Modifier; // Modifier Tree for each modifier in the array
		int             ModifierCount;
		int             Index;    // Used for assignment primitives

		PRIMITIVE_ITEM* Next;

		PRIMITIVE_ITEM();
		~PRIMITIVE_ITEM();
	};
	PRIMITIVE_ITEM* Primitives;
	PRIMITIVE_ITEM* PrimitivesLast;
	void Add(PRIMITIVE_ITEM* Primitive);

	std::vector<std::shared_ptr<RenderCommand>> render_commands_;

	void Add(std::shared_ptr<RenderCommand> Render);

	double Evaluate(OPERATOR_ITEM* Root);

	void RenderLine(
		double x1, double y1,
		double x2, double y2,
		double x3, double y3,
		double x4, double y4,
		double xR, double yR, // Rotation Center
		double A
	);

	bool RenderCircle(PRIMITIVE_ITEM* Primitive);
	bool RenderLineVector(PRIMITIVE_ITEM* Primitive);
	bool RenderLineCenter(PRIMITIVE_ITEM* Primitive);
	bool RenderLineLowerLeft(PRIMITIVE_ITEM* Primitive);
	bool RenderOutline(PRIMITIVE_ITEM* Primitive);
	bool RenderPolygon(PRIMITIVE_ITEM* Primitive);
	bool RenderMoire(PRIMITIVE_ITEM* Primitive);
	bool RenderThermal(PRIMITIVE_ITEM* Primitive);
	bool RenderAssignment(PRIMITIVE_ITEM* Primitive);

	double* Modifiers;
	int     ModifierCount;
	bool    NewModifiers;
	bool    exposure_;

	std::string Buffer;
	unsigned Length;
	unsigned Index;
	bool Inches;

	double Get_mm(double Number);

	bool Float(double* Number);
	bool Integer(int* Integer);
	void SkipWhiteSpace();

	OPERATOR_ITEM* Modifier();
	OPERATOR_ITEM* Term();
	OPERATOR_ITEM* Factor();
	OPERATOR_ITEM* Variable();

	bool Primitive();
	bool Comment();
	bool Circle();
	bool Line_Vector();
	bool Line_Center();
	bool Line_LowerLeft();
	bool Outline();
	bool Polygon();
	bool Moire();
	bool Thermal();
	bool Assignment();

public:
	GerberMacro();
	~GerberMacro();

	std::string Name;

	// Modifiers is a null-terminated array
	// Returns a new list of render commands => The user must free the memory
	std::vector<std::shared_ptr<RenderCommand>> Render(double* Modifiers, int ModifierCount);

	bool LoadMacro(const char* buffer, unsigned Length, bool Inches);
};
