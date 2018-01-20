#pragma once

#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include "Logger.h"

#define STREAM_ASSIGNMENT_FN(name) { #name, [&](std::istringstream& iss) { iss >> params.name; }}

struct Params
{
	double minSupport = 0.0;
	double growthThreshold = 4.0;
	bool cp = true;
	bool rawData = false;
	bool tabularOutput = false;
};

/**
* Reads parameters from file.
*/
inline Params readParams(const std::string& path)
{
	typedef void AssignFn(std::istringstream&);

	Params params;
	std::unordered_map<std::string, std::function<AssignFn>> rawParams = {
		STREAM_ASSIGNMENT_FN(minSupport),
		STREAM_ASSIGNMENT_FN(growthThreshold),
		STREAM_ASSIGNMENT_FN(cp),
		STREAM_ASSIGNMENT_FN(rawData),
		STREAM_ASSIGNMENT_FN(tabularOutput),
	};

	std::ifstream file(path);

	std::string line;
	int ln = 0;
	while (std::getline(file, line))
	{
		++ln;
		std::istringstream iss(line);
		std::string key;
		char assignment;
		if (!(iss >> key >> assignment) || assignment != '=')
		{
			logW << "Line " << path << ':' << ln << " is malformed" << std::endl;
			continue;
		}

		const auto fnIt = rawParams.find(key);
		if (fnIt != rawParams.end())
		{
			fnIt->second(iss);
		}
	}

	return params;
}
