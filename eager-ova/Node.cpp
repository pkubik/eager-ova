#include <cassert>
#include "Node.h"

void Node::setTidset(Tidset&& value)
{
	tidset = value;
	support = static_cast<Support>(tidset.size());
}

void Node::calculateClassSupports(const std::vector<Tidset>& classTidsets)
{
	classSupports.reserve(classTidsets.size());

	for (const auto& classTidset : classTidsets)
	{
		auto diff = vectorIntersection(tidset, classTidset);
		classSupports.push_back(static_cast<Support>(diff.size()));
	}
}

std::vector<Support> Node::subsetsMinClassSupports(const std::vector<Id>& set) const
{
	auto result = classSupports;
	Node::SubsetIterator si{ *this, set };
	for (const Node* node = si.next(); node != nullptr; node = si.next())
	{
		for (uint i = 0; i < result.size(); ++i)
		{
			result[i] = std::min(result[i], node->classSupports[i]);
		}
	}
	return result;
}

void Node::simplify()
{
	tidset.clear();
	tidset.shrink_to_fit();
}

Node Node::join(const Node& node, const std::vector<Tidset>& classTidsets) const
{
	assert(!isSimplified());

	Node child = Node{ root };
	child.ids = vectorUnion(ids, node.ids);
	child.setTidset(vectorIntersection(tidset, node.tidset));
	child.calculateClassSupports(classTidsets);
	child.classValidity = classValidity;

	return child;
}

bool Node::isAnyClassValid() const
{
	return std::any_of(classValidity.begin(), classValidity.end(), [](bool value) { return value; });
}