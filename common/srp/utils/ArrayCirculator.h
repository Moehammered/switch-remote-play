#ifndef __ARRAYCIRCULATOR_H__
#define __ARRAYCIRCULATOR_H__

template<typename valueType, template<class...> class container, typename ...extraArgs>
class ArrayCirculator
{
public:
	ArrayCirculator(container<valueType, extraArgs...> const& c);

	void Increment();
	void Increment(long value);
	void Decrement();
	void Decrement(long value);

	bool Valid() const;

	valueType const Value() const;

	long SeekTo(valueType const& item);

	long Find(valueType const item) const;

	valueType const operator*() const;

private:
	container<valueType, extraArgs...> const& cont;
	long cursor;

	long MoveCursor(long val, long size);
};

template<typename valueType, template<class...> class container, typename ...extraArgs>
inline ArrayCirculator<valueType, container, extraArgs...>::ArrayCirculator(container<valueType, extraArgs...> const& c)
	: cont{ c }, cursor{ 0 }
{
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
inline void ArrayCirculator<valueType, container, extraArgs...>::Increment()
{
	cursor = MoveCursor(1, cont.size());
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
inline void ArrayCirculator<valueType, container, extraArgs...>::Increment(long value)
{
	cursor = MoveCursor(value, cont.size());
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
inline void ArrayCirculator<valueType, container, extraArgs...>::Decrement()
{
	cursor = MoveCursor(-1, cont.size());
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
inline void ArrayCirculator<valueType, container, extraArgs...>::Decrement(long value)
{
	cursor = MoveCursor(-value, cont.size());
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
inline bool ArrayCirculator<valueType, container, extraArgs...>::Valid() const
{
	return cont.size() != 0;
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
inline valueType const ArrayCirculator<valueType, container, extraArgs...>::Value() const
{
	auto itr = cont.begin();
	for (auto i = cursor; i > 0; --i)
		itr++;
	
	return *itr;
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
inline long ArrayCirculator<valueType, container, extraArgs...>::SeekTo(valueType const& item)
{
	auto tempCursor = 0L;

	for (auto const& a : cont)
	{
		if (a == item)
		{
			auto offset = tempCursor - cursor;
			cursor = tempCursor;
			return offset;
		}
		++tempCursor;
	}

	return 0;
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
inline long ArrayCirculator<valueType, container, extraArgs...>::Find(valueType const item) const
{
	auto tempCursor = 0L;

	for (auto const& a : cont)
	{
		if (a == item)
			return tempCursor - cursor;
		++tempCursor;
	}

	return 0;
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
inline valueType const ArrayCirculator<valueType, container, extraArgs...>::operator*() const
{
	return Value();
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
inline long ArrayCirculator<valueType, container, extraArgs...>::MoveCursor(long val, long size)
{
	auto next = cursor + val;
	next = next % size;
	if (next < 0)
		return size + next;
	else
		return next;
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
ArrayCirculator<valueType, container, extraArgs...>& operator++(ArrayCirculator<valueType, container, extraArgs...>& a)
{
	a.Increment();
	return a;
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
ArrayCirculator<valueType, container, extraArgs...> operator++(ArrayCirculator<valueType, container, extraArgs...>& a, int b)
{
	auto copy = ArrayCirculator<valueType, container, extraArgs...>(a);
	a.Increment();
	return copy;
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
ArrayCirculator<valueType, container, extraArgs...>& operator--(ArrayCirculator<valueType, container, extraArgs...>& a)
{
	a.Decrement();
	return a;
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
ArrayCirculator<valueType, container, extraArgs...> operator--(ArrayCirculator<valueType, container, extraArgs...>& a, int b)
{
	auto copy = ArrayCirculator<valueType, container, extraArgs...>(a);
	a.Decrement();
	return copy;
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
ArrayCirculator<valueType, container, extraArgs...> operator+(ArrayCirculator<valueType, container, extraArgs...> const & a, long b)
{
	auto copy = ArrayCirculator<valueType, container, extraArgs...>(a);
	copy.Increment(b);
	return copy;
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
ArrayCirculator<valueType, container, extraArgs...> operator-(ArrayCirculator<valueType, container, extraArgs...> const & a, long b)
{
	auto copy = ArrayCirculator<valueType, container, extraArgs...>(a);
	copy.Decrement(b);
	return copy;
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
void operator+=(ArrayCirculator<valueType, container, extraArgs...>& a, long b)
{
	a.Increment(b);
}

template<typename valueType, template<class...> class container, typename ...extraArgs>
void operator-=(ArrayCirculator<valueType, container, extraArgs...>& a, long b)
{
	a.Decrement(b);
}

#endif
