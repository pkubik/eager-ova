#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include "CounterDict.h"
#include "Types.h"
#include "CSVReader.h"

typedef std::vector<Id> Tidset;

class Dataset
{
public:
	explicit Dataset(std::vector<std::string> columnNames, const bool ignoreColumns = false, const int classColumn = -1)
		: columnNames(std::move(columnNames))
		, ignoreColumns(ignoreColumns)
		, classColumn(classColumn >= 0 ? classColumn : (static_cast<uint>(this->columnNames.size()) - 1))
	{}

	void append(const std::vector<std::string>& row)
	{
		for (uint i = 0; i < columnNames.size(); ++i)
		{
			if (columnNames[i].empty())
			{
				continue;
			}

			if (i != classColumn)
			{
				if (row[i].empty() || row[i] == "?" || row[i] == "NaN" || row[i] == "nan" || row[i] == "NAN")
				{
					continue;
				}
			}

			Id valueId;
			
			if (ignoreColumns)
			{
				valueId = valueEncoding.at(row[i]);
			}
			else
			{
				const auto& value = columnNames[i] + '-' + row[i];
				valueId = valueEncoding.at(value);
			}
			
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

	const auto& getColumnNames() const
	{
		return columnNames;
	}

	uint getNumberOfTransactions() const
	{
		return currentTid;
	}

	static Dataset fromFile(
		const std::string& path,
		const bool rawData = false,
		const std::string& columnUsage = "",
		const std::string& indexPath = "")
	{
		CSVReader reader(path, loadRowIndex(indexPath));
		
		std::vector<std::string> columnNames = reader.getColumnNames();
		int targetColumn = -1;

		if (columnUsage.size() == columnNames.size())
		{
			for (uint i = 0; i < columnUsage.size(); ++i)
			{
				if (columnUsage[i] == '0')
				{
					columnNames[i] = "";
				}
				if (columnUsage[i] == 'T')
				{
					targetColumn = i;
				}
			}
		}
		else
		{
			if (!columnUsage.empty())
			{
				logW << "Parameter 'columnUsage' do not match the number of the columns. It will be ignored";
			}
			columnNames = reader.getColumnNames();
		}
		
		Dataset dataset{ std::move(columnNames), rawData, targetColumn };

		while (!reader.isEOF())
		{
			dataset.append(reader.nextRow());
		}

		return dataset;
	}

private:
	std::vector<std::string> columnNames;
	const bool ignoreColumns;
	const uint classColumn;
	std::unordered_set<Id> requiredValues;
	std::unordered_map<Id, Tidset> items;
	CounterDict<std::string, Id> valueEncoding;
	std::vector<Id> columnByValue;
	Id currentTid = 0;
};
