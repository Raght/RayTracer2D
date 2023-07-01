#include <iterator>


struct Range
{
	//class iterator : public std::iterator_traits<std::random_access_iterator_tag>
	//{
	//	int num = a;
	//};
	
	Range();
	Range(int a, int b);
    Range(const Range& range);

	RangeIterator begin() const noexcept;
	RangeIterator end() const noexcept;


	int a, b;
};

//template <class type>
struct RangeIterator
{
    //using difference_type = int;
    //using value_type = int;
    //using pointer = const int*;
    //using reference = const int&;
    //using iterator_category = std::forward_iterator_tag;
    
	//using pointer = const type*;
	//using reference = const type&;

	RangeIterator(const Range& range, int i);
    RangeIterator(RangeIterator&& rangeIterator);
	RangeIterator(const RangeIterator& rangeIterator);

    const int& operator*() noexcept;
    const int& operator*() const noexcept;
    const int* operator&() noexcept;
    const int* operator&() const noexcept;
    const int* operator->() noexcept;
    const int* operator->() const noexcept;

    RangeIterator& operator++() noexcept;
    RangeIterator& operator--() noexcept;
    RangeIterator& operator+=(const int n) noexcept;
    RangeIterator& operator-=(const int n) noexcept;
    RangeIterator operator+(const int n) const noexcept;
    RangeIterator operator-(const int n) const noexcept;
    int operator-(const RangeIterator& rangeIterator) const noexcept;

    bool operator==(const RangeIterator& other) const noexcept;
    bool operator!=(const RangeIterator& other) const noexcept;

    int* _Unwrapped() noexcept;

    //_NODISCARD _CONSTEXPR20 reference operator[](const difference_type _Off) const noexcept {
    //    return const_cast<reference>(_Mybase::operator[](_Off));
    //}

    //_CONSTEXPR20 _Vector_iterator operator++(int) noexcept {
    //    _Vector_iterator _Tmp = *this;
    //    _Mybase::operator++();
    //    return _Tmp;
    //}

    //_CONSTEXPR20 _Vector_iterator& operator--() noexcept {
    //    _Mybase::operator--();
    //    return *this;
    //}

    //_CONSTEXPR20 _Vector_iterator operator--(int) noexcept {
    //    _Vector_iterator _Tmp = *this;
    //    _Mybase::operator--();
    //    return _Tmp;
    //}

    //_CONSTEXPR20 _Vector_iterator& operator+=(const difference_type _Off) noexcept {
    //    _Mybase::operator+=(_Off);
    //    return *this;
    //}

    //_NODISCARD _CONSTEXPR20 _Vector_iterator operator+(const difference_type _Off) const noexcept {
    //    _Vector_iterator _Tmp = *this;
    //    _Tmp += _Off;
    //    return _Tmp;
    //}

    //_CONSTEXPR20 _Vector_iterator& operator-=(const difference_type _Off) noexcept {
    //    _Mybase::operator-=(_Off);
    //    return *this;
    //}

    //using _Mybase::operator-;
    //
    //_NODISCARD _CONSTEXPR20 _Vector_iterator operator-(const difference_type _Off) const noexcept {
    //    _Vector_iterator _Tmp = *this;
    //    _Tmp -= _Off;
    //    return _Tmp;
    //}
    //
    //_NODISCARD _CONSTEXPR20 reference operator[](const difference_type _Off) const noexcept {
    //    return const_cast<reference>(_Mybase::operator[](_Off));
    //}

	Range range;
	int i;
	int elem;
};