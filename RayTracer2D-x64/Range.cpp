#include "Range.h"


Range::Range()
{
    a = 0;
    b = 1;
}

Range::Range(int a, int b)
{
    this->a = a;
    this->b = b;
};

Range::Range(const Range& range)
{
    a = range.a;
    b = range.b;
}

RangeIterator Range::begin() const noexcept
{
	return RangeIterator(*this, 0);
}

RangeIterator Range::end() const noexcept
{
	return RangeIterator(*this, b - a);
}


RangeIterator::RangeIterator(const Range& range, int i)
{
    this->range = range;
    this->i = i;
    elem = range.a + i;
}

RangeIterator::RangeIterator(const RangeIterator& rangeIterator)
{
    range = rangeIterator.range;
    i = rangeIterator.i;
    elem = rangeIterator.elem;
}

RangeIterator::RangeIterator(RangeIterator&& rangeIterator)
{
    range = rangeIterator.range;
    i = rangeIterator.i;
    elem = rangeIterator.elem;
}

const int& RangeIterator::operator*() noexcept { return elem; }
const int& RangeIterator::operator*() const noexcept { return elem; }
const int* RangeIterator::operator&() noexcept { return &elem; }
const int* RangeIterator::operator&() const noexcept { return &elem; }
const int* RangeIterator::operator->() noexcept { return &elem; }
const int* RangeIterator::operator->() const noexcept { return &elem; }

RangeIterator& RangeIterator::operator++() noexcept
{
    i++;
    elem++;
    return *this;
}

RangeIterator& RangeIterator::operator--() noexcept
{
    i--;
    elem--;
    return *this;
}

RangeIterator& RangeIterator::operator+=(const int n) noexcept
{
    if (elem + n >= range.b)
    {
        i = range.b - range.a;
        elem = range.b;
        return *this;
    }

    i += n;
    elem += n;
    return *this;
}

RangeIterator& RangeIterator::operator-=(const int n) noexcept
{
    this->operator+=(-n);
    return *this;
}

RangeIterator RangeIterator::operator+(const int n) const noexcept
{
    RangeIterator temp = *this;
    temp += n;
    return temp;
}

RangeIterator RangeIterator::operator-(const int n) const noexcept
{
    RangeIterator temp = *this;
    temp -= n;
    return temp;
}

int RangeIterator::operator-(const RangeIterator& rangeIterator) const noexcept
{
    return (*this).elem - rangeIterator.elem;
}

bool RangeIterator::operator==(const RangeIterator& other) const noexcept
{
    return (*this).range.a == other.range.a && (*this).range.b == other.range.b && (*this).elem == other.elem;
}

bool RangeIterator::operator!=(const RangeIterator& other) const noexcept
{
    return !this->operator==(other);
}

int* RangeIterator::_Unwrapped() noexcept
{
    return &elem;
}