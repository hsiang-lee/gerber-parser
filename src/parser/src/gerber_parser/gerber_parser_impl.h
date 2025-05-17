#pragma once
#include <string>
#include <vector>
#include <memory>

class Parser;
class Gerber;
class Plotter;
class Aperture;
class GerberLayer;
class GerberFile;

class GerberParserImpl
{
private:
    std::string file_name_;
    GerberFile *gerber_file_;
    GerberLayer *current_layer_;
    bool start_of_layer_{false};

    std::shared_ptr<Gerber> gerber_;
    std::shared_ptr<Plotter> plotter_;

    // Parameters Variables and Structures:
    struct FORMAT
    {
        bool omit_trailing_zeroes_{false};
        int XInteger{0};
        int XDecimal{0};
        int YInteger{0};
        int YDecimal{0};
    };
    FORMAT format_;

    void Init();
    bool ParseGerber();
    bool LoadGerber(const std::string &file_name);
    bool LoadGerber(const std::vector<char> &gerber_data);

    Parser *GetParser(char code);
    std::vector<Parser *> parsers_;

    Aperture *GetAperture(int code) const;
    void SetAperture(int code, Aperture *);

    void AddNewLayer();
    void PrepareEmptyLayer();
    void PrepareLayer();

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
    GerberParserImpl(const std::string &file_name);
    GerberParserImpl(const std::vector<char> &gerber_data);
    ~GerberParserImpl();

    std::string FileName() const;
    std::shared_ptr<Gerber> GetGerber() const;
};
