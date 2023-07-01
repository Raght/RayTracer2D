#include "Range.h"


Range::Range()
{
    a = 0;
    b = 1;
    step = 1;
}

Range::Range(int a, int b, int step)
{
    this->a = a;
    this->b = b;
    this->step = step;
};

Range::Range(const Range& range)
{
    a = range.a;
    b = range.b;
    step = range.step;
}

RangeIterator Range::begin() noexcept
{
    return RangeIterator(0, a, step);
}

RangeIterator Range::begin() const noexcept
{
    return RangeIterator(0, a, step);
}

RangeIterator Range::end() noexcept
{
    int steps = (b - a + step - 1) / step;

    return RangeIterator(steps, a + steps * step, step);
}

RangeIterator Range::end() const noexcept
{
    int steps = (b - a + step - 1) / step;

    return RangeIterator(steps, a + steps * step, step);
}
