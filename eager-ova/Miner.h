#pragma once

#include <algorithm>
#include <stack>
#include <iterator>
#include <cassert>
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


template <typename T>
inline std::vector<T> vectorUnion(const std::vector<T>& v1, const std::vector<T>& v2)
{
	std::vector<T> result;
	result.reserve(std::min(v1.size(), v2.size()));
	std::set_union(
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
		struct SubsetIterator
		{
			explicit SubsetIterator(const Node& root, const std::vector<Id>& set)
				: set(set)
			{
				stack.push(&root);
			}

			const Node* next()
			{
				while (!stack.empty() && stack.top()->ids.size() + 1 < set.size())
				{
					const Node* current = stack.top();
					stack.pop();
					for (const auto& child : current->children)
					{
						if (child.ids.size() < set.size() &&
							std::includes(
								set.begin(), set.end(),
								child.ids.begin(), child.ids.end()))
						{
							stack.push(&child);
						}
					}
				}

				if (stack.empty())
				{
					return nullptr;
				}
				
				const Node* current = stack.top();
				stack.pop();
				return current;
			}

			std::vector<Id> set;
			std::stack<const Node*> stack;
		};

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

			return static_cast<Support>(tidset.size());
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
				result.push_back(static_cast<Support>(diff.size()));
			}

			return result;
		}

		void simplify(const std::vector<Tidset>& classTidsets)
		{
			classSupports = getClassSupports(classTidsets);
			support = static_cast<Support>(tidset.size());
			tidset.clear();
			tidset.shrink_to_fit();
		}

		void join(const Node& node)
		{
			assert(!isSimplified());

			children.emplace_back();
			children.back().ids = vectorUnion(ids, node.ids);
			children.back().tidset = vectorIntersection(tidset, node.tidset);

			// Check subset supports
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
