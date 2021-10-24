#ifndef __RANGEDITERATOR_H__
#define __RANGEDITERATOR_H__

#include <assert.h>

template<typename T, typename U>
void cycleNumber(T& circ, U const& member)
{
	auto originalVal = *circ;
	++circ;
	while (*circ != originalVal && *circ != member)
		++circ;
}

template<typename number, number min, number max>
class NumberRange
{
public:
	NumberRange();
	NumberRange(number start);

	void Increment();
	void Increment(number amount);
	void Decrement();
	void Decrement(number amount);

	number const Value() const;

	number const operator *() const;
	number& operator *();

private:
	number value;
};

template<typename number, number min, number max>
inline NumberRange<number, min, max>::NumberRange()
	: value{ min }
{
	static_assert(min < max, "Invalid number range. Min must be less than max.");
}

template<typename number, number min, number max>
inline NumberRange<number, min, max>::NumberRange(number start)
	: value{ start }
{
	static_assert(min < max, "Invalid number range. Min must be less than max.");
	if (value > max)
		value = max;
	else if (value < min)
		value = min;
}

template<typename number, number min, number max>
inline void NumberRange<number, min, max>::Increment()
{
	++value;
	if (value > max || value < min)
		value = min;
}

template<typename number, number min, number max>
inline void NumberRange<number, min, max>::Increment(number amount)
{
	value += amount;
	if (value > max)
	{
		auto diff = value - max;
		value = min + diff;
	}
	else if (value < min)
	{
		auto diff = min - value;
		value = max - diff;
	}
}

template<typename number, number min, number max>
inline void NumberRange<number, min, max>::Decrement()
{
	--value;
	if (value < min || value > max)
		value = max;
}

template<typename number, number min, number max>
inline void NumberRange<number, min, max>::Decrement(number amount)
{
	value -= amount;
	if (value > max)
	{
		auto diff = value - max;
		value = min + diff;
	}
	else if (value < min)
	{
		auto diff = min - value;
		value = max - diff;
	}
}

template<typename number, number min, number max>
inline number const NumberRange<number, min, max>::Value() const
{
	return value;
}

template<typename number, number min, number max>
inline number const NumberRange<number, min, max>::operator*() const
{
	return Value();
}

template<typename number, number min, number max>
inline number& NumberRange<number, min, max>::operator*()
{
	return value;
}

template<typename number, number min, number max>
inline NumberRange<number, min, max>& operator++(NumberRange<number, min, max>& a)
{
	a.Increment();
	return a;
}

template<typename number, number min, number max>
inline NumberRange<number, min, max> operator++(NumberRange<number, min, max>& a, int)
{
	auto temp = NumberRange<number, min, max>(a);
	a.Increment();
	return temp;
}

template<typename number, number min, number max>
inline NumberRange<number, min, max>& operator--(NumberRange<number, min, max>& a)
{
	a.Decrement();
	return a;
}

template<typename number, number min, number max>
inline NumberRange<number, min, max> operator--(NumberRange<number, min, max>& a, int)
{
	auto temp = NumberRange<number, min, max>(a);
	a.Decrement();
	return temp;
}

template<typename number, number min, number max>
inline void operator+=(NumberRange<number, min, max>& a, number b)
{
	a.Increment(b);
}

template<typename number, number min, number max>
inline void operator-=(NumberRange<number, min, max>& a, number b)
{
	a.Decrement(b);
}

template<typename number, number min, number max>
inline void operator*=(NumberRange<number, min, max>& a, number b)
{
	auto result = *a * b;
	auto diff = result - a.Value();
	a.Increment(diff);
}

template<typename number, number min, number max>
inline void operator/=(NumberRange<number, min, max>& a, number b)
{
	auto result = *a / b;
	auto diff = result - a.Value();
	a.Increment(diff);
}

template<typename number, number min, number max>
inline NumberRange<number, min, max> operator+(NumberRange<number, min, max> const & a, number b)
{
	auto temp = NumberRange<number, min, max>(a);
	temp.Increment(b);
	return temp;
}

template<typename number, number min, number max>
inline NumberRange<number, min, max> operator-(NumberRange<number, min, max> const& a, number b)
{
	auto temp = NumberRange<number, min, max>(a);
	temp.Decrement(b);
	return temp;
}

template<typename number, number min, number max>
inline NumberRange<number, min, max> operator*(NumberRange<number, min, max> const& a, number b)
{
	auto temp = NumberRange<number, min, max>(a);
	auto result = *a * b;
	auto diff = result - a.Value();
	temp.Increment(diff);
	return temp;
}

template<typename number, number min, number max>
inline NumberRange<number, min, max> operator/(NumberRange<number, min, max> const& a, number b)
{
	auto temp = NumberRange<number, min, max>(a);
	auto result = *a / b;
	auto diff = result - a.Value();
	temp.Increment(diff);
	return temp;
}

template<typename number, number min, number max, typename other>
inline other operator+(other a, NumberRange<number, min, max> const& b)
{
	return a + *b;
}

template<typename number, number min, number max, typename other>
inline other operator-(other a, NumberRange<number, min, max> const& b)
{
	return a - *b;
}

template<typename number, number min, number max, typename other>
inline other operator*(other a, NumberRange<number, min, max> const& b)
{
	return a * b.Value();
}

template<typename number, number min, number max, typename other>
inline other operator/(other a, NumberRange<number, min, max> const& b)
{
	return a / b.Value();
}

#endif