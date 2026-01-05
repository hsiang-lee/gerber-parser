#include "gerber_parser_impl.h"
#include "log/log.h"

#include <random>
#include <stdexcept>
#include <thread>

#include "gerber/gerber_layer.h"
#include "gerber/plotter.h"
#include "gerber_file.h"
#include "parser/dcode_parser.h"
#include "parser/gcode_parser.h"
#include "parser/icode_parser.h"
#include "parser/jcode_parser.h"
#include "parser/mcode_parser.h"
#include "parser/ncode_parser.h"
#include "parser/parameter_parser.h"
#include "parser/star_parser.h"
#include "parser/xcode_parser.h"
#include "parser/ycode_parser.h"

GerberParserImpl::GerberParserImpl(const std::string &file_name)
    : file_name_(file_name), parsers_(10) {
  Init();
  LoadGerber(file_name);
}

GerberParserImpl::GerberParserImpl(const std::vector<char> &gerber_data) {
  Init();
  LoadGerber(gerber_data);
}

GerberParserImpl::~GerberParserImpl() {
  for (auto each : parsers_) {
    delete each;
  }
}

void GerberParserImpl::Init() {
  Log::log_.turnOff();

  format_.omit_trailing_zeroes_ = false;
  format_.XInteger = 6;  // NOLINT
  format_.XDecimal = 6;  // NOLINT
  format_.YInteger = 6;  // NOLINT
  format_.YDecimal = 6;  // NOLINT

  current_layer_ = nullptr;

  parsers_[0] = new XCodeParser(*this);
  parsers_[1] = new YCodeParser(*this);
  parsers_[2] = new ICodeParser(*this);
  parsers_[3] = new JCodeParser(*this);
  parsers_[4] = new DCodeParser(*this);
  parsers_[5] = new GCodeParser(*this);
  parsers_[6] = new NCodeParser(*this);
  parsers_[7] = new MCodeParser(*this);
  parsers_[8] = new StarParser(*this);
  parsers_[9] = new ParameterParser(*this);
}

std::string GerberParserImpl::FileName() const { return file_name_; }

std::shared_ptr<Gerber> GerberParserImpl::GetGerber() const {
  return gerber_;
}

bool GerberParserImpl::ParseGerber() {
  while (!gerber_file_->EndOfFile()) {
    gerber_file_->SkipWhiteSpace();

    auto parser = GetParser(gerber_file_->GetChar());
    if (!parser->Run()) {
      return false;
    }

    if (parser->EndOfFile()) {
      return true;
    }
  }

  Log::log_ << "Line " << gerber_file_->line_number_
            << " - Error: No end-of-file code"
            << "\n";
  return false;
}

Parser *GerberParserImpl::GetParser(char code) {
  switch (code) {
    case 'X':
      return parsers_[0];
    case 'Y':
      return parsers_[1];
    case 'I':
      return parsers_[2];
    case 'J':
      return parsers_[3];
    case 'D':
      return parsers_[4];
    case 'G':
      return parsers_[5];
    case 'N':
      return parsers_[6];
    case 'M':
      return parsers_[7];
    case '*':
      return parsers_[8];
    default:
      return parsers_[9];
  }
}

Aperture *GerberParserImpl::GetAperture(int code) const {
  return gerber_->GetAperture(code);
}

void GerberParserImpl::SetAperture(int code, Aperture *aperture) {
  gerber_->SetAperture(code, aperture);
}

void GerberParserImpl::AddNewLayer() {
  auto layer = new GerberLayer(current_layer_, gerber_->unit_);
  gerber_->AddLayer(layer);
  current_layer_ = layer;

  if (plotter_) {
    plotter_->SetExposure(geOff);
    plotter_->Do(gerber_file_->line_number_);
  }
  plotter_ = std::make_shared<Plotter>(*layer, plotter_);
}

void GerberParserImpl::PrepareEmptyLayer() {
  if (!current_layer_) {
    auto layer = new GerberLayer(nullptr, gerber_->unit_);
    gerber_->AddLayer(layer);
    current_layer_ = layer;

    plotter_ = std::make_shared<Plotter>(*layer, plotter_);
  }
}

void GerberParserImpl::PrepareLayer() {
  if (!current_layer_) {
    AddNewLayer();
  }
}

bool GerberParserImpl::LoadGerber(const std::string &file_name) {
  FILE *file = fopen(file_name.c_str(), "rb");
  if (!file) {
    Log::log_ << "failed to open gerber file.\n";
    return false;
  }

  fseek(file, 0, SEEK_END);
  const auto length = ftell(file);
  fseek(file, 0, SEEK_SET);

  std::vector<char> data(length);
  fread(data.data(), length, 1, file);
  fclose(file);

  return LoadGerber(data);
}

bool GerberParserImpl::LoadGerber(const std::vector<char> &gerber_data) {
  start_of_layer_ = false;
  gerber_ = std::make_shared<Gerber>();
  gerber_file_ = new GerberFile(gerber_data);
  const auto ret = ParseGerber();
  delete gerber_file_;
  gerber_file_ = nullptr;

  return ret;
}