#pragma once

#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <iostream>

#define STREAM_ASSIGNMENT_FN(name) { #name, [&](std::istringstream& iss) { iss >> params.name; }}

struct Params
{
	std::string METHOD = "K_MAX";
	unsigned int K_MAX = 50;
	unsigned int K_CPAR = 50;
	unsigned int K_PCL = 15;
};

/**
* Reads parameters from file.
*/
inline Params readParams(const std::string& path)
{
	typedef void AssignFn(std::istringstream&);

	Params params;
	std::unordered_map<std::string, std::function<AssignFn>> rawParams = {
		STREAM_ASSIGNMENT_FN(METHOD),
		STREAM_ASSIGNMENT_FN(K_MAX),
		STREAM_ASSIGNMENT_FN(K_CPAR),
		STREAM_ASSIGNMENT_FN(K_PCL)
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
			std::cerr << "Line " << path << ':' << ln << " is malformed" << std::endl;
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