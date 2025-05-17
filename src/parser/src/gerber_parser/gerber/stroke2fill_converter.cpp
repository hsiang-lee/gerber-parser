#include "stroke2fill_converter.h"
#include "gerber_primitive.h"
#include "gerber_layer.h"
#include "log/log.h"

#include <cmath>
#include <iostream>

StrokesToFillsConverter::StrokesToFillsConverter(GerberLayer &layer) : layer_(layer)
{
}

void StrokesToFillsConverter::Segment::Add(Item *item)
{
    closed_ = false;
    items_.emplace_back(item);
}

bool StrokesToFillsConverter::Segment::IsClosed()
{
    if (closed_)
    {
        return true;
    }

    if (items_.empty())
    {
        return false;
    }

    if (items_.back()->end_ == items_.front()->start_)
    {
        closed_ = true;
        return true;
    }

    return false;
}

void StrokesToFillsConverter::Segment::Reverse()
{
    if (IsClosed())
    {
        return; // Don't bother reversing closed loops
    }

    if (items_.empty())
    {
        return;
    }

    auto old_list = items_;
    items_.clear();

    auto begin_line = old_list.front();

    auto start = begin_line->start_;
    auto begin = old_list.begin();
    for (auto iter = ++begin; iter != old_list.end(); ++iter)
    {
        auto item = *iter;

        switch (item->type_)
        {
        case Primitive::cLine:
            item->start_ = start;
            std::swap(start, item->end_);
            items_.push_front(item);
            break;

        case Primitive::cArc:
            item->angle_ *= -1;
            std::swap(start, item->end_);
            items_.push_front(item);
            break;

        default:
            break;
        }
    }

    begin_line->start_ = start;
    items_.push_front(begin_line);
}

void StrokesToFillsConverter::NewSegment()
{
    auto tmp = std::make_shared<Segment>();
    segments_.emplace_back(tmp);
}

void StrokesToFillsConverter::ExtractSegments()
{
    auto old_list = layer_.primitives_;
    layer_.primitives_.clear();

    for (const auto& render : old_list)
    {
        auto *composite = dynamic_cast<Composite *>(render);
        switch (render->type_)
        {
        case Primitive::cStroke:
            NewSegment();
            for (auto &each : composite->items_)
            {
                segments_.back()->Add(each);
            }
            break;
        case Primitive::cClose:
        case Primitive::cOutline:
            layer_.Add(render);
            break;
        case Primitive::cFlash:
        case Primitive::cRectangle:
        case Primitive::cCircle:
        default:
            break;
        }
    }
}

std::shared_ptr<StrokesToFillsConverter::Segment> StrokesToFillsConverter::FindNeighbour(const std::shared_ptr<Segment> &current)
{
    if (current->items_.empty() || current->IsClosed())
    {
        return nullptr;
    }

    for (auto &each : segments_)
    {
        if (each != current && !each->items_.empty() && !each->IsClosed())
        {
            if (current->items_.back()->end_ == each->items_.front()->start_)
            {
                return each;
            }

            if (current->items_.back()->end_ == each->items_.back()->end_)
            {
                each->Reverse();
                return each;
            }
        }
    }

    // No candidates found, but run the test again checking for near segments.
    // Points are considered the same if they are closer than 1 μm.
    // This is required because many Gerber generators make rounding errors.
    double dX = 0.0;
    double dY = 0.0;

    for (auto &each : segments_)
    {
        if (each != current && !each->items_.empty() && !each->IsClosed())
        {
            dX = std::fabs(current->items_.back()->end_.first - each->items_.front()->start_.first);
            dY = std::fabs(current->items_.back()->end_.second - each->items_.front()->start_.second);
            if (dX < 1e-3 && dY < 1e-3)
            { // NOLINT
                Log::log_ << "Strokes2Fills - Warning: Joining segments that are close, but not coincident:";
                return each;
            }
            dX = std::fabs(current->items_.back()->end_.first - each->items_.back()->end_.first);
            dY = std::fabs(current->items_.back()->end_.second - each->items_.back()->end_.second);
            if (dX < 1e-3 && dY < 1e-3)
            { // NOLINT
                each->Reverse();
                std::cout << "Strokes2Fills - Warning: Joining segments that are close, but not coincident:" << std::endl;
                std::cout << "dX = " << dX << " mm (" << dX / 25.4e-3 << " mil)" << std::endl; // NOLINT
                std::cout << "dY = " << dY << " mm (" << dY / 25.4e-3 << " mil)" << std::endl; // NOLINT
                return each;
            }
        }
    }

    return nullptr;
}

void StrokesToFillsConverter::JoinSegments()
{
    auto iter = segments_.begin();
    while (iter != segments_.end())
    {
        auto neighbour = FindNeighbour(*iter);
        if (!neighbour)
        {
            iter = ++iter;
            continue;
        }

        if (neighbour == segments_.front())
        {
            segments_.pop_front();
        }

        if (neighbour == segments_.back())
        {
            segments_.pop_back();
        }

        segments_.remove(neighbour);

        auto begin_line = neighbour->items_.front();
        neighbour->items_.pop_front();

        for (const auto &each : neighbour->items_)
        {
            (*iter)->items_.emplace_back(each);
        }
    }
}

void StrokesToFillsConverter::AddSegments()
{
    if (segments_.empty())
    {
        return;
    }

    auto outline = new Outline();
    auto iter = segments_.begin();
    while (iter != segments_.end())
    {
        for (const auto &each : (*iter)->items_)
        {
            outline->items_.emplace_back(each);
        }

        ++iter;
        segments_.pop_front();
    }

    layer_.Add(outline);
}
