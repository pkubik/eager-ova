#pragma once

#include <iterator>
#include "Dataset.h"
#include "Node.h"
#include "Types.h"

inline Node createRoot(const std::vector<Tidset>& classTidsets)
{
	Node root;
	root.classValidity = std::valarray<bool>(true, classTidsets.size());
	for (const auto& classTidset : classTidsets)
	{
		auto classSupport = static_cast<Support>(classTidset.size());
		root.support += classSupport;
		root.classSupports.push_back(classSupport);
	}

	return root;
}

struct Rule
{
	std::vector<Id> lhs;
	Id rhs;
	Support support;
	double confidence;
	double growth;
};

inline Node::UniquePtr createRootChild(const Node& root, const Id id, Tidset&& tidset, const std::vector<Tidset>& classTidsets)
{
	auto node = std::make_unique<Node>(&root);
	node->ids.push_back(id);
	node->setTidset(std::move(tidset));
	node->calculateClassSupports(classTidsets);
	node->classValidity = root.classValidity;

	return node;
}

inline void addRootChild(Node& root, const Id id, Tidset&& tidset, const std::vector<Tidset>& classTidsets)
{
	auto node = createRootChild(root, id, std::move(tidset), classTidsets);

	if (node->isGenerator())
	{
		root.children.emplace_back(std::move(node));
	}
}

inline double ruleGrowth(const Support const supX, const Support supY, const Support supXY, const Support dbSize)
{
	const Support supNotY = dbSize - supY;
	const Support supXNotY = supX - supXY;

	return static_cast<double>(supXY) * supNotY / supXNotY / supY;
}

class Miner
{
public:
	struct Params
	{
		double minRelSupport = 0.0;
		bool cp = true;
		double growthThreshold = 0.0;
	};

	Miner(const std::vector<Id>& classIds)
		: classIds(classIds)
	{}

	std::vector<Rule> mine(const std::unordered_map<Id, Tidset>& items)
	{
		for (auto id : classIds)
		{
			classTidsets.push_back(items.at(id));
			dbSize += static_cast<Support>(classTidsets.back().size());
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
					
					if (tmp->isGenerator())
					{
						evaluateNode(*tmp, results);

						if (tmp->isAnyClassValid())
						{
							lnode->children.push_back(std::move(tmp));
							stack.push(lnode->children.back().get());
						}
					}
				}
				lnode->simplify();
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
				else
				{
					double confidenceValue = static_cast<double>(node.classSupports[i]) / node.support;
					double growthValue = growth(node.support, node.classSupports[i], i);

					if ((params.cp && confidenceValue == 1.0) ||
						(!params.cp && growthValue > params.growthThreshold))
					{
						node.classValidity[i] = false;
						rules.emplace_back();
						auto& rule = rules.back();
						rule.lhs = node.ids;
						rule.rhs = classIds[i];
						rule.support = node.support;
						rule.confidence = confidenceValue;
						rule.growth = growthValue;
						node.classValidity[i] = false;
					}
				}
			}
		}
	}

	double growth(const Support supX, const Support supXY, const int classIdx) const
	{
		const Support supY = static_cast<Support>(classTidsets[classIdx].size());
		return ruleGrowth(supX, supY, supXY, dbSize);
	}

	Params params;

private:
	Node root;
	const std::vector<Id> classIds;
	std::vector<Tidset> classTidsets;
	Support dbSize = 0;
};
