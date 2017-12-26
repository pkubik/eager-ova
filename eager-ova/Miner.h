#pragma once

#include <iterator>
#include "Dataset.h"
#include "Node.h"
#include "Types.h"

inline Node createRoot(const std::vector<Tidset>& classTidsets)
{
	Node root;
	for (const auto& classTidset : classTidsets)
	{
		auto classSupport = static_cast<Support>(classTidset.size());
		root.support += classSupport;
		root.classSupports.push_back(classSupport);
		root.classValidity.push_back(true);
	}

	return root;
}

struct Rule
{
	std::vector<Id> lhs;
	Id rhs;
	Support support;
	double confidence;
};

inline Node::UniquePtr createRootChild(const Node& root, const Id id, Tidset&& tidset, const std::vector<Tidset>& classTidsets)
{
	auto node = std::make_unique<Node>(&root);
	node->ids.push_back(id);
	node->setTidset(std::move(tidset));
	node->calculateClassSupports(classTidsets);
	node->classValidity = root.classValidity;
	node->invalidateNonGenerators();

	return node;
}

inline void addRootChild(Node& root, const Id id, Tidset&& tidset, const std::vector<Tidset>& classTidsets)
{
	auto node = createRootChild(root, id, std::move(tidset), classTidsets);

	if (node->isAnyClassValid())
	{
		root.children.emplace_back(std::move(node));
	}
}

class Miner
{
public:
	struct Params
	{
		double minRelSupport = 0.0;
	};

	Miner(const std::vector<Id>& classIds)
		: classIds(classIds)
	{}

	std::vector<Rule> mine(const std::unordered_map<Id, Tidset>& items)
	{
		for (auto id : classIds)
		{
			classTidsets.push_back(items.at(id));
		}
		root = createRoot(classTidsets);

		std::vector<Rule> results;

		for (const auto& pair : items)
		{
			if (!std::any_of(classIds.begin(), classIds.end(), [&](Id id) {return id == pair.first; }))
			{
				auto rootChild = createRootChild(root, pair.first, Tidset{ pair.second }, classTidsets);
				// We allow root children to not represent generators
				evaluateNode(*rootChild, results);
				if (rootChild->isAnyClassValid())
				{
					root.children.push_back(std::move(rootChild));
				}
			}
		}

		std::stack<Node*> stack;
		stack.push(&root);

		while (!stack.empty())
		{
			Node* current = stack.top();
			stack.pop();
			for (auto cit = current->children.begin(); cit != current->children.end(); ++cit)
			{
				Node* lnode = cit->get();
				for (auto sit = cit + 1; sit != current->children.end(); ++sit)
				{
					Node* rnode = sit->get();
					auto tmp = lnode->join(*rnode, classTidsets);
					tmp->invalidateNonGenerators();

					evaluateNode(*tmp, results);

					if (tmp->isAnyClassValid())
					{
						lnode->children.push_back(std::move(tmp));
						stack.push(lnode->children.back().get());
					}
				}

			}
		}

		root = createRoot(classTidsets);
		return results;
	}

	void evaluateNode(Node& node, std::vector<Rule>& rules) const
	{
		for (uint i = 0; i < classIds.size(); ++i)
		{
			if (node.classValidity[i])
			{
				if ((static_cast<double>(node.support) / root.support) <= params.minRelSupport)
				{
					node.classValidity[i] = false;
				}
				else if (node.support == node.classSupports[i]) // confidence == 1
				{
					rules.emplace_back();
					auto& rule = rules.back();
					rule.lhs = node.ids;
					rule.rhs = classIds[i];
					rule.support = node.support;
					rule.confidence = 1.0;
					node.classValidity[i] = false;
				}
			}
		}
	}

	Params params;

private:
	Node root;
	const std::vector<Id> classIds;
	std::vector<Tidset> classTidsets;
};
