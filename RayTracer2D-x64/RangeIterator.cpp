#include "RangeIterator.h"


RangeIterator::RangeIterator()
{
    i = 0;
    start = 0;
    step = 1;
    elem = 0;
}

RangeIterator::RangeIterator(int i, int start, int step)
{
    this->i = i;
    this->start = start;
    this->step = step;
    this->elem = start + i * step;
}

RangeIterator::RangeIterator(const RangeIterator& rangeIterator)
{
    i = rangeIterator.i;
    start = rangeIterator.start;
    step = rangeIterator.step;
    elem = rangeIterator.elem;
}


const int& RangeIterator::operator*() noexcept { elem = start + i * step; return elem; }


RangeIterator& RangeIterator::operator++() noexcept
{
    i++;
    return *this;
}


bool RangeIterator::operator==(const RangeIterator& other) const noexcept
{
    return i == other.i;
}

bool RangeIterator::operator!=(const RangeIterator& other) const noexcept
{
    return i != other.i;
}


int RangeIterator::operator-(const RangeIterator& other) noexcept
{
    return this->i - other.i;
}

RangeIterator RangeIterator::operator+(int i) const noexcept
{
    return RangeIterator(this->i + i, start, step);
}
