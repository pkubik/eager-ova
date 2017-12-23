#pragma once

#include <vector>
#include <iterator>
#include <algorithm>
#include <stack>
#include "Types.h"
#include "Dataset.h"

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

	const Node* root = this;
	std::vector<Id> ids;
	Tidset tidset;
	std::vector<Node> children;
	Support support = 0;
	std::vector<Support> classSupports;
	std::vector<bool> isClassValid;

	Node() = default;
	explicit Node(const Node* root)
		: root(root)
	{}

	bool isSimplified() const { return tidset.size() == 0; }
	bool isRoot() const { return ids.size() == 0; }
	Support getSupport() const { return support; }
	void setTidset(Tidset&& value);
	void calculateClassSupports(const std::vector<Tidset>& classTidsets);
	void simplify();
	std::vector<Support> subsetsMinClassSupports(const std::vector<Id>& set) const;
	void join(const Node& node, const std::vector<Tidset>& classTidsets);
};
