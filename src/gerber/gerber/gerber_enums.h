#pragma once

enum GERBER_UNIT {
	guInches,
	guMillimeters
};

enum GERBER_INTERPOLATION {
	giLinear,
	giLinear10X,
	giLinear0_1X,
	giLinear0_01X,
	giClockwiseCircular,
	giCounterclockwiseCircular
};

enum GERBER_EXPOSURE {
	geOn,
	geOff,
	geFlash
};
