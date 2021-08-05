//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of a library
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//==============================================================================

#include "Calculator.h"


using namespace std;


#define e  2.718281828459045235360L
#define pi 3.141592653589793238463L


void operator+= (string& S, double d) {
	char s[0x100]; sprintf(s, "%0.12lg", d); S += s;
}


void operator+= (string& S, int i) {
	char s[0x100]; sprintf(s, "%d", i); S += s;
}


void operator+= (string& S, void* p) {
	char s[0x100]; sprintf(s, "%p", p); S += s;
}


long double fix(long double x) {
	if (x < 0.0) return ceill(x);
	else        return floorl(x);
}


CALCULATOR::CALCULATOR() {
	Tree = 0;
	Measure = Radians;

	Constants["e"] = e;
	Constants["pi"] = pi;
	Constants["\\c"] = 2.99792458e8L;
	Constants["\\G"] = 6.67259e-11L;
	Constants["\\g"] = 9.80665L;
	Constants["\\me"] = 9.1093897e-31L;
	Constants["\\mp"] = 1.6726231e-27L;
	Constants["\\mn"] = 1.6749286e-27L;
	Constants["\\u"] = 1.6605402e-27L;
	Constants["\\e"] = 1.60217646e-19L;
	Constants["\\h"] = 6.6260689633e-34L;
	Constants["\\k"] = 1.380650424e-23L;
	Constants["\\mu"] = pi * 4e-7L;
	Constants["\\epsilon"] = 8.854187817e-12L;
	Constants["\\Phi"] = 2.06783461e-15L;
	Constants["\\NA"] = 6.0221417930e23L;
	Constants["\\R"] = 8.31447215L;
	Constants["\\rhoAl"] = 26.53e-9L;
	Constants["\\rhoCu"] = 16.78e-9L;
	Constants["\\F"] = 9.6485304e4L;
	Constants["\\AU"] = 1.4959787e11L;
	Constants["\\pc"] = 3.0856776e16L;
	Constants["\\mil"] = 25.4e-6L;
}


CALCULATOR::~CALCULATOR() {
	DeleteTree(Tree);
}


CALCULATOR::NODE::NODE() {
	Operation = Val;
	Value = 0.0;
	left_ = 0;
	Right = 0;
	Other = 0;
}


void CALCULATOR::DeleteTree(NODE* Root) {
	if (Root) {
		DeleteTree(Root->left_);
		DeleteTree(Root->Right);
		DeleteTree(Root->Other);
		delete Root;
	}
}


CALCULATOR::NODE* CALCULATOR::CopyNode(NODE* Root) {
	NODE* N = new NODE;
	if (Root) {
		N->Operation = Root->Operation;
		N->Value = Root->Value;
		N->left_ = CopyNode(Root->left_);
		N->Right = CopyNode(Root->Right);
		N->Other = CopyNode(Root->Other);
		N->Name = (Root->Name);
		return N;
	}
	else {
		return 0;
	}
}


CALCULATOR::NODE* CALCULATOR::NewNode(NODE* Root) {
	NODE* N = new NODE;
	if (Root) {
		N->Operation = Root->Operation;
		N->Value = Root->Value;
		N->left_ = Root->left_;
		N->Right = Root->Right;
		N->Other = Root->Other;
		N->Name = Root->Name;
	}
	return N;
}


bool CALCULATOR::ConditionOp(NODE* Root) {
	if (buffer_[Index] == '[') {
		Index++;
		Root->Operation = Condition;
		Root->left_ = NewNode();
		Root->Right = NewNode();
		if (!LogicOp(Root->left_)) return false;
		if (buffer_[Index++] != ']') return false;
		if (!LogicOp(Root->Right)) return false;
		if (buffer_[Index] == 0) {
			return true;
		}
		else {
			Root->Other = NewNode();
			return ConditionOp(Root->Other);
		}
	}
	else {
		return LogicOp(Root);
	}
}


bool CALCULATOR::LogicOp(NODE* Root) {
	NODE* N;

	if (AddOp(Root)) {
		while (buffer_[Index]) {
			switch (buffer_[Index]) {
			case '>':
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				if (buffer_[Index + 1] == '=') {
					Root->Operation = GreaterEqual;
					Index += 2;
				}
				else {
					Root->Operation = Greater;
					Index += 1;
				}
				if (!AddOp(Root->Right)) return false;
				break;
			case '<':
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				if (buffer_[Index + 1] == '=') {
					Root->Operation = LessEqual;
					Index += 2;
				}
				else {
					Root->Operation = Less;
					Index += 1;
				}
				if (!AddOp(Root->Right)) return false;
				break;
			case '~':
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				if (buffer_[Index + 1] == '=') {
					Root->Operation = NotEqual;
					Index += 2;
				}
				else {
					return false;
				}
				if (!AddOp(Root->Right)) return false;
				break;
			case '=':
				Index++;
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				Root->Operation = Equal;
				if (!AddOp(Root->Right)) return false;
				break;
			case '&':
				Index++;
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				Root->Operation = And;
				if (!AddOp(Root->Right)) return false;
				break;
			case '|':
				Index++;
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				Root->Operation = Or;
				if (!AddOp(Root->Right)) return false;
				break;
			case ':':
				Index++;
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				Root->Operation = Xor;
				if (!AddOp(Root->Right)) return false;
				break;
			case 'a':
				if ((buffer_[Index + 1] == 'n') && (buffer_[Index + 2] == 'd')) {
					Index += 3;
					N = NewNode(Root);
					Root->left_ = N;
					Root->Right = NewNode();
					Root->Operation = bAnd;
					if (!AddOp(Root->Right)) return false;
				}
				else {
					return true;
				}
				break;
			case 'o':
				if (buffer_[Index + 1] == 'r') {
					Index += 2;
					N = NewNode(Root);
					Root->left_ = N;
					Root->Right = NewNode();
					Root->Operation = bOr;
					if (!AddOp(Root->Right)) return false;
				}
				else {
					return true;
				}
				break;
			case 'x':
				if ((buffer_[Index + 1] == 'o') && (buffer_[Index + 2] == 'r')) {
					Index += 3;
					N = NewNode(Root);
					Root->left_ = N;
					Root->Right = NewNode();
					Root->Operation = bXor;
					if (!AddOp(Root->Right)) return false;
				}
				else {
					return true;
				}
				break;
			default:
				return true;
			}
		}
	}
	else {
		return false;
	}
	return true;
}


