#pragma once

#include <utility>
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
	explicit RuleDecoder(std::vector<std::string> reversedEncoding)
		: reversedEncoding(std::move(reversedEncoding)) {}

	struct DecodedRule
	{
		DecodedRule(const Rule& rule, const std::vector<std::string>& reversedEncoding)
			: lhs(decodeVector(rule.lhs, reversedEncoding))
		    , rhs(reversedEncoding.at(rule.rhs))
			, originalRule(rule)
		{}

		std::vector<std::string> lhs;
		std::string rhs;
		const Rule& originalRule;
	};

	DecodedRule decode(const Rule& rule) const
	{
		return DecodedRule(rule, reversedEncoding);
	}

	std::string decodeToString(const DecodedRule& rule) const
	{
		std::string result;
		for (const auto& id : rule.lhs)
		{
			result += id + ", ";
		}
		result = result.substr(0, result.size() - 2) + "; " + rule.rhs;
		result += "; "
			+ std::to_string(rule.originalRule.lhsSupport)
			+ ", " + std::to_string(rule.originalRule.rhsSupport)
			+ ", " + std::to_string(rule.originalRule.support)
			+ ", " + std::to_string(rule.originalRule.confidence)
			+ ", " + std::to_string(rule.originalRule.growth);

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

class TableRuleDecoder
{
public:
	TableRuleDecoder(std::unordered_map<std::string, Id> encoding, std::vector<std::string> columnNames)
		: ruleDecoder(reverseEncoding(encoding)), encoding(std::move(encoding)), columnNames(std::move(columnNames)) {}

	std::pair<size_t, std::string> decomposeValue(const std::string& value) const
	{
		const auto delimPos = value.find('-');
		const auto columnName = value.substr(0, delimPos);
		auto cellValue = value.substr(delimPos + 1);

		auto columnIdx = std::find(columnNames.begin(), columnNames.end(), columnName) - columnNames.begin();

		return { columnIdx, cellValue };
	}

	std::string decodeToRow(const RuleDecoder::DecodedRule& rule) const
	{
		std::vector<std::string> values(columnNames.size());

		for (const auto& value : rule.lhs)
		{
			const auto pair = decomposeValue(value);
			values[pair.first] = pair.second;
		}

		{
			const auto pair = decomposeValue(rule.rhs);
			values[pair.first] = pair.second;
		}

		std::string result;
		for (const auto& value : values)
		{
			result += value + ", ";
		}
		result += std::to_string(rule.originalRule.lhsSupport)
			+ ", " + std::to_string(rule.originalRule.rhsSupport)
			+ ", " + std::to_string(rule.originalRule.support)
			+ ", " + std::to_string(rule.originalRule.confidence)
			+ ", " + std::to_string(rule.originalRule.growth);

		return result;
	}

	std::string decodeToRow(const Rule& rule) const
	{
		const auto decodedRule = ruleDecoder.decode(rule);
		return decodeToRow(decodedRule);
	}

	std::string headerString() const
	{
		std::string result;
		for (const auto& name : columnNames)
		{
			result += name + ", ";
		}
		result += "lhsSupport, rhsSupport, support, confidence, growth";

		return result;
	}

private:
	RuleDecoder ruleDecoder;
	const std::unordered_map<std::string, Id> encoding;
	const std::vector<std::string> columnNames;
};

class RuleWriter
{
public:
	RuleWriter(const std::string& path, const std::unordered_map<std::string, Id>& encoding)
		: decoder(reverseEncoding(encoding)), stream(path) {}

	void writeRule(const Rule& rule)
	{
		stream << decoder.decodeToString(rule) << "\n";
	}

private:
	RuleDecoder decoder;
	std::ofstream stream;
};

class TableRuleWriter
{
public:
	TableRuleWriter(const std::string& path, std::unordered_map<std::string, Id> encoding, std::vector<std::string> columnNames)
		: decoder(std::move(encoding), std::move(columnNames)), stream(path)
	{
		stream << decoder.headerString() << "\n";
	}

	void writeRule(const Rule& rule)
	{
		stream << decoder.decodeToRow(rule) << "\n";
	}

private:
	TableRuleDecoder decoder;
	std::ofstream stream;
};
