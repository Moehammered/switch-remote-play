#ifndef __CIRCULATOR_H__
#define __CIRCULATOR_H__

template<class valueType, class container>
class Circulator
{
public:
	Circulator(container const& col);
	void Increment() const;
	void Decrement() const;

	Circulator & operator ++();
	Circulator operator ++(int);
	Circulator & operator --();
	Circulator operator --(int);

	valueType const operator *() const;

	valueType const Value() const;

private:
	container const& cont;
	mutable int cursor;
};


template<class valueType, class container>
inline Circulator<valueType, container>::Circulator(container const& col)
	: cont{ col }
{
	cursor = 0;
}

template<class valueType, class container>
inline void Circulator<valueType, container>::Increment() const
{
	if (cont.size() > 0)
	{
		++cursor;
		if (cursor >= cont.size())
			cursor = 0;
	}
}


template<class valueType, class container>
inline void Circulator<valueType, container>::Decrement() const
{
	if (cont.size() > 0)
	{
		--cursor;
		if (cursor < 0)
			cursor = cont.size() - 1;
	}
}

template<class valueType, class container>
inline Circulator<valueType, container> & Circulator<valueType, container>::operator++()
{
	Increment();
	return *this;
}

template<class valueType, class container>
inline Circulator<valueType, container> Circulator<valueType, container>::operator++(int)
{
	auto temp = Circulator(this[0]);
	Increment();
	return temp;
}

template<class valueType, class container>
inline Circulator<valueType, container> & Circulator<valueType, container>::operator--()
{
	Decrement();
	return *this;
}

template<class valueType, class container>
inline Circulator<valueType, container> Circulator<valueType, container>::operator--(int)
{
	auto temp = Circulator(this[0]);
	Decrement();
	return temp;
}

template<class valueType, class container>
inline valueType const Circulator<valueType, container>::operator*() const
{
	return Value();
}

template<class valueType, class container>
inline valueType const Circulator<valueType, container>::Value() const
{
	auto itr = cont.begin();
	for (auto i = cursor; i > 0; --i)
		++itr;
	return *itr;
}

#endif