bool CALCULATOR::AddOp(NODE* Root) {
	NODE* N;

	if (MulOp(Root)) {
		while (buffer_[Index]) {
			switch (buffer_[Index]) {
			case '+':
				Index++;
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				Root->Operation = Add;
				if (!MulOp(Root->Right)) return false;
				break;
			case '-':
				Index++;
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				Root->Operation = Subtract;
				if (!MulOp(Root->Right)) return false;
				break;
			default:
				return true;
			}
		}
	}
	else {
		return false;
	}
	return true;
}


bool CALCULATOR::MulOp(NODE* Root) {
	NODE* N;

	if (PowerOp(Root)) {
		while (buffer_[Index]) {
			switch (buffer_[Index]) {
			case '*':
				Index++;
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				Root->Operation = Multiply;
				if (!PowerOp(Root->Right)) return false;
				break;
			case '/':
				Index++;
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				Root->Operation = Divide;
				if (!PowerOp(Root->Right)) return false;
				break;
			case '%':
				Index++;
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				Root->Operation = Remainder;
				if (!PowerOp(Root->Right)) return false;
				break;
			case 'r':
				if ((buffer_[Index + 1] == 'e') && (buffer_[Index + 2] == 'm')) {
					Index += 3;
					N = NewNode(Root);
					Root->left_ = N;
					Root->Right = NewNode();
					Root->Operation = Remainder;
					if (!PowerOp(Root->Right)) return false;
				}
				else {
					return true;
				}
				break;
			default:
				return true;
			}
		}
	}
	else {
		return false;
	}
	return true;
}


bool CALCULATOR::PowerOp(NODE* Root) {
	NODE* N;

	if (Function(Root)) {
		while (buffer_[Index]) {
			switch (buffer_[Index]) {
			case '^':
				Index++;
				N = NewNode(Root);
				Root->left_ = N;
				Root->Right = NewNode();
				Root->Operation = Power;
				if (!Function(Root->Right)) return false;
				break;
			default:
				return true;
			}
		}
	}
	else {
		return false;
	}
	return true;
}


void CALCULATOR::FuncName(string* Name) {
	*Name = "";

	int i = Index;
	while (
		((buffer_[i] >= 'a') && (buffer_[i] <= 'z')) ||
		((buffer_[i] >= 'A') && (buffer_[i] <= 'Z')) ||
		(buffer_[i] == '\\')
		) {
		*Name += buffer_[i++];
	}
}


