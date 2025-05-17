#include "gerber_parser.h"
#include "gerber_parser_impl.h"

GerberParser::GerberParser(const std::string &file_name)
    : impl_(new GerberParserImpl(file_name)) {}

GerberParser::GerberParser(const std::vector<char> &gerber_data)
    : impl_(new GerberParserImpl(gerber_data)) {}

GerberParser::~GerberParser()
{
    delete impl_;
}

std::string GerberParser::FileName() const { return impl_->FileName(); }

std::shared_ptr<Gerber> GerberParser::GetGerber() const
{
    return impl_->GetGerber();
}
