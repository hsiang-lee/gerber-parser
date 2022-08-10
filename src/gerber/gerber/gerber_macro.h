#pragma once

#include <memory>
#include <string>
#include <vector>


class OperatorItem;
class RenderCommand;
class GerberMacroParser;
class PrimitiveItem;

class GerberMacro {
private:
	std::vector<std::shared_ptr<PrimitiveItem>> primitives_;
	void Add(const std::shared_ptr<PrimitiveItem>& primitive);

	std::vector<std::shared_ptr<RenderCommand>> render_commands_;
	void Add(const std::shared_ptr<RenderCommand>& render);

	bool valid_{ false };
	std::string name_;
	std::vector<double> modifiers_;
	bool exposure_;

	double Get_mm(double number) const;

	bool inches_;

	std::unique_ptr<GerberMacroParser> parser_;
	friend class GerberMacroParser;
	friend class PrimitiveItem;

public:
	GerberMacro(const std::string& name, const std::string& buffer, bool inches);
	~GerberMacro();

	bool Valid() const;
	std::string Name() const;
	std::vector<std::shared_ptr<RenderCommand>> Render(const std::vector<double>& modifiers);
};
