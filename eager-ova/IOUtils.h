#pragma once

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include "Logger.h"

/**
* Reads parameters from file.
*
* Only <string, double> pairs are accepted as parameters, e.g. `minSupport = 0.8`.
*/
inline std::map<std::string, double> readParams(const std::string& path)
{
	std::ifstream file(path);

	std::map<std::string, double> params;
	std::string line;
	int ln = 0;
	while (std::getline(file, line))
	{
		++ln;
		std::istringstream iss(line);
		std::string key;
		char assignment;
		double value;
		if (!(iss >> key >> assignment >> value) || assignment != '=')
		{
			logW << "Line " << path << ':' << ln << " is malformed" << std::endl;
			continue;
		}

		params[key] = value;
	}

	return params;
}