bool CALCULATOR::Function(NODE* Root) {
	NODE* N;
	string s;
	bool   Minus = false;

	if (buffer_[Index] == '-') {
		Index++;
		Minus = true;
	}
	else if (buffer_[Index] == '~') {
		Index++;
		Root->Operation = Not;
		Root->Right = NewNode();
		return Factorial(Root->Right);
	}

	FuncName(&s);

	if ((buffer_[Index] == 'd') && (buffer_[Index + 1] == '(')) {
		Index++;
		if (!LogicOp(Root)) return false;
		if (buffer_[Index++] != 'd') return false;
		N = NewNode();
		if (!Value(N)) return false;
		if (N->Operation != Var) return false;
		Diff(Root, N->Name.c_str());
		DeleteTree(N);
	}
	else if (s == "log") {
		Index += 3;
		Root->Operation = Log;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "lb") {
		Index += 2;
		Root->Operation = Log2;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "ln") {
		Index += 2;
		Root->Operation = Ln;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "abs") {
		Index += 3;
		Root->Operation = Abs;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "round") {
		Index += 5;
		Root->Operation = Round;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "fix") {
		Index += 3;
		Root->Operation = Fix;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "floor") {
		Index += 5;
		Root->Operation = Floor;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "ceil") {
		Index += 4;
		Root->Operation = Ceil;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "rand") {
		Index += 4;
		Root->Operation = Rand;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "sin") {
		Index += 3;
		Root->Operation = Sin;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "asin") {
		Index += 4;
		Root->Operation = ASin;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "cos") {
		Index += 3;
		Root->Operation = Cos;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "acos") {
		Index += 4;
		Root->Operation = ACos;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "tan") {
		Index += 3;
		Root->Operation = Tan;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "atan") {
		Index += 4;
		Root->Operation = ATan;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "sec") {
		Index += 3;
		Root->Operation = Sec;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "asec") {
		Index += 4;
		Root->Operation = ASec;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "cosec") {
		Index += 5;
		Root->Operation = Cosec;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "acosec") {
		Index += 6;
		Root->Operation = ACosec;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "cot") {
		Index += 3;
		Root->Operation = Cot;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "acot") {
		Index += 4;
		Root->Operation = ACot;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "sinh") {
		Index += 4;
		Root->Operation = Sinh;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "asinh") {
		Index += 5;
		Root->Operation = ASinh;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "cosh") {
		Index += 4;
		Root->Operation = Cosh;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "acosh") {
		Index += 5;
		Root->Operation = ACosh;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "tanh") {
		Index += 4;
		Root->Operation = Tanh;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "atanh") {
		Index += 5;
		Root->Operation = ATanh;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "sech") {
		Index += 4;
		Root->Operation = Sech;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "asech") {
		Index += 5;
		Root->Operation = ASech;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "cosech") {
		Index += 6;
		Root->Operation = Cosech;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "acosech") {
		Index += 7;
		Root->Operation = ACosech;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "coth") {
		Index += 4;
		Root->Operation = Coth;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "acoth") {
		Index += 5;
		Root->Operation = ACoth;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else if (s == "not") {
		Index += 3;
		Root->Operation = bNot;
		Root->Right = NewNode();
		if (!Factorial(Root->Right)) return false;
	}
	else {
		if (Minus) Index;
		return Factorial(Root);
	}

	if (Minus) {
		N = NewNode(Root);
		Root->Operation = Multiply;
		Root->left_ = NewNode();
		Root->left_->Value = -1.;
		Root->Right = N;
	}

	return true;
}


bool CALCULATOR::Factorial(NODE* Root) {
	NODE* N;
	if (Value(Root)) {
		if (buffer_[Index] == '!') {
			Index++;
			N = NewNode(Root);
			Root->left_ = 0;
			Root->Right = N;
			Root->Operation = Fact;
		}
	}
	else {
		return false;
	}
	return true;
}


bool CALCULATOR::Exponent(NODE* Root) {
	int  s = 1;
	int  i;
	bool Binary;

	if (buffer_[Index] == 'e' || buffer_[Index] == 'E') Binary = false;
	else if (buffer_[Index] == 'p' || buffer_[Index] == 'P') Binary = true;
	else return true; // It's not an exponent, but not an error either
	Index++;

	s = 1;
	if (buffer_[Index] == '+') {
		Index++;
	}
	else if (buffer_[Index] == '-') {
		Index++;
		s = -1;
	}
	i = 0;
	while ((buffer_[Index] >= '0') && (buffer_[Index] <= '9')) {
		i = i * 10 + (buffer_[Index] - '0');
		Index++;
	}
	i *= s;

	if (Binary) Root->Value *= pow(2.0, i);
	else       Root->Value *= pow(10.0, i);

	return true;
}


bool CALCULATOR::Value(NODE* Root) {
	bool        Minus = false;
	string      s;
	NODE* N;
	long double f;

	if (buffer_[Index] == '-') {
		Index++;
		Minus = true;
	}

	if ((buffer_[Index] == '0') && (buffer_[Index + 1] == 'x')) {
		Index += 2;
		f = 0;
		while (((buffer_[Index] >= '0') && (buffer_[Index] <= '9')) ||
			((buffer_[Index] >= 'a') && (buffer_[Index] <= 'f')) ||
			((buffer_[Index] >= 'A') && (buffer_[Index] <= 'F'))) {
			if (buffer_[Index] <= '9') {
				f = f * 16. + (buffer_[Index] - '0');
			}
			else if (buffer_[Index] <= 'F') {
				f = f * 16. + (buffer_[Index] - 'A' + 10.);
			}
			else {
				f = f * 16. + (buffer_[Index] - 'a' + 10.);
			}
			Index++;
		}
		if (Minus) f *= -1.;
		Root->Operation = Val;
		Root->Value = f;
		if (!Exponent(Root)) return false;

	}
	else if ((buffer_[Index] == '0') && (buffer_[Index + 1] == 'b')) {
		Index += 2;
		f = 0;
		while (buffer_[Index] == '0' || buffer_[Index] == '1') {
			f = f * 2. + buffer_[Index] - '0';
			Index++;
		}
		if (Minus) f *= -1.;
		Root->Operation = Val;
		Root->Value = f;
		if (!Exponent(Root)) return false;

	}
	else if (((buffer_[Index] >= '0') && (buffer_[Index] <= '9')) ||
		((buffer_[Index] == '.') || (buffer_[Index] == ','))) {
		if (!Float(&f)) return false;
		if (Minus) f *= -1.;
		Root->Operation = Val;
		Root->Value = f;
		if (!Exponent(Root)) return false;

	}
	else if (buffer_[Index] == '(') {
		Index++;
		if (!LogicOp(Root)) return false;
		if (buffer_[Index++] != ')') return false;
		if (Minus) {
			N = NewNode(Root);
			Root->Operation = Multiply;
			Root->left_ = NewNode();
			Root->left_->Value = -1.;
			Root->Right = N;
		}
	}
	else {
		FuncName(&s);
		if (s.length()) {
			auto Constant = Constants.find(s);
			if (Constant != Constants.end()) {
				Index += Constant->first.length();
				Root->Operation = Val;
				Root->Value = Constant->second;
				if (Minus) Root->Value *= -1.;

			}
			else {
				Index += s.length();
				Root->Operation = Var;
				Root->Name = s;
				if (Minus) {
					N = NewNode(Root);
					Root->Operation = Multiply;
					Root->left_ = NewNode();
					Root->left_->Value = -1.;
					Root->Right = N;
				}
			}
		}
		else {
			return false;
		}
	}
	return true;
}


bool CALCULATOR::Float(long double* f) {
	int s = 1;
	int i;
	long double temp;

	temp = 0.0;
	while ((buffer_[Index] >= '0') && (buffer_[Index] <= '9')) {
		temp = temp * 10. + (buffer_[Index] - '0');
		Index++;
	}
	temp *= s;

	if ((buffer_[Index] == '.') || (buffer_[Index] == ',')) {
		Index++;
		i = -1;
		while ((buffer_[Index] >= '0') && (buffer_[Index] <= '9')) {
			temp += (buffer_[Index] - '0') * pow(10., i);
			Index++;
		}
	}
	*f = temp;

	return true;
}


void CALCULATOR::BuildTree(const char* Formula) {
	DeleteTree(Tree);
	Tree = 0;
	int j, q;

	for (j = 0; Formula[j]; j++);
	buffer_ = new char[j + 1];

	q = 0;
	for (j = 0; Formula[j]; j++) {
		if (Formula[j] != ' ' && Formula[j] != '\t') buffer_[q++] = Formula[j];
	}
	buffer_[q] = 0;

	if (q) {
		Index = 0;
		Tree = NewNode();
		if (!ConditionOp(Tree)) {
			DeleteTree(Tree);
			Tree = 0;
		}
	}

	delete[] buffer_;

	while (Simplify(Tree));
}


bool comp(char* a, char* b) {
	for (int j = 0; j < 10; j++) {
		if (a[j] != b[j]) return false;
	}
	return true;
}


long double CALCULATOR::CalcTree(NODE* Root, const char* Variable,
	long double Value) {
	long double A;
	long double B;
	long double nan = NAN/*0.0/0.0*/;
	long double inf = INFINITY/*1.0/0.0*/;
	int i1, i2;

	if (Root) {
		if (Root->Right) { // Function
			switch (Root->Operation) {
			case Fact:
				A = CalcTree(Root->Right, Variable, Value);
				if (A < 0.) return nan;
				i1 = floor(A);
				if (A != i1) return nan;
				A = 1.;
				for (; i1 > 1; i1) {
					A *= i1;
				}
				return A;
			case Power:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				i1 = round(B);
				if (i1 == B) {
					return pow(A, i1);
				}
				else {
					if (A < 0.) return nan;
					if (comp((char*)(&A), (char*)(&nan)) ||
						comp((char*)(&B), (char*)(&nan)) ||
						A == inf ||
						B == inf ||
						A == -inf ||
						B == -inf) {
						return nan;
					}
					else {
						return pow(A, B);
					}
				}
			case Multiply:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				return A * B;
			case Divide:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				if (B == 0.) {
					if (A < 0.) {
						return -1 * inf;
					}
					else if (A > 0.) {
						return inf;
					}
					else {
						return nan;
					}
				}
				else {
					return A / B;
				}
			case Remainder:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				if (B == 0.) {
					return nan;
				}
				else {
					i1 = round(A);
					i2 = round(B);
					if ((i1 == A) && (i2 == B)) {
						return i1 % i2;
					}
					else {
						if (((A > 0.) && (B > 0.)) ||
							((A < 0.) && (B < 0.))) {
							while (A > B) A -= B;
							return A;
						}
						else {
							while (A < B) A += B;
							return A;
						}
					}
				}
			case Add:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				return A + B;
			case Subtract:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				return A - B;
			case Log:
				A = CalcTree(Root->Right, Variable, Value);
				return log10l(A);
			case Log2:
				A = CalcTree(Root->Right, Variable, Value);
				return log2l(A);
			case Ln:
				A = CalcTree(Root->Right, Variable, Value);
				return logl(A);
			case Abs:
				A = CalcTree(Root->Right, Variable, Value);
				return fabsl(A);
			case Round:
				A = CalcTree(Root->Right, Variable, Value);
				return round(A);
			case Fix:
				A = CalcTree(Root->Right, Variable, Value);
				return fix(A);
			case Floor:
				A = CalcTree(Root->Right, Variable, Value);
				return floorl(A);
			case Ceil:
				A = CalcTree(Root->Right, Variable, Value);
				return ceill(A);
			case Rand:
				A = CalcTree(Root->Right, Variable, Value);
				B = rand();
				B /= RAND_MAX;
				return A * B;
			case Sin:
				A = CalcTree(Root->Right, Variable, Value);
				if (Measure == Degrees) A *= pi / 180.;
				return sinl(A);
			case ASin:
				A = CalcTree(Root->Right, Variable, Value);
				A = asinl(A);
				if (Measure == Degrees) A *= 180. / pi;
				return A;
			case Cos:
				A = CalcTree(Root->Right, Variable, Value);
				if (Measure == Degrees) A *= pi / 180.;
				return cosl(A);
			case ACos:
				A = CalcTree(Root->Right, Variable, Value);
				A = acosl(A);
				if (Measure == Degrees) A *= 180. / pi;
				return A;
			case Tan:
				A = CalcTree(Root->Right, Variable, Value);
				if (Measure == Degrees) A *= pi / 180.;
				return tanl(A);
			case ATan:
				A = CalcTree(Root->Right, Variable, Value);
				A = atanl(A);
				if (Measure == Degrees) A *= 180. / pi;
				return A;
			case Sec:
				A = CalcTree(Root->Right, Variable, Value);
				if (Measure == Degrees) A *= pi / 180.;
				A = cosl(A);
				if (A == 0.) {
					return inf;
				}
				else {
					return 1 / A;
				}
			case ASec:
				A = CalcTree(Root->Right, Variable, Value);
				if (A == 0.) {
					return nan;
				}
				else {
					A = 1 / A;
					A = acosl(A);
					if (Measure == Degrees) A *= 180. / pi;
					return A;
				}
			case Cosec:
				A = CalcTree(Root->Right, Variable, Value);
				if (Measure == Degrees) A *= pi / 180.;
				A = sinl(A);
				if (A == 0.) {
					return inf;
				}
				else {
					return 1 / A;
				}
			case ACosec:
				A = CalcTree(Root->Right, Variable, Value);
				if (A == 0.) {
					return nan;
				}
				else {
					A = 1 / A;
					A = asinl(A);
					if (Measure == Degrees) A *= 180. / pi;
					return A;
				}
			case Cot:
				A = CalcTree(Root->Right, Variable, Value);
				if (Measure == Degrees) A *= pi / 180.;
				A = tanl(A);
				if (A == 0.) {
					return inf;
				}
				else {
					return 1 / A;
				}
			case ACot:
				A = CalcTree(Root->Right, Variable, Value);
				if (A == 0.) {
					return nan;
				}
				else {
					A = 1 / A;
					A = atanl(A);
					if (Measure == Degrees) A *= 180. / pi;
					return A;
				}
			case Sinh:
				A = CalcTree(Root->Right, Variable, Value);
				return sinhl(A);
			case ASinh:
				A = CalcTree(Root->Right, Variable, Value);
				return logl(A + sqrtl(A * A + 1));
			case Cosh:
				A = CalcTree(Root->Right, Variable, Value);
				return coshl(A);
			case ACosh:
				A = CalcTree(Root->Right, Variable, Value);
				if (A < 1.) {
					return nan;
				}
				else {
					return logl(A + sqrt(A * A - 1));
				}
			case Tanh:
				A = CalcTree(Root->Right, Variable, Value);
				return tanhl(A);
			case ATanh:
				A = CalcTree(Root->Right, Variable, Value);
				if ((-1. < A) && (A < 1.)) {
					return 0.5 * logl((1 + A) / (1 - A));
				}
				else {
					return nan;
				}
			case Sech:
				A = CalcTree(Root->Right, Variable, Value);
				A = coshl(A);
				if (A == 0.) {
					return inf;
				}
				else {
					return 1 / A;
				}
			case ASech:
				A = CalcTree(Root->Right, Variable, Value);
				if ((A <= 0.) || (A > 1.)) {
					return nan;
				}
				else {
					return logl(1 / A + sqrt(1 / A / A - 1));
				}
			case Cosech:
				A = CalcTree(Root->Right, Variable, Value);
				A = sinh(A);
				if (A == 0.) {
					return inf;
				}
				else {
					return 1 / A;
				}
			case ACosech:
				A = CalcTree(Root->Right, Variable, Value);
				if (A == 0.) {
					return nan;
				}
				else {
					return logl(1 / A + sqrt(1 / A / A + 1));
				}
			case Coth:
				A = CalcTree(Root->Right, Variable, Value);
				A = tanhl(A);
				if (A == 0.) {
					return inf;
				}
				else {
					return 1 / A;
				}
			case ACoth:
				A = CalcTree(Root->Right, Variable, Value);
				if ((-1 <= A) && (A <= 1.)) {
					return nan;
				}
				else {
					return 0.5 * logl((A + 1) / (A - 1));
				}
			case Condition:
				A = CalcTree(Root->left_, Variable, Value);
				if (A != 0.) {
					return CalcTree(Root->Right, Variable, Value);
				}
				else {
					return CalcTree(Root->Other, Variable, Value);
				}
			case Greater:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				if (A > B) {
					return 1.;
				}
				else {
					return 0.;
				}
			case Less:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				if (A < B) {
					return 1.;
				}
				else {
					return 0.;
				}
			case Equal:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				if (A == B) {
					return 1.;
				}
				else {
					return 0.;
				}
			case GreaterEqual:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				if (A >= B) {
					return 1.;
				}
				else {
					return 0.;
				}
			case LessEqual:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				if (A <= B) {
					return 1.;
				}
				else {
					return 0.;
				}
			case NotEqual:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				if (A != B) {
					return 1.;
				}
				else {
					return 0.;
				}
			case Not:
				A = CalcTree(Root->Right, Variable, Value);
				if (A == 0.) {
					return 1.;
				}
				else {
					return 0.;
				}
			case bNot:
				A = CalcTree(Root->Right, Variable, Value);
				return ~((uint64_t)A);
			case And:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				if ((A != 0.) && (B != 0.)) {
					return 1.;
				}
				else {
					return 0.;
				}
			case Or:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				if ((A != 0.) || (B != 0.)) {
					return 1.;
				}
				else {
					return 0.;
				}
			case Xor:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				if (((A != 0.) && (B == 0.)) ||
					((A == 0.) && (B != 0.))) {
					return 1.;
				}
				else {
					return 0.;
				}
			case bAnd:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				return ((uint64_t)A) & ((uint64_t)B);
			case bOr:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				return ((uint64_t)A) | ((uint64_t)B);
			case bXor:
				A = CalcTree(Root->left_, Variable, Value);
				B = CalcTree(Root->Right, Variable, Value);
				return ((uint64_t)A) ^ ((uint64_t)B);
			default:
				break;
			}
		}
		else { // Value
			if (Root->Operation == Var) {
				if (Root->Name == Variable) {
					return Value;
				}
				else {
					return 0.;
				}
			}
			else {
				return Root->Value;
			}
		}
	}
	else {
		return 0.;
	}
	return 0.;
}


long double CALCULATOR::CalculateTree(const char* Variable, long double Value) {
	return CalcTree(Tree, Variable, Value);
}


long double CALCULATOR::Calculate(
	const char* Formula,
	const char* Variable,
	long double Value
) {
	BuildTree(Formula);
	return CalculateTree(Variable, Value);
}


bool CALCULATOR::Simplify(NODE* Root) {
	NODE* N;
	if (Root) {
		while (Simplify(Root->left_));
		while (Simplify(Root->Right));

		if (Root->Right) { // Function
			if (Root->Right->Operation == Val &&
				Root->Operation != Rand
				) {
				if (Root->left_ == 0 ||
					Root->left_->Operation == Val
					) {
					Root->Value = CalcTree(Root, 0, 0.);
					Root->Operation = Val;
					DeleteTree(Root->left_);
					Root->left_ = 0;
					DeleteTree(Root->Right);
					Root->Right = 0;
					return true;
				}
			}

			switch (Root->Operation) {
			case Power:
				if (Root->Right->Operation == Val) {
					if (Root->Right->Value == 0.) {
						DeleteTree(Root->left_);
						Root->left_ = 0;
						DeleteTree(Root->Right);
						Root->Right = 0;
						Root->Operation = Val;
						Root->Value = 1.;
						return true;
					}
					else if (Root->Right->Value == 1.) {
						DeleteTree(Root->Right);
						N = Root->left_;
						Root->Operation = N->Operation;
						Root->Value = N->Value;
						Root->left_ = N->left_;
						Root->Right = N->Right;
						Root->Other = N->Other;
						Root->Name = N->Name;
						delete N;
						return true;
					}
				}
				break;

			case Multiply:
				if (Root->Right->Operation == Val) {
					if (Root->Right->Value == 0.) {
						DeleteTree(Root->left_);
						Root->left_ = 0;
						DeleteTree(Root->Right);
						Root->Right = 0;
						Root->Operation = Val;
						Root->Value = 0.;
						return true;
					}
					else if (Root->Right->Value == 1.) {
						DeleteTree(Root->Right);
						N = Root->left_;
						Root->Operation = N->Operation;
						Root->Value = N->Value;
						Root->left_ = N->left_;
						Root->Right = N->Right;
						Root->Other = N->Other;
						Root->Name = N->Name;
						delete N;
						return true;
					}
				}
				else if (Root->left_->Operation == Val) {
					if (Root->left_->Value == 0.) {
						DeleteTree(Root->left_);
						Root->left_ = 0;
						DeleteTree(Root->Right);
						Root->Right = 0;
						Root->Operation = Val;
						Root->Value = 0.;
						return true;
					}
					else if (Root->left_->Value == 1.) {
						DeleteTree(Root->left_);
						N = Root->Right;
						Root->Operation = N->Operation;
						Root->Value = N->Value;
						Root->left_ = N->left_;
						Root->Right = N->Right;
						Root->Other = N->Other;
						Root->Name = N->Name;
						delete N;
						return true;
					}
				}
				break;

			case Divide:
				if (Root->Right->Operation == Val) {
					if (Root->Right->Value == 1.) {
						DeleteTree(Root->Right);
						N = Root->left_;
						Root->Operation = N->Operation;
						Root->Value = N->Value;
						Root->left_ = N->left_;
						Root->Right = N->Right;
						Root->Other = N->Other;
						Root->Name = N->Name;
						delete N;
						return true;
					}
				}
				else {
					if (Root->left_->Operation == Val &&
						Root->left_->Value == 0.) {
						Root->Operation = Val;
						Root->Value = 0.;
						DeleteTree(Root->left_);
						Root->left_ = 0;
						DeleteTree(Root->Right);
						Root->Right = 0;
						return true;
					}
				}
				break;

			case Add:
				if (Root->Right->Operation == Val) {
					if (Root->Right->Value == 0.) {
						DeleteTree(Root->Right);
						N = Root->left_;
						Root->Operation = N->Operation;
						Root->Value = N->Value;
						Root->left_ = N->left_;
						Root->Right = N->Right;
						Root->Other = N->Other;
						Root->Name = N->Name;
						delete N;
						return true;
					}
				}
				else if (Root->left_->Operation == Val) {
					if (Root->left_->Value == 0.) {
						DeleteTree(Root->left_);
						N = Root->Right;
						Root->Operation = N->Operation;
						Root->Value = N->Value;
						Root->left_ = N->left_;
						Root->Right = N->Right;
						Root->Other = N->Other;
						Root->Name = N->Name;
						delete N;
						return true;
					}
				}
				break;

			case Subtract:
				if (Root->Right->Operation == Val) {
					if (Root->Right->Value == 0.) {
						DeleteTree(Root->Right);
						N = Root->left_;
						Root->Operation = N->Operation;
						Root->Value = N->Value;
						Root->left_ = N->left_;
						Root->Right = N->Right;
						Root->Other = N->Other;
						Root->Name = N->Name;
						delete N;
						return true;
					}
				}
				break;

			default:
				break;
			}
		}
	}
	return false;
}


void CALCULATOR::Diff(NODE* Root, const char* Variable) {
	NODE* N;
	NODE* Temp;
	if (Root) {
		if (Root->Right) { // Function
			switch (Root->Operation) {
			case Power:
				N = NewNode(Root);
				Root->Operation = Add;
				Root->left_ = NewNode();
				Root->Right = NewNode();

				Temp = Root->left_;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = CopyNode(N->left_);
				Diff(Temp->Right, Variable);

				Temp = Temp->left_;
				Temp->Operation = Multiply;
				Temp->left_ = CopyNode(N->Right);
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Power;
				Temp->left_ = CopyNode(N->left_);
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Subtract;
				Temp->left_ = CopyNode(N->Right);
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->Right;
				Temp->Operation = Multiply;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = CopyNode(N->Right);
				Diff(Temp->Right, Variable);

				Temp = Temp->left_;
				Temp->Operation = Ln;
				Temp->Right = CopyNode(N->left_);
				break;
			case Multiply:
				N = NewNode(Root);
				Root->Operation = Add;
				Root->left_ = N;
				Root->Right = CopyNode(N);
				Diff(Root->left_->left_, Variable);
				Diff(Root->Right->Right, Variable);
				break;
			case Divide:
				N = NewNode(Root);
				Root->Operation = Divide;
				Root->left_ = NewNode();
				Root->Right = NewNode();

				Temp = Root->left_;
				Temp->Operation = Subtract;
				Temp->left_ = N;
				Temp->Right = CopyNode(N);
				Diff(Temp->left_->left_, Variable);
				Diff(Temp->Right->Right, Variable);

				Temp = Root->Right;
				Temp->Operation = Power;
				Temp->left_ = CopyNode(N->Right);
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;
				break;
			case Add:
				Diff(Root->left_, Variable);
				Diff(Root->Right, Variable);
				break;
			case Subtract:
				Diff(Root->left_, Variable);
				Diff(Root->Right, Variable);
				break;
			case Log:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Divide;
				Temp->left_ = NewNode();
				Temp->Right = N;

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = 0.434294481903251828; // log(e)
				break;
			case Log2:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Divide;
				Temp->left_ = NewNode();
				Temp->Right = N;

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = 1.44269504088896341; // log2(e)
				break;
			case Ln:
				N = Root->Right;
				Root->Operation = Divide;
				Root->left_ = N;
				Root->Right = CopyNode(N);
				Diff(Root->left_, Variable);
				break;
			case Sin:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Cos;
				Temp->Right = N;
				break;
			case ASin:
				N = Root->Right;
				Root->Operation = Divide;
				Root->left_ = CopyNode(N);
				Root->Right = NewNode();
				Diff(Root->left_, Variable);

				Temp = Root->Right;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Subtract;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->Right->left_->Right;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;

				Temp = Root->Right->Right;
				Temp->Operation = Val;
				Temp->Value = 0.5;
				break;
			case Cos:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = -1.;

				Temp = Root->left_->Right;
				Temp->Operation = Sin;
				Temp->Right = N;
				break;
			case ACos:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Divide;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = -1.;

				Temp = Root->left_->Right;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Subtract;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->left_->Right->left_->Right;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;

				Temp = Root->left_->Right->Right;
				Temp->Operation = Val;
				Temp->Value = 0.5;
				break;
			case Tan:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Sec;
				Temp->Right = N;

				Temp = Root->left_->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;
				break;
			case ATan:
				N = Root->Right;
				Root->Operation = Divide;
				Root->left_ = CopyNode(N);
				Root->Right = NewNode();
				Diff(Root->left_, Variable);

				Temp = Root->Right;
				Temp->Operation = Add;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->Right->Right;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;
				break;
			case Sec:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Sec;
				Temp->Right = CopyNode(N);

				Temp = Root->left_->Right;
				Temp->Operation = Tan;
				Temp->Right = N;
				break;
			case ASec:
				N = Root->Right;
				Root->Operation = Divide;
				Root->left_ = CopyNode(N);
				Root->Right = NewNode();
				Diff(Root->left_, Variable);

				Temp = Root->Right;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Abs;
				Temp->Right = CopyNode(N);

				Temp = Root->Right->Right;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Subtract;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;

				Temp = Root->Right->Right->left_->Right;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->Right->Right->Right;
				Temp->Operation = Val;
				Temp->Value = 0.5;
				break;
			case Cosec:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = NewNode();

				Temp = Root->left_;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = -1.;

				Temp = Root->left_->Right;
				Temp->Operation = Cosec;
				Temp->Right = CopyNode(N);

				Temp = Root->Right;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = CopyNode(N);
				Diff(Temp->Right, Variable);

				Temp = Temp->left_;
				Temp->Operation = Cot;
				Temp->Right = N;
				break;
			case ACosec:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Divide;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = -1.;

				Temp = Root->left_->Right;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Abs;
				Temp->Right = CopyNode(N);

				Temp = Root->left_->Right->Right;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Subtract;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;

				Temp = Root->left_->Right->Right->left_->Right;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->left_->Right->Right->Right;
				Temp->Operation = Val;
				Temp->Value = 0.5;
				break;
			case Cot:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = -1.;

				Temp = Root->left_->Right;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Cosec;
				Temp->Right = N;

				Temp = Root->left_->Right->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;
				break;
			case ACot:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Divide;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = -1;

				Temp = Root->left_->Right;
				Temp->Operation = Add;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = 1;

				Temp = Root->left_->Right->Right;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;
				break;
			case Sinh:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Cosh;
				Temp->Right = N;
				break;
			case ASinh:
				N = Root->Right;
				Root->Operation = Divide;
				Root->left_ = CopyNode(N);
				Root->Right = NewNode();
				Diff(Root->left_, Variable);

				Temp = Root->Right;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Add;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;

				Temp = Root->Right->left_->Right;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->Right->Right;
				Temp->Operation = Val;
				Temp->Value = 0.5;
				break;
			case Cosh:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Sinh;
				Temp->Right = N;
				break;
			case ACosh:
				N = Root->Right;
				Root->Operation = Divide;
				Root->left_ = CopyNode(N);
				Root->Right = NewNode();
				Diff(Root->left_, Variable);

				Temp = Root->Right;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Subtract;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;

				Temp = Root->Right->left_->Right;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->Right->Right;
				Temp->Operation = Val;
				Temp->Value = 0.5;
				break;
			case Tanh:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Sech;
				Temp->Right = N;

				Temp = Root->left_->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;
				break;
			case ATanh:
				N = Root->Right;
				Root->Operation = Divide;
				Root->left_ = CopyNode(N);
				Root->Right = NewNode();
				Diff(Root->left_, Variable);

				Temp = Root->Right;
				Temp->Operation = Subtract;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->Right->Right;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;
				break;
			case Sech:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = NewNode();

				Temp = Root->left_;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = -1.;

				Temp = Root->left_->Right;
				Temp->Operation = Sech;
				Temp->Right = CopyNode(N);

				Temp = Root->Right;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = CopyNode(N);
				Diff(Temp->Right, Variable);

				Temp = Temp->left_;
				Temp->Operation = Tanh;
				Temp->Right = N;
				break;
			case ASech:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Divide;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = -1.;

				Temp = Root->left_->Right;
				Temp->Operation = Multiply;
				Temp->left_ = CopyNode(N);
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Subtract;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->left_->Right->Right->left_->Right;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;

				Temp = Root->left_->Right->Right->Right;
				Temp->Operation = Val;
				Temp->Value = 0.5;
				break;
			case Cosech:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = NewNode();

				Temp = Root->left_;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = -1.;

				Temp = Root->left_->Right;
				Temp->Operation = Cosech;
				Temp->Right = CopyNode(N);

				Temp = Root->Right;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = CopyNode(N);
				Diff(Temp->Right, Variable);

				Temp = Temp->left_;
				Temp->Operation = Coth;
				Temp->Right = N;
				break;
			case ACosech:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Divide;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = -1.;

				Temp = Root->left_->Right;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Abs;
				Temp->Right = CopyNode(N);

				Temp = Root->left_->Right->Right;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Add;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->left_->Right->Right->left_->Right;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;

				Temp = Root->left_->Right->Right->Right;
				Temp->Operation = Val;
				Temp->Value = 0.5;
				break;
			case Coth:
				N = Root->Right;
				Root->Operation = Multiply;
				Root->left_ = NewNode();
				Root->Right = CopyNode(N);
				Diff(Root->Right, Variable);

				Temp = Root->left_;
				Temp->Operation = Multiply;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = -1.;

				Temp = Root->left_->Right;
				Temp->Operation = Power;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Cosech;
				Temp->Right = N;

				Temp = Root->left_->Right->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;
				break;
			case ACoth:
				N = Root->Right;
				Root->Operation = Divide;
				Root->left_ = CopyNode(N);
				Root->Right = NewNode();
				Diff(Root->left_, Variable);

				Temp = Root->Right;
				Temp->Operation = Subtract;
				Temp->left_ = NewNode();
				Temp->Right = NewNode();

				Temp = Temp->left_;
				Temp->Operation = Val;
				Temp->Value = 1.;

				Temp = Root->Right->Right;
				Temp->Operation = Power;
				Temp->left_ = N;
				Temp->Right = NewNode();

				Temp = Temp->Right;
				Temp->Operation = Val;
				Temp->Value = 2.;
				break;
			default:
				DeleteTree(Root->Right);
				Root->Right = 0;
				Root->Operation = Val;
				Root->Value = 0.;
				break;
			}
			while (Simplify(Root));
		}
		else { // Value
			if (Root->Operation == Var) {
				if (Root->Name == Variable) {
					Root->Operation = Val;
					Root->Value = 1.;
				}
			}
			else {
				Root->Operation = Val;
				Root->Value = 0.;
			}
		}
	}
}


void CALCULATOR::ViewTree(NODE* Root, string* Result) {
	string A, B;

	if (Root) {
		if (Root->Right) { // Function
			switch (Root->Operation) {
			case Fact:
				ViewTree(Root->Right, &A);
				*Result += A;
				*Result += "!";
				return;

			case Power:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result += "(";
				*Result += A;
				*Result += ")^(";
				*Result += B;
				*Result += ")";
				return;

			case Multiply:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result += "(";
				*Result += A;
				*Result += "*";
				*Result += B;
				*Result += ")";
				return;

			case Divide:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result += "(";
				*Result += A;
				*Result += "/";
				*Result += B;
				*Result += ")";
				return;

			case Remainder:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result += "(";
				*Result += A;
				*Result += "rem";
				*Result += B;
				*Result += ")";
				return;

			case Add:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result += "(";
				*Result += A;
				*Result += "+";
				*Result += B;
				*Result += ")";
				return;

			case Subtract:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result += "(";
				*Result += A;
				*Result += "-";
				*Result += B;
				*Result += ")";
				return;

			case Log:
				ViewTree(Root->Right, &A);
				*Result += "log(";
				*Result += A;
				*Result += ")";
				return;

			case Log2:
				ViewTree(Root->Right, &A);
				*Result += "log2(";
				*Result += A;
				*Result += ")";
				return;

			case Ln:
				ViewTree(Root->Right, &A);
				*Result += "ln(";
				*Result += A;
				*Result += ")";
				return;

			case Abs:
				ViewTree(Root->Right, &A);
				*Result += "abs(";
				*Result += A;
				*Result += ")";
				return;

			case Round:
				ViewTree(Root->Right, &A);
				*Result += "round(";
				*Result += A;
				*Result += ")";
				return;

			case Fix:
				ViewTree(Root->Right, &A);
				*Result += "fix(";
				*Result += A;
				*Result += ")";
				return;

			case Floor:
				ViewTree(Root->Right, &A);
				*Result += "floor(";
				*Result += A;
				*Result += ")";
				return;

			case Ceil:
				ViewTree(Root->Right, &A);
				*Result += "ceil(";
				*Result += A;
				*Result += ")";
				return;

			case Rand:
				ViewTree(Root->Right, &A);
				*Result += "rand(";
				*Result += A;
				*Result += ")";
				return;

			case Sin:
				ViewTree(Root->Right, &A);
				*Result += "sin(";
				*Result += A;
				*Result += ")";
				return;

			case ASin:
				ViewTree(Root->Right, &A);
				*Result += "asin(";
				*Result += A;
				*Result += ")";
				return;

			case Cos:
				ViewTree(Root->Right, &A);
				*Result += "cos(";
				*Result += A;
				*Result += ")";
				return;

			case ACos:
				ViewTree(Root->Right, &A);
				*Result += "acos(";
				*Result += A;
				*Result += ")";
				return;

			case Tan:
				ViewTree(Root->Right, &A);
				*Result += "tan(";
				*Result += A;
				*Result += ")";
				return;

			case ATan:
				ViewTree(Root->Right, &A);
				*Result += "atan(";
				*Result += A;
				*Result += ")";
				return;

			case Sec:
				ViewTree(Root->Right, &A);
				*Result += "sec(";
				*Result += A;
				*Result += ")";
				return;

			case ASec:
				ViewTree(Root->Right, &A);
				*Result += "asec(";
				*Result += A;
				*Result += ")";
				return;

			case Cosec:
				ViewTree(Root->Right, &A);
				*Result += "cosec(";
				*Result += A;
				*Result += ")";
				return;

			case ACosec:
				ViewTree(Root->Right, &A);
				*Result += "acosec(";
				*Result += A;
				*Result += ")";
				return;

			case Cot:
				ViewTree(Root->Right, &A);
				*Result += "cot(";
				*Result += A;
				*Result += ")";
				return;

			case ACot:
				ViewTree(Root->Right, &A);
				*Result += "acot(";
				*Result += A;
				*Result += ")";
				return;

			case Sinh:
				ViewTree(Root->Right, &A);
				*Result += "sinh(";
				*Result += A;
				*Result += ")";
				return;

			case ASinh:
				ViewTree(Root->Right, &A);
				*Result += "asinh(";
				*Result += A;
				*Result += ")";
				return;

			case Cosh:
				ViewTree(Root->Right, &A);
				*Result += "cosh(";
				*Result += A;
				*Result += ")";
				return;

			case ACosh:
				ViewTree(Root->Right, &A);
				*Result += "acosh(";
				*Result += A;
				*Result += ")";
				return;

			case Tanh:
				ViewTree(Root->Right, &A);
				*Result += "tanh(";
				*Result += A;
				*Result += ")";
				return;

			case ATanh:
				ViewTree(Root->Right, &A);
				*Result += "atanh(";
				*Result += A;
				*Result += ")";
				return;

			case Sech:
				ViewTree(Root->Right, &A);
				*Result += "sech(";
				*Result += A;
				*Result += ")";
				return;

			case ASech:
				ViewTree(Root->Right, &A);
				*Result += "asech(";
				*Result += A;
				*Result += ")";
				return;

			case Cosech:
				ViewTree(Root->Right, &A);
				*Result += "cosech(";
				*Result += A;
				*Result += ")";
				return;

			case ACosech:
				ViewTree(Root->Right, &A);
				*Result += "acosech(";
				*Result += A;
				*Result += ")";
				return;

			case Coth:
				ViewTree(Root->Right, &A);
				*Result += "coth(";
				*Result += A;
				*Result += ")";
				return;

			case ACoth:
				ViewTree(Root->Right, &A);
				*Result += "acoth(";
				*Result += A;
				*Result += ")";
				return;

			case Condition:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "[";
				*Result += A;
				*Result += "]";
				*Result += B;
				ViewTree(Root->Other, &A);
				*Result += A;
				return;

			case Greater:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += ">";
				*Result += B;
				*Result += ")";
				return;

			case Less:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += "<";
				*Result += B;
				*Result += ")";
				return;

			case Equal:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += "=";
				*Result += B;
				*Result += ")";
				return;

			case GreaterEqual:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += ">=";
				*Result += B;
				*Result += ")";
				return;

			case LessEqual:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += "<=";
				*Result += B;
				*Result += ")";
				return;

			case NotEqual:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += "~=";
				*Result += B;
				*Result += ")";
				return;

			case Not:
				ViewTree(Root->left_, &A);
				*Result = "(~";
				*Result += A;
				*Result += ")";
				return;

			case bNot:
				ViewTree(Root->left_, &A);
				*Result = "(not";
				*Result += A;
				*Result += ")";
				return;

			case And:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += "&";
				*Result += B;
				*Result += ")";
				return;

			case Or:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += "|";
				*Result += B;
				*Result += ")";
				return;

			case Xor:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += ":";
				*Result += B;
				*Result += ")";
				return;

			case bAnd:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += " and ";
				*Result += B;
				*Result += ")";
				return;

			case bOr:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += " or ";
				*Result += B;
				*Result += ")";
				return;

			case bXor:
				ViewTree(Root->left_, &A);
				ViewTree(Root->Right, &B);
				*Result = "(";
				*Result += A;
				*Result += " xor ";
				*Result += B;
				*Result += ")";
				return;

			default:
				*Result = "(Unknown Operation: ";
				*Result += (int)Root->Operation;
				*Result += ", Value: 0x";
				*Result += (double)round(Root->Value);
				*Result += ", Name: ";
				*Result += Root->Name;
				*Result += ", Left: ";
				*Result += Root->left_;
				*Result += ", Right: ";
				*Result += Root->Right;
				*Result += ", Other: ";
				*Result += Root->Other;
				*Result += ")";
				return;
			}
		}
		else { // Value
			if (Root->Operation == Var) {
				*Result = Root->Name;
				return;

			}
			else if (Root->Operation == Val) {
				if (Root->Value < 0.0) {
					*Result = '(';
					*Result += (double)Root->Value;
					*Result += ')';
				}
				else {
					*Result = "";
					*Result += (double)Root->Value;
				}
				return;

			}
			else {
				*Result = "(Invalid node: Operation: ";
				*Result += (int)Root->Operation;
				*Result += ", Value: ";
				*Result += (double)Root->Value;
				*Result += ", Name: ";
				*Result += Root->Name;
				*Result += ", Left: ";
				*Result += Root->left_;
				*Result += ", Right: ";
				*Result += Root->Right;
				*Result += ", Other: ";
				*Result += Root->Other;
				*Result += ")";
				return;
			}
		}
	}
	else {
		*Result = "";
	}
	*Result = "";
}


void CALCULATOR::ShowTree(string* Result) {
	*Result = "";
	ViewTree(Tree, Result);
}


