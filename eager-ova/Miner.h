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

inline void addRootChild(Node& root, const Id id, Tidset&& tidset, const std::vector<Tidset>& classTidsets)
{
	root.children.emplace_back(&root);
	root.children.back().ids.push_back(id);
	root.children.back().setTidset(std::move(tidset));
	root.children.back().calculateClassSupports(classTidsets);
	root.children.back().classValidity = root.classValidity;
}

class Miner
{
public:
	struct Params
	{
		double minRelSupport = 0.0;
	};

	Miner(const std::vector<Tidset>& classTidsets, const std::vector<std::pair<Id, Tidset>>& items)
		: root{ createRoot(classTidsets) }, classTidsets{ classTidsets }
	{
		for (const auto& pair : items)
		{
			addRootChild(root, pair.first, Tidset{ pair.second }, classTidsets);
		}
	}

private:
	Node root;
	const std::vector<Tidset> classTidsets;
};
