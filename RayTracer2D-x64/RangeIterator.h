#pragma once
#include <iterator>


struct RangeIterator
{
    RangeIterator();
    RangeIterator(int i, int elem, int step = 1);
    RangeIterator(const RangeIterator& rangeIterator);

    const int& operator*() noexcept;

    RangeIterator& operator++() noexcept;

    bool operator==(const RangeIterator& other) const noexcept;
    bool operator!=(const RangeIterator& other) const noexcept;

    int operator-(const RangeIterator& other) noexcept;

    RangeIterator operator+(int i) const noexcept;

    int i, start, step, elem;
};