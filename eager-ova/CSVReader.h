#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include "Types.h"
#include "IOUtils.h"
#include "CounterDict.h"


/*
 * CSV header is required
 */
class CSVReader
{
public:
	typedef std::vector<Item> Row;

	explicit CSVReader(const std::string& path)
		: path(path), stream(path)
	{
		std::string cell;

		std::getline(stream, lineBuffer);
		std::istringstream iss(lineBuffer);
		while (std::getline(iss, cell, ','))
		{
			columnNames.push_back(cell);
		}

		loadNextRow();
	}

	Row nextRow()
	{
		if (isEOF())
		{
			logW << "No more rows to fetch for " << path << "." << std::endl;
			throw std::logic_error("No more rows to fetch.");
		}

		std::string cell;
		Row row(columnNames.size());

		++lineNumber;
		std::istringstream iss(lineBuffer);
		for (unsigned i = 0; i < columnNames.size(); ++i)
		{
			if (!(std::getline(iss, cell, ',')))
			{
				logW << "Line " << path << ':' << lineNumber + 1 << " is malformed" << std::endl;
				throw std::runtime_error("Parsing error");
			}

			const auto name = columnNames[i] + '-' + cell;
			const auto code = encoding.at(name);
			row[i] = code;
		}

		loadNextRow();
		return row;
	}

	const std::unordered_map<std::string, Item>& getEncoding() const
	{
		return encoding.getDict();
	}

	bool isEOF() const
	{
		return eof;
	}

private:
	std::string path;
	std::ifstream stream;
	std::vector<std::string> columnNames;
	CounterDict<std::string, Item> encoding;
	int lineNumber = 0;
	std::string lineBuffer;
	bool eof = false;

	static bool isLineEmpty(const std::string& line)
	{
		if (line.size() == 0) return true;
		if (line[0] == '#') return true;
		return false;
	}

	void loadNextRow()
	{
		do
		{
			if (!std::getline(stream, lineBuffer))
			{
				eof = true;
				break;
			}
		} while (isLineEmpty(lineBuffer));
	}
};