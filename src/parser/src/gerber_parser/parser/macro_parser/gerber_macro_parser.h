#pragma once
#include <string>
#include <memory>

class GerberFile;
class GerberMacro;
class OperatorItem;
class PrimitiveItem;

class GerberMacroParser {
public:
	GerberMacroParser(const std::string& buffer, const std::string& name, bool inches);
	~GerberMacroParser();

	std::shared_ptr<GerberMacro> Parse();

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
    std::shared_ptr<OperatorItem> Literal();
    
    bool ParserModifier(std::shared_ptr<PrimitiveItem> item, int modifier_count, int j_value = 0);

	std::shared_ptr<GerberMacro> macro_;
	std::unique_ptr<GerberFile> gerber_file_;
};
