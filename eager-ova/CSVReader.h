#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <stdexcept>
#include "IOUtils.h"
#include "RowIndex.h"


/*
 * CSV header is required
 */
class CSVReader
{
public:
	typedef std::vector<std::string> Row;

	explicit CSVReader(const std::string& path)
		: path(path), stream(path)
	{
		init();
	}

	explicit CSVReader(const std::string& path, RowIndex rowIndex)
		: path(path), stream(path), rowIndex(std::move(rowIndex)), rowIndexIterator(this->rowIndex.begin())
	{
		init();
	}

	Row nextRow()
	{
		if (isEOF())
		{
			logW << "No more rows to fetch for " << path << "." << std::endl;
			throw std::logic_error("No more rows to fetch.");
		}

		if (!rowIndex.empty())
		{
			while (lineNumber < *rowIndexIterator)
			{
				++lineNumber;
				loadNextRow();
			}
			++rowIndexIterator;
		}
		++lineNumber;

		std::string cell;
		Row row(columnNames.size());

		std::istringstream iss(lineBuffer);
		for (unsigned i = 0; i < columnNames.size(); ++i)
		{
			if (!(std::getline(iss, cell, ',')))
			{
				logW << "Line " << path << ':' << lineNumber + 1 << " is malformed" << std::endl;
				throw std::runtime_error("Parsing error");
			}

			row[i] = cell;
		}

		loadNextRow();
		return row;
	}

	const std::vector<std::string>& getColumnNames() const
	{
		return columnNames;
	}

	bool isEOF() const
	{
		return eof;
	}

	unsigned int getNumberOfReadLines() const
	{
		return lineNumber;
	}

private:
	std::string path;
	std::ifstream stream;
	RowIndex rowIndex;
	RowIndex::const_iterator rowIndexIterator;
	std::vector<std::string> columnNames;
	unsigned int lineNumber = 0;
	std::string lineBuffer;
	bool eof = false;

	static bool isLineEmpty(const std::string& line)
	{
		if (line.empty()) return true;
		if (line[0] == '#') return true;
		return false;
	}

	void init()
	{
		if (stream.fail())
		{
			throw std::runtime_error("CSV file does not exist.");
		}

		std::string cell;

		std::getline(stream, lineBuffer);
		std::istringstream iss(lineBuffer);
		while (std::getline(iss, cell, ','))
		{
			columnNames.push_back(cell);
		}

		loadNextRow();
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