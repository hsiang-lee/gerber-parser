#pragma once
#include <list>
#include <memory>

class Item;
class GerberLayer;

class StrokesToFillsConverter
{
    class Segment
    {
    private:
        bool closed_ = false; // true => closed; false => maybe closed

    public:
        std::list<Item *> items_;

        void Add(Item *item);
        bool IsClosed();
        void Reverse();
    };

public:
    StrokesToFillsConverter(GerberLayer &layer);

    void NewSegment();
    void ExtractSegments();
    void JoinSegments();
    void AddSegments();
    std::shared_ptr<Segment> FindNeighbour(const std::shared_ptr<Segment> &current);

private:
    GerberLayer &layer_;
    std::list<std::shared_ptr<Segment>> segments_;
};
