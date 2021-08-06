#pragma once
#include <stdio.h>
#include <map>
#include <list>
#include <vector>
#include <memory>

#include "gerber_file.h"

#include "gerber/gerber_aperture.h"
#include "gerber/gerber_level.h"
#include "bound_box.h"


extern bool gerber_warnings;


class Parser;
class GCodeParser;
class DCodeParser;
class NCodeParser;
class ParameterParser;

class Gerber {
private:
	GerberFile gerber_file_;
	std::shared_ptr<GerberLevel> current_level_;

	void Add(std::shared_ptr<GerberLevel> level);

	// M Codes: Miscellaneous functions
	bool MCode(bool& end_of_file);

	bool LoadGerber(const std::string& file_name);

	// Parameters Variables and Structures:
	struct FORMAT {
		bool omit_trailing_zeroes_;
		int  XInteger;
		int  XDecimal;
		int  YInteger;
		int  YDecimal;
	};

	FORMAT format_;
	GERBER_UNIT units_;

	std::string name_;
	bool start_of_level_{ false };

	bool negative_{ false };
	std::vector<std::shared_ptr<GerberLevel>> levels_;

	bool ParseGerber();

	std::vector<std::shared_ptr<GerberAperture>> apertures_;
	std::map<char, std::shared_ptr<Parser>> parsers_;

	friend class GCodeParser;
	friend class DCodeParser;
	friend class NCodeParser;
	friend class ParameterParser;

public:
	Gerber(const std::string& file_name);
	~Gerber();

	bool IsNegative() const;
	BoundBox GetBBox() const;
	GERBER_UNIT Unit() const;
	std::string Name() const;

	std::vector<std::shared_ptr<GerberLevel>> Levels() const;
};
