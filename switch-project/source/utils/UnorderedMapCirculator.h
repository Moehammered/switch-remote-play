#ifndef __UNORDEREDMAPCIRCULATOR_H__
#define __UNORDEREDMAPCIRCULATOR_H__

#include <unordered_map>

template<typename T, typename U>
void cycleMap(T& circ, U const & member)
{
	auto originalVal = circ.KeyPair().first;
	++circ;
	while (circ.KeyPair().first != originalVal && circ.KeyPair().first != member)
		++circ;
}

template<typename key, typename value, typename ...extraArgs>
class UnorderedMapCirculator
{
public:
	UnorderedMapCirculator(std::unordered_map<key, value, extraArgs...> const& c);

	void Increment();
	void Increment(long val);
	void Decrement();
	void Decrement(long val);

	bool Valid() const;

	value const Value() const;

	long SeekTo(value const& item);
	long SeekTo(key const& item);

	long Find(value const item) const;
	long Find(key const item) const;

	value const operator*() const;

	std::pair<key, value> const KeyPair() const;

private:
	std::unordered_map<key, value, extraArgs...> const& cont;
	long cursor;

	long MoveCursor(long val, long size);
};

template<typename key, typename value, typename ...extraArgs>
inline UnorderedMapCirculator<key, value, extraArgs...>::UnorderedMapCirculator(std::unordered_map<key, value, extraArgs...> const& c)
	: cont{ c }, cursor{ 0 }
{
}

template<typename key, typename value, typename ...extraArgs>
inline void UnorderedMapCirculator<key, value, extraArgs...>::Increment()
{
	cursor = MoveCursor(1, cont.size());
}

template<typename key, typename value, typename ...extraArgs>
inline void UnorderedMapCirculator<key, value, extraArgs...>::Increment(long val)
{
	/*cursor += val;
	auto size = (long)cont.size();
	if (cursor >= size)
		cursor = cursor % size;
	else if (cursor < 0)
	{
		auto backStep = -cursor % size;
		cursor = size % (size - backStep);
	}*/
	cursor = MoveCursor(val, cont.size());
}

template<typename key, typename value, typename ...extraArgs>
inline void UnorderedMapCirculator<key, value, extraArgs...>::Decrement()
{
	/*if (--cursor < 0)
		cursor = cont.size() - 1;*/
	cursor = MoveCursor(-1, cont.size());
}

template<typename key, typename value, typename ...extraArgs>
inline void UnorderedMapCirculator<key, value, extraArgs...>::Decrement(long val)
{
	/*cursor -= val;
	auto size = (long)cont.size();
	if (cursor >= size)
		cursor = cursor % size;
	else if (cursor < 0)
	{
		auto backStep = -cursor % size;
		cursor = (size - backStep) % size;
	}*/
	cursor = MoveCursor(-val, cont.size());
}

template<typename key, typename value, typename ...extraArgs>
inline bool UnorderedMapCirculator<key, value, extraArgs...>::Valid() const
{
	return cont.size() != 0;
}

template<typename key, typename value, typename ...extraArgs>
inline value const UnorderedMapCirculator<key, value, extraArgs...>::Value() const
{
	auto itr = cont.begin();
	for (auto i = cursor; i > 0; --i)
		++itr;

	return itr->second;
}

template<typename key, typename value, typename ...extraArgs>
inline long UnorderedMapCirculator<key, value, extraArgs...>::SeekTo(value const& item)
{
	auto tempCursor = 0L;

	for (auto const& pair : cont)
	{
		if (pair.second == item)
		{
			auto offset = tempCursor - cursor;
			cursor = tempCursor;
			return offset;
		}
		++tempCursor;
	}

	return 0;
}

template<typename key, typename value, typename ...extraArgs>
inline long UnorderedMapCirculator<key, value, extraArgs...>::SeekTo(key const& item)
{
	auto tempCursor = 0L;

	for (auto const& pair : cont)
	{
		if (pair.first == item)
		{
			auto offset = tempCursor - cursor;
			cursor = tempCursor;
			return offset;
		}
		++tempCursor;
	}

	return 0;
}

template<typename key, typename value, typename ...extraArgs>
inline long UnorderedMapCirculator<key, value, extraArgs...>::Find(value const item) const
{
	auto tempCursor = 0L;

	for (auto const& pair : cont)
	{
		if (pair.second == item)
			return tempCursor - cursor;
		++tempCursor;
	}

	return 0;
}

template<typename key, typename value, typename ...extraArgs>
inline long UnorderedMapCirculator<key, value, extraArgs...>::Find(key const item) const
{
	auto tempCursor = 0L;

	for (auto const& pair : cont)
	{
		if (pair.first == item)
			return tempCursor - cursor;
		++tempCursor;
	}

	return 0;
}

template<typename key, typename value, typename ...extraArgs>
inline value const UnorderedMapCirculator<key, value, extraArgs...>::operator*() const
{
	return Value();
}

template<typename key, typename value, typename ...extraArgs>
inline std::pair<key, value> const UnorderedMapCirculator<key, value, extraArgs...>::KeyPair() const
{
	auto itr = cont.begin();
	for (auto i = cursor; i > 0; --i)
		++itr;

	return *itr;
}

template<typename key, typename value, typename ...extraArgs>
inline long UnorderedMapCirculator<key, value, extraArgs...>::MoveCursor(long val, long size)
{
	auto next = cursor + val;
	next = next % size;
	if (next < 0)
		return size + next;
	else
		return next;
}


template<typename key, typename value, typename ...extraArgs>
inline UnorderedMapCirculator<key, value, extraArgs...>& operator++(UnorderedMapCirculator<key, value, extraArgs...>& a)
{
	a.Increment();
	return a;
}

template<typename key, typename value, typename ...extraArgs>
inline UnorderedMapCirculator<key, value, extraArgs...> operator++(UnorderedMapCirculator<key, value, extraArgs...>& a, int)
{
	auto copy = UnorderedMapCirculator<key, value, extraArgs...>&(a);
	a.Increment();
	return copy;
}

template<typename key, typename value, typename ...extraArgs>
inline UnorderedMapCirculator<key, value, extraArgs...>& operator--(UnorderedMapCirculator<key, value, extraArgs...>& a)
{
	a.Decrement();
	return a;
}

template<typename key, typename value, typename ...extraArgs>
inline UnorderedMapCirculator<key, value, extraArgs...> operator--(UnorderedMapCirculator<key, value, extraArgs...>& a, int)
{
	auto copy = UnorderedMapCirculator<key, value, extraArgs...>(a);
	a.Decrement();
	return copy;
}

template<typename key, typename value, typename ...extraArgs>
inline UnorderedMapCirculator<key, value, extraArgs...> operator+(UnorderedMapCirculator<key, value, extraArgs...> const & a, long val)
{
	auto copy = UnorderedMapCirculator<key, value, extraArgs...>(a);
	copy.Increment(val);
	return copy;
}

template<typename key, typename value, typename ...extraArgs>
inline UnorderedMapCirculator<key, value, extraArgs...> operator-(UnorderedMapCirculator<key, value, extraArgs...>& a, long val)
{
	auto copy = UnorderedMapCirculator<key, value, extraArgs...>(a);
	copy.Decrement(val);
	return copy;
}

template<typename key, typename value, typename ...extraArgs>
inline void operator+=(UnorderedMapCirculator<key, value, extraArgs...>& a, long b)
{
	a.Increment(b);
}

template<typename key, typename value, typename ...extraArgs>
inline void operator-=(UnorderedMapCirculator<key, value, extraArgs...>& a, long b)
{
	a.Decrement(b);
}

#endif
