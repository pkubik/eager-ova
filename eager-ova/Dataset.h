#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include "CounterDict.h"
#include "Types.h"

typedef std::vector<Id> Tidset;

class Dataset
{
public:
	explicit Dataset(const std::vector<std::string>& columnNames, const int classColumn = -1)
		: columnNames(columnNames), classColumn(classColumn >= 0 ? classColumn : (static_cast<uint>(columnNames.size()) - 1))
	{}

	void append(const std::vector<std::string>& row)
	{
		for (uint i = 0; i < columnNames.size(); ++i)
		{
			const auto& value = columnNames[i] + '-' + row[i];
			const auto valueId = valueEncoding.at(value);
			
			// If new value Id appeared
			if (valueId == columnByValue.size())
			{
				columnByValue.push_back(i);
			}

			if (i == classColumn)
			{
				requiredValues.insert(valueId);
			}

			items[valueId].push_back(currentTid);
		}
		++currentTid;
	}

	const auto& getRequiredValues() const
	{
		return requiredValues;
	}

	const auto& getValueEncoding() const
	{
		return valueEncoding.getDict();
	}

	const auto& getItems() const
	{
		return items;
	}

	const auto& getColumnsByValues() const
	{
		return columnByValue;
	}

private:
	std::vector<std::string> columnNames;
	const uint classColumn;
	std::unordered_set<Id> requiredValues;
	std::unordered_map<Id, Tidset> items;
	CounterDict<std::string, Id> valueEncoding;
	std::vector<Id> columnByValue;
	Id currentTid = 0;
};