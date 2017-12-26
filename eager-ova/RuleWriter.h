#pragma once

#include <vector>
#include <string>
#include "Miner.h"

inline std::vector<std::string> decodeVector(const std::vector<Id>& vec, const std::vector<std::string>& reversedEncoding)
{
	std::vector<std::string> result;
	result.reserve(vec.size());

	for (auto id : vec)
	{
		result.push_back(reversedEncoding.at(id));
	}

	return result;
}

class RuleDecoder
{
public:
	explicit RuleDecoder(const std::vector<std::string>& reversedEncoding)
		: reversedEncoding(reversedEncoding) {}

	struct DecodedRule
	{
		DecodedRule(const Rule& rule, const std::vector<std::string>& reversedEncoding)
			: lhs(decodeVector(rule.lhs, reversedEncoding))
		    , rhs(reversedEncoding.at(rule.rhs))
			, support(rule.support)
		    , confidence(rule.confidence)
		{}

		std::vector<std::string> lhs;
		std::string rhs;
		Support support;
		double confidence;
	};

	DecodedRule decode(const Rule& rule) const
	{
		return DecodedRule(rule, reversedEncoding);
	}

private:
	const std::vector<std::string> reversedEncoding;
};
