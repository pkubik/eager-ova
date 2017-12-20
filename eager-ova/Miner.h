#pragma once

#include <algorithm>
#include <iterator>
#include "Dataset.h"
#include "Types.h"


template <typename T>
inline std::vector<T> vectorIntersection(const std::vector<T>& v1, const std::vector<T>& v2)
{
	std::vector<T> result;
	result.reserve(std::min(v1.size(), v2.size()));
	std::set_intersection(
		v1.begin(), v1.end(), 
		v2.begin(), v2.end(),
		std::inserter(result, result.begin()));

	return result;
}


class Miner
{
public:
	struct Node
	{
		std::vector<Id> ids;
		Tidset tidset;
		std::vector<Node> children;
		Support support;
		std::vector<Support> classSupports;

		Node() = default;

		bool isSimplified() const
		{
			return tidset.size() == 0;
		}

		bool isRoot() const
		{
			return ids.size() == 0;
		}

		Support getSupport() const
		{
			if (isSimplified())
				return support;

			return tidset.size();
		}

		std::vector<Support> getClassSupports(const std::vector<Tidset>& classTidsets) const
		{
			if (!classSupports.empty())
				return classSupports;

			std::vector<Support> result;
			result.reserve(classTidsets.size());
			
			for (const auto& classTidset : classTidsets)
			{
				auto diff = vectorIntersection(tidset, classTidset);
				result.push_back(diff.size());
			}

			return result;
		}

		void simplify(const std::vector<Tidset>& classTidsets)
		{
			classSupports = getClassSupports(classTidsets);
			support = tidset.size();
			tidset.clear();
			tidset.shrink_to_fit();
		}
	};

	struct Params
	{
		double minRelSupport = 0.0;
	};

	Miner(const std::vector<Tidset>& classTidsets, const std::vector<std::pair<Id, Tidset>>& items)
		: classTidsets(classTidsets)
	{
		for (const auto& pair : items)
		{
			root.children.emplace_back();
			root.children.back().ids.push_back(pair.first);
			root.children.back().tidset = pair.second;
		}
	}

private:
	Node root;
	const std::vector<Tidset> classTidsets;
};
