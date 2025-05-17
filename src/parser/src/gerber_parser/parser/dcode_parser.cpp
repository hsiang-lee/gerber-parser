#include "dcode_parser.h"
#include "gerber/plotter.h"
#include "gerber_file.h"
#include "gerber_parser_impl.h"
#include "log/log.h"

DCodeParser::DCodeParser(GerberParserImpl &gerber) : gerber_parser_(gerber) {}

bool DCodeParser::Run()
{
    gerber_parser_.PrepareLayer();
    gerber_parser_.start_of_layer_ = false;

    int code = 0;
    if (!gerber_parser_.gerber_file_->GetInteger(code))
    {
        return false;
    }

    switch (code)
    {
    case 1: // Draw line, exposure on
        gerber_parser_.plotter_->SetExposure(geOn);
        return true;

    case 2: // Exposure off
        gerber_parser_.plotter_->SetExposure(geOff);
        return true;

    case 3: // Flash aperture
        gerber_parser_.plotter_->SetExposure(geFlash);
        return true;

    default: // Select aperture
        if (code >= 3000)
        { // NOLINT
            Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                      << " - Error: Aperture code out of range: D" << code << "\n";
            return false;
        }

        auto aperture = gerber_parser_.GetAperture(code);
        if (!aperture)
        {
            Log::log_ << "Line " << gerber_parser_.gerber_file_->line_number_
                      << " - Error: Aperture not defined: D" << code << "\n";
            return true;
        }

        gerber_parser_.plotter_->ApertureSelect(aperture, gerber_parser_.gerber_file_->line_number_);
        return true;
    }

    return false;
}

bool DCodeParser::EndOfFile() { return false; }
