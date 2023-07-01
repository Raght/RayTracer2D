#include <iterator>
#include <iostream>
#include "RangeIterator.h"


struct Range
{
    Range();
    Range(int a, int b, int step = 1);
    Range(const Range& range);

    RangeIterator begin() noexcept;
    RangeIterator begin() const noexcept;
    RangeIterator end() noexcept;
    RangeIterator end() const noexcept;

    int a, b, step;
};