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

		const Node* root = this;
		std::vector<Id> ids;
		Tidset tidset;
		std::vector<Node> children;
		Support support = 0;
		std::vector<Support> classSupports;

		Node() = default;
		explicit Node(const Node* root)
			: root(root)
		{}

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
			return support;
		}

		void setTidset(Tidset&& value)
		{
			tidset = value;
			support = static_cast<Support>(tidset.size());
		}

		void calculateClassSupports(const std::vector<Tidset>& classTidsets)
		{
			classSupports.reserve(classTidsets.size());
			
			for (const auto& classTidset : classTidsets)
			{
				auto diff = vectorIntersection(tidset, classTidset);
				classSupports.push_back(static_cast<Support>(diff.size()));
			}
		}

		void simplify(const std::vector<Tidset>& classTidsets)
		{
			tidset.clear();
			tidset.shrink_to_fit();
		}

		std::vector<Support> subsetsMinClassSupports(const std::vector<Id>& set) const
		{
			auto result = root->classSupports;
			SubsetIterator si{ *root, set };
			for (const Node* node = si.next(); node != nullptr; node = si.next())
			{
				for (uint i = 0; i < result.size(); ++i)
				{
					result[i] = std::min(result[i], node->classSupports[i]);
				}
			}
			return result;
		}

		void join(const Node& node, const std::vector<Tidset>& classTidsets)
		{
			assert(!isSimplified());

			children.emplace_back(root);
			Node& child = children.back();
			child.ids = vectorUnion(ids, node.ids);
			child.setTidset(vectorIntersection(tidset, node.tidset));
			child.calculateClassSupports(classTidsets);

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
		for (const auto& classTidset : classTidsets)
		{
			auto classSupport = static_cast<Support>(classTidset.size());
			root.support += classSupport;
			root.classSupports.push_back(classSupport);
		}

		for (const auto& pair : items)
		{
			root.children.emplace_back(&root);
			root.children.back().ids.push_back(pair.first);
			root.children.back().tidset = pair.second;
		}
	}

private:
	Node root;
	const std::vector<Tidset> classTidsets;
};
