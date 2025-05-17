#include "gerber_macro_parser.h"
#include "aperture/gerber_macro.h"
#include "gerber_file.h"
#include "operator_item.h"
#include "aperture/primitive_item.h"
#include "log/log.h"

GerberMacroParser::GerberMacroParser(const std::string &buffer, const std::string &name, bool inches)
{
    macro_ = std::make_shared<GerberMacro>(name, inches);
    gerber_file_ = std::make_unique<GerberFile>(std::vector<char>{buffer.begin(), buffer.end()});
}

GerberMacroParser::~GerberMacroParser() // NOLINT
{
}

std::shared_ptr<GerberMacro> GerberMacroParser::Parse()
{
    if (!Primitive())
    {
        return nullptr;
    }

    if (gerber_file_->EndOfFile() || gerber_file_->GetChar() != '*')
    {
        return nullptr;
    }

    while (Primitive())
    {
        if (gerber_file_->EndOfFile() || gerber_file_->GetChar() != '*')
        {
            return nullptr;
        }
    }

    return macro_;
}

bool GerberMacroParser::Primitive()
{
    gerber_file_->SkipWhiteSpace();
    if (gerber_file_->EndOfFile())
    {
        return false;
    }

    int identifier = 0;
    auto ret = gerber_file_->GetInteger(identifier);
    if (!ret)
    {
        if (gerber_file_->GetChar() == '$')
        {
            identifier = PrimitiveItem::kAssignment;
        }
        else
        {
            Log::log_ << "Error: Macro primitive expected";
            return false;
        }
    }

    switch (identifier)
    {
    case PrimitiveItem::kComment:
        return Comment();

    case PrimitiveItem::kCircle:
        return Circle();

    case PrimitiveItem::kLineVector:
    case PrimitiveItem::kLineVector2:
        return LineVector();

    case PrimitiveItem::kLineCenter:
        return LineCenter();

    case PrimitiveItem::kLineLowerLeft:
        return LineLowerLeft();

    case PrimitiveItem::kEndOfFile:
        return true;

    case PrimitiveItem::kOutline:
        return Outline();

    case PrimitiveItem::kPolygon:
        return Polygon();

    case PrimitiveItem::kMoire:
        return Moire();

    case PrimitiveItem::kThermal:
        return Thermal();

    case PrimitiveItem::kAssignment:
        return Assignment();

    default:
        Log::log_ << "Error: Unknown Macro Primitive: " << gerber_file_->PeekChar();
        return false;
    }

    Log::log_ << "Error: End of block expected";
    return false;
}

bool GerberMacroParser::Comment()
{
    while (!gerber_file_->EndOfFile())
    {
        if (gerber_file_->GetChar() == '*')
        {
            return true;
        }
    }

    return false;
}

bool GerberMacroParser::ParserModifier(std::shared_ptr<PrimitiveItem> item, int modifier_count, int j_value)
{
    gerber_file_->SkipWhiteSpace();

    int j = j_value;
    bool b = true;
    while (b && !gerber_file_->EndOfFile() && j < modifier_count)
    {
        if (gerber_file_->PeekChar() == ',')
        {
            gerber_file_->pointer_++;
            auto tmp = Modifier();
            item->modifiers_[j++] = tmp;
            b = bool(tmp);
        }
        else
        {
            break; // It is legal to specify fewer modifiers than the maximum
        }
    }

    return b;
}

bool GerberMacroParser::Circle()
{
    constexpr int modifier_count = 5;
    auto item = std::make_shared<CirclePrimitiveItem>();
    item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);
    macro_->Add(item);
    return ParserModifier(item, modifier_count);
}

bool GerberMacroParser::LineVector()
{
    constexpr int modifier_count = 7;
    auto item = std::make_shared<LineVectorPrimitiveItem>();
    item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);
    macro_->Add(item);
    return ParserModifier(item, modifier_count);
}

bool GerberMacroParser::LineCenter()
{
    constexpr int modifier_count = 6;
    auto item = std::make_shared<LineCenterPrimitiveItem>();
    item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);
    macro_->Add(item);
    return ParserModifier(item, modifier_count);
}

bool GerberMacroParser::LineLowerLeft()
{
    constexpr int modifier_count = 6;
    auto item = std::make_shared<LineLowerLeftPrimitiveItem>();
    item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);
    macro_->Add(item);
    return ParserModifier(item, modifier_count);
}

bool GerberMacroParser::Outline()
{
    if (gerber_file_->EndOfFile() || gerber_file_->GetChar() != ',')
    {
        return false;
    }

    int exposure = 0;
    auto ret = gerber_file_->GetInteger(exposure);
    if (!ret)
    {
        Log::log_ << "Error: Integer exposure expected";
        return false;
    }

    if (gerber_file_->EndOfFile() || gerber_file_->GetChar() != ',')
    {
        return false;
    }

    int n = 0;
    ret = gerber_file_->GetInteger(n);
    if (!ret)
    {
        Log::log_ << "Error: Integer number of outline points expected";
        return false;
    }

    const auto modifier_count = 5 + 2 * n;

    auto item = std::make_shared<OutlinePrimitiveItem>();
    item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);

    item->modifiers_[0] = std::make_shared<OperatorItem>(nullptr, nullptr, OperatorItem::kLiteral);
    item->modifiers_[0]->Value = macro_->exposure_;

    item->modifiers_[1] = std::make_shared<OperatorItem>(nullptr, nullptr, OperatorItem::kLiteral);
    item->modifiers_[1]->Value = n;
    macro_->Add(item);
    return ParserModifier(item, modifier_count, 2);
}

