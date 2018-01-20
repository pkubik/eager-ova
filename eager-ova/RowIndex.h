#pragma once

#include <vector>
#include <fstream>
#include "Logger.h"

typedef std::vector<unsigned int> RowIndex;

inline RowIndex loadRowIndex(const std::string& path)
{
	RowIndex rowIndex;
	std::ifstream stream(path);

	if (stream.fail())
	{
		logW << "Index file " << path << " not found. Using whole dataset." << std::endl;
		return rowIndex;
	}

	unsigned int value;
	while (stream >> value)
	{
		rowIndex.push_back(value);
	}

	return rowIndex;
}
