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

/**
 * \brief Creates reversed encoding for decoding numeric identifiers
 * \param encoding Original string -> int encoding. Integers must form a {0, ..., N-1} sequence
 * \return Reversed input encoding
 */
inline std::vector<std::string> reverseEncoding(const std::unordered_map<std::string, Id>& encoding)
{
	std::vector<std::string> result;
	result.resize(encoding.size());

	for (const auto& pair : encoding)
	{
		result[pair.second] = pair.first;
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

	std::string decodeToString(const DecodedRule& rule) const
	{
		std::string result = "";
		for (const auto& id : rule.lhs)
		{
			result += id + ", ";
		}
		result = result.substr(0, result.size() - 2) + " -> " + rule.rhs;
		result += "; " + std::to_string(rule.support) + ", " + std::to_string(rule.confidence);

		return result;
	}

	std::string decodeToString(const Rule& rule) const
	{
		const auto decodedRule = decode(rule);
		return decodeToString(decodedRule);
	}

private:
	const std::vector<std::string> reversedEncoding;
};
