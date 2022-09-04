#pragma once
#include <map>
#include <vector>
#include <memory>

#include "gerber_file.h"
#include "gerber/gerber_enums.h"


class Parser;
class Gerber;
class Plotter;
class Aperture;
class GerberLevel;

class GerberParser {
private:
	std::string file_name_;
	std::unique_ptr<GerberFile> gerber_file_;
	std::shared_ptr<GerberLevel> current_level_;
	bool start_of_level_{ false };

	std::shared_ptr<Gerber> gerber_;
	std::shared_ptr<Plotter> plotter_;

	// Parameters Variables and Structures:
	struct FORMAT {
		bool omit_trailing_zeroes_{ false };
		int  XInteger{ 0 };
		int  XDecimal{ 0 };
		int  YInteger{ 0 };
		int  YDecimal{ 0 };
	};
	FORMAT format_;

	void Init();
	bool ParseGerber();
	bool LoadGerber(const std::string& file_name);
	bool LoadGerber(const std::vector<char>& gerber_data);

	std::shared_ptr<Parser> GetParser(char code);
	std::map<char, std::shared_ptr<Parser>> parsers_;

	std::shared_ptr<Aperture> GetAperture(int code) const;
	void SetAperture(int code, std::shared_ptr<Aperture>);

	void AddNewLevel();
	void PrepareEmptyLevel();
	void PrepareLevel();

	friend class GCodeParser;
	friend class XCodeParser;
	friend class YCodeParser;
	friend class ICodeParser;
	friend class JCodeParser;
	friend class DCodeParser;
	friend class NCodeParser;
	friend class MCodeParser;
	friend class StarParser;
	friend class ParameterParser;

	friend class AdParser;
	friend class AmParser;
	friend class AsParser;
	friend class FsParser;
	friend class IcParser;
	friend class IfParser;
	friend class IjParser;
	friend class InParser;
	friend class IoParser;
	friend class IpParser;
	friend class IrParser;
	friend class KoParser;
	friend class LnParser;
	friend class LpParser;
	friend class MiParser;
	friend class MoParser;
	friend class OfParser;
	friend class PfParser;
	friend class SfParser;
	friend class SrParser;
	friend class AtParser;

public:
	GerberParser(const std::string& file_name);
	GerberParser(const std::vector<char>& gerber_data);
	~GerberParser() = default;

	std::string FileName() const;
	std::shared_ptr<Gerber> GetGerber() const;
};
