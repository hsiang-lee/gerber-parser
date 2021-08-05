#pragma once

class Color {
	bool cmyk_;
	union {
		struct {
			double R, G, B;
		};
		struct {
			double C, M, Y, K;
		};
	};

	double A;

public:
	Color(double r, double g, double b, double a) :
		R(r), G(g), B(b), A(a), cmyk_(false) {

	}

	Color(double c, double m, double y, double k, double a) :
		C(c), M(m), Y(y), K(k), A(a), cmyk_(true) {

	}

	void operator=  (const Color& colour) {
		cmyk_ = colour.cmyk_;

		if (cmyk_) {
			C = colour.C;
			M = colour.M;
			Y = colour.Y;
			K = colour.K;
		}
		else {
			R = colour.R;
			G = colour.G;
			B = colour.B;
		}

		A = colour.A;
	}

	bool operator== (const Color& colour) {
		if (cmyk_) {
			return C == colour.C &&
				M == colour.M &&
				Y == colour.Y &&
				K == colour.K &&
				A == colour.A;
		}
		else {
			return R == colour.R &&
				G == colour.G &&
				B == colour.B &&
				A == colour.A;
		}
	}

	bool cmyk() const {
		return cmyk_;
	}

	double r() const {
		return R;
	}

	double g() const {
		return G;
	}

	double b() const {
		return B;
	}

	double a() const {
		return A;
	}

	double c() const {
		return C;
	}

	double m() const {
		return M;
	}

	double y() const {
		return Y;
	}

	double k() const {
		return K;
	}
};
