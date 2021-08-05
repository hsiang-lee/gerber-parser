//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of a library
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//==============================================================================

#ifndef Calculator_h
#define Calculator_h


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <map>
#include <string>


class CALCULATOR {
public:
	enum MEASURE {
		Degrees = 1,
		Radians = 0
	};


private:
	std::map<std::string, long double> Constants;

	enum OPERATION {
		Fact, Power, Multiply, Divide, Remainder,
		Add, Subtract, Log, Log2, Ln,
		Abs, Round, Fix, Floor, Ceil, Rand,
		Sin, ASin, Cos, ACos, Tan, ATan,
		Sec, ASec, Cosec, ACosec, Cot, ACot,
		Sinh, ASinh, Cosh, ACosh, Tanh, ATanh,
		Sech, ASech, Cosech, ACosech, Coth, ACoth,
		Condition, Greater, Less, Equal, GreaterEqual, LessEqual,
		NotEqual, Not,
		And, Or, Xor,
		bAnd, bOr, bXor, bNot,
		Var, Val
	};

	struct NODE {
		OPERATION   Operation;
		long double Value;
		std::string Name;
		NODE* left_;
		NODE* Right;
		NODE* Other;

		NODE();
	};

	NODE* Tree;
	char* buffer_;
	int   Index;

	NODE* NewNode(NODE* Root = 0);
	NODE* CopyNode(NODE* Root);

	void FuncName(std::string* Name);

	bool ConditionOp(NODE* Root);
	bool LogicOp(NODE* Root);
	bool AddOp(NODE* Root);
	bool MulOp(NODE* Root);
	bool PowerOp(NODE* Root);
	bool Function(NODE* Root);
	bool Factorial(NODE* Root);
	bool Exponent(NODE* Root);
	bool Value(NODE* Root);
	bool Float(long double* f);

	void        ViewTree(NODE* Root, std::string* Result);
	long double CalcTree(NODE* Root, const char* Variable, long double Value);
	void        DeleteTree(NODE* Root);
	bool        Simplify(NODE* Root);
	void        Diff(NODE* Root, const char* Variable);


public:
	MEASURE Measure;
	CALCULATOR();
	~CALCULATOR();

	void        BuildTree(const char* Formula);
	long double CalculateTree(const char* Variable = "", long double Value = 0.);
	void        ShowTree(std::string* Result);
	long double Calculate(const char* Formula, const char* Variable = "",
		long double  Value = 0.);
};


#endif