bool GerberMacroParser::Polygon()
{
    constexpr int modifier_count = 6;
    auto item = std::make_shared<PolygonPrimitiveItem>();
    item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);
    macro_->Add(item);
    return ParserModifier(item, modifier_count);
}

bool GerberMacroParser::Moire()
{
    constexpr int modifier_count = 9;
    auto item = std::make_shared<MoirePrimitiveItem>();
    item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);
    macro_->Add(item);
    return ParserModifier(item, modifier_count);
}

bool GerberMacroParser::Thermal()
{
    constexpr int modifier_count = 6;
    auto item = std::make_shared<ThermalPrimitiveItem>();
    item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(modifier_count);
    macro_->Add(item);
    return ParserModifier(item, modifier_count);
}

bool GerberMacroParser::Assignment()
{
    int index = 0;
    auto ret = gerber_file_->GetInteger(index);
    if (!ret)
    {
        return false;
    }

    if (gerber_file_->pointer_ > gerber_file_->buffer_.size())
    {
        return false;
    }

    if (gerber_file_->GetChar() != '=')
    {
        return false;
    }

    auto item = std::make_shared<AssignmentPrimitiveItem>();
    item->modifiers_ = std::vector<std::shared_ptr<OperatorItem>>(1);
    item->modifiers_[0] = Modifier();
    item->index_ = index;

    if (!item->modifiers_[0])
    {
        return false;
    }

    macro_->Add(item);
    return true;
}

// Term {("+" | "-") Term};
std::shared_ptr<OperatorItem> GerberMacroParser::Modifier()
{
    gerber_file_->SkipWhiteSpace();

    auto left = Term();
    if (left == nullptr)
    {
        return nullptr;
    }

    while (!gerber_file_->EndOfFile())
    {
        OperatorItem::Type opera = OperatorItem::kAdd;
        switch (gerber_file_->PeekChar())
        {
        case '+':
            opera = OperatorItem::kAdd;
            break;

        case '-':
            opera = OperatorItem::kSubtract;
            break;

        default:
            return left;
        }
        gerber_file_->pointer_++;

        auto root = std::make_shared<OperatorItem>(left, Term(), opera);
        left = root;
    }

    return nullptr;
}

// Factor {("x" | "X" | "/") Factor};
std::shared_ptr<OperatorItem> GerberMacroParser::Term()
{
    gerber_file_->SkipWhiteSpace();

    auto left = Factor();
    if (left == nullptr)
    {
        return nullptr;
    }

    while (!gerber_file_->EndOfFile())
    {
        OperatorItem::Type opera = OperatorItem::kAdd;
        switch (gerber_file_->PeekChar())
        {
        case 'x':
        case 'X':
            opera = OperatorItem::kMultiply;
            break;

        case '/':
            opera = OperatorItem::kDivide;
            break;

        default:
            return left;
        }
        gerber_file_->pointer_++;

        left = std::make_shared<OperatorItem>(left, Factor(), opera);
    }

    return nullptr;
}

// ["+" | "-"] (("(" Modifier ")") | Variable | Float)
std::shared_ptr<OperatorItem> GerberMacroParser::Factor()
{
    if (gerber_file_->EndOfFile())
    {
        return nullptr;
    }

    bool negative = false;
    if (gerber_file_->PeekChar() == '-')
    {
        negative = true;
        gerber_file_->pointer_++;
    }
    else if (gerber_file_->PeekChar() == '+')
    {
        gerber_file_->pointer_++;
    }

    std::shared_ptr<OperatorItem> item = nullptr;
    if (gerber_file_->PeekChar() == '(')
    {
        gerber_file_->pointer_++;

        item = Modifier();
        if (item == nullptr)
        {
            Log::log_ << "Error: Expression expected";
            return nullptr;
        }

        if (gerber_file_->PeekChar() != ')')
        {
            Log::log_ << "Error: ')' expected";
            return nullptr;
        }
        gerber_file_->pointer_++;
    }
    else
    {
        item = Variable();
        if (item == nullptr)
        {
            item = Literal();
            if (!item)
            {
                return nullptr;
            }
        }
    }

    if (item != nullptr && negative)
    {
        auto left = std::make_shared<OperatorItem>(nullptr, nullptr, OperatorItem::kLiteral);
        auto root = std::make_shared<OperatorItem>(left, item, OperatorItem::kMultiply);
        root->Left()->Value = -1;
        item = root;
    }

    return item;
}

// "$" Integer
std::shared_ptr<OperatorItem> GerberMacroParser::Variable()
{
    if (gerber_file_->EndOfFile() || gerber_file_->PeekChar() != '$')
    {
        return nullptr;
    }
    gerber_file_->pointer_++;

    int i = 0;
    auto ret = gerber_file_->GetInteger(i);
    if (!ret)
    {
        gerber_file_->pointer_--;
        return nullptr;
    }

    auto item = std::make_shared<OperatorItem>(nullptr, nullptr, OperatorItem::kVariable);
    item->Index = i;

    return item;
}

std::shared_ptr<OperatorItem> GerberMacroParser::Literal()
{
    double d = 0.0;
    auto ret = gerber_file_->GetFloat(d);
    if (!ret)
    {
        Log::log_ << "Error: Float expected";
        return nullptr;
    }

    auto item = std::make_shared<OperatorItem>(nullptr, nullptr, OperatorItem::kLiteral);
    item->Value = d;
    return item;
}
