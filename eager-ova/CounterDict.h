#pragma once

#include <unordered_map>

template <typename K, typename C>
class CounterDict
{
public:
	C at(const K& key)
	{
		auto insertion = map.insert({ key, counter });
		if (insertion.second)
		{
			++counter;
		}

		return insertion.first->second;
	}

	const std::unordered_map<K, C>& getDict() const
	{
		return map;
	}

private:
	std::unordered_map<K, C> map;
	
	/* Value to be assigned to a new element */
	C counter = 0;
};
