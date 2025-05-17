#include "parameter_parser.h"
#include "aperture/gerber_macro.h"
#include "gerber_file.h"
#include "gerber_parser_impl.h"

#include "parameter_parser/ad_parser.h"
#include "parameter_parser/am_parser.h"
#include "parameter_parser/as_parser.h"
#include "parameter_parser/at_parser.h"
#include "parameter_parser/fs_parser.h"
#include "parameter_parser/ic_parser.h"
#include "parameter_parser/if_parser.h"
#include "parameter_parser/ij_parser.h"
#include "parameter_parser/in_parser.h"
#include "parameter_parser/io_parser.h"
#include "parameter_parser/ip_parser.h"
#include "parameter_parser/ir_parser.h"
#include "parameter_parser/ko_parser.h"
#include "parameter_parser/ln_parser.h"
#include "parameter_parser/lp_parser.h"
#include "parameter_parser/mi_parser.h"
#include "parameter_parser/mo_parser.h"
#include "parameter_parser/of_parser.h"
#include "parameter_parser/pf_parser.h"
#include "parameter_parser/sf_parser.h"
#include "parameter_parser/sr_parser.h"

#include "log/log.h"

ParameterParser::ParameterParser(GerberParserImpl &gerber)
    : gerber_parser_(gerber)
{
    parsers_["AD"] = std::make_shared<AdParser>(gerber_parser_, *this);
    parsers_["AM"] = std::make_shared<AmParser>(gerber_parser_, *this);
    parsers_["AS"] = std::make_shared<AsParser>(gerber_parser_, *this);
    parsers_["FS"] = std::make_shared<FsParser>(gerber_parser_, *this);
    parsers_["IC"] = std::make_shared<IcParser>(gerber_parser_, *this);
    parsers_["IF"] = std::make_shared<IfParser>(gerber_parser_, *this);
    parsers_["IJ"] = std::make_shared<IjParser>(gerber_parser_, *this);
    parsers_["IN"] = std::make_shared<InParser>(gerber_parser_, *this);
    parsers_["IO"] = std::make_shared<IoParser>(gerber_parser_, *this);
    parsers_["IP"] = std::make_shared<IpParser>(gerber_parser_, *this);
    parsers_["IR"] = std::make_shared<IrParser>(gerber_parser_, *this);
    parsers_["KO"] = std::make_shared<KoParser>(gerber_parser_, *this);
    parsers_["LN"] = std::make_shared<LnParser>(gerber_parser_, *this);
    parsers_["LP"] = std::make_shared<LpParser>(gerber_parser_, *this);
    parsers_["MI"] = std::make_shared<MiParser>(gerber_parser_, *this);
    parsers_["MO"] = std::make_shared<MoParser>(gerber_parser_, *this);
    parsers_["OF"] = std::make_shared<OfParser>(gerber_parser_, *this);
    parsers_["PF"] = std::make_shared<PfParser>(gerber_parser_, *this);
    parsers_["SF"] = std::make_shared<SfParser>(gerber_parser_, *this);
    parsers_["SR"] = std::make_shared<SrParser>(gerber_parser_, *this);
    parsers_["IA"] = std::make_shared<AtParser>(gerber_parser_, *this);
    parsers_["TD"] = std::make_shared<AtParser>(gerber_parser_, *this);
    parsers_["TF"] = std::make_shared<AtParser>(gerber_parser_, *this);
    parsers_["TO"] = std::make_shared<AtParser>(gerber_parser_, *this);
}

ParameterParser::~ParameterParser() // NOLINT
{
}

bool ParameterParser::Run()
{
    auto delimiter = gerber_parser_.gerber_file_
                         ->buffer_[gerber_parser_.gerber_file_->pointer_ - 1];

    gerber_parser_.gerber_file_->SkipWhiteSpace();

    while (gerber_parser_.gerber_file_->MoreThanLastOne())
    {
        auto first = gerber_parser_.gerber_file_->GetChar();
        if (first == delimiter)
        {
            ++gerber_parser_.gerber_file_->pointer_;
            return true;
        }

        auto second = gerber_parser_.gerber_file_->GetChar();
        std::string code;
        code += first;
        code += second;

        if (parsers_.find(code) != parsers_.end())
        {
            auto parser = parsers_[code];
            if (!parser->Run())
            {
                return false;
            }
        }
        else
        {
            Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                      << " - Error: Unknown parameter: " << code;
            return false;
        }

        gerber_parser_.gerber_file_->SkipWhiteSpace();
    }

    Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
              << " - Error: Parameter Block without end-delimiter";

    return false;
}

bool ParameterParser::EndOfFile() { return false; }

void ParameterParser::Add(const std::shared_ptr<GerberMacro> &macro)
{
    macros_.emplace_back(macro);
}

std::shared_ptr<GerberMacro> ParameterParser::FindMacro(
    const std::string &name)
{
    for (const auto &macro : macros_)
    {
        if (macro->Name() == name)
        {
            return macro;
        }
    }

    return nullptr;
}
