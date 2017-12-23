#pragma once

#include <iterator>
#include "Dataset.h"
#include "Node.h"
#include "Types.h"

class Miner
{
public:
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
