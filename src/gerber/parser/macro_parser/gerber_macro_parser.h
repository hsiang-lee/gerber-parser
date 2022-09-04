#pragma once
#include <string>
#include <memory>

class GerberFile;
class GerberMacro;
class OperatorItem;

class GerberMacroParser {
public:
	GerberMacroParser(GerberMacro& macro, const std::string& buffer);
	~GerberMacroParser();

	bool LoadMacro(bool inches);

private:
	bool Primitive();
	bool Comment();
	bool Circle();
	bool LineVector();
	bool LineCenter();
	bool LineLowerLeft();
	bool Outline();
	bool Polygon();
	bool Moire();
	bool Thermal();
	bool Assignment();

	std::shared_ptr<OperatorItem> Modifier();
	std::shared_ptr<OperatorItem> Term();
	std::shared_ptr<OperatorItem> Factor();
	std::shared_ptr<OperatorItem> Variable();

	GerberMacro& macro_;
	std::unique_ptr<GerberFile> gerber_file_;
};
