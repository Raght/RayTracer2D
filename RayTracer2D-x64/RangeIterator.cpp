#include "RangeIterator.h"


RangeIterator::RangeIterator()
{
    i = 0;
    elem = 0;
    step = 1;
}

RangeIterator::RangeIterator(int i, int elem, int step)
{
    this->i = i;
    this->elem = elem;
    this->step = step;
}

RangeIterator::RangeIterator(const RangeIterator& rangeIterator)
{
    i = rangeIterator.i;
    elem = rangeIterator.elem;
    step = rangeIterator.step;
}


const int& RangeIterator::operator*() noexcept { return elem; }


RangeIterator& RangeIterator::operator++() noexcept
{
    i++;
    elem += step;
    return *this;
}


bool RangeIterator::operator==(const RangeIterator& other) const noexcept
{
    return i == other.i && elem == other.elem && step == other.step;
}

bool RangeIterator::operator!=(const RangeIterator& other) const noexcept
{
    return !this->operator==(other);
}


int RangeIterator::operator-(const RangeIterator& other) noexcept
{
    return this->i - other.i;
}