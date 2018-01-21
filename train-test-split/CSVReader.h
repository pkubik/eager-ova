#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <random>
#include <chrono>
#include <map>

#include "CounterDict.h"

namespace fs = std::experimental::filesystem;




static inline std::string &ltrim(std::string &s)
{
	s.erase(s.begin(), find_if_not(s.begin(), s.end(), [](int c) {return isspace(c); }));
	return s;
}

static inline std::string &rtrim(std::string &s)
{
	s.erase(find_if_not(s.rbegin(), s.rend(), [](int c) {return isspace(c); }).base(), s.end());
	return s;
}

static inline std::string trim(const std::string &s)
{
	std::string t = s;
	return ltrim(rtrim(t));
}

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

	Row nextRow()
	{
		if (isEOF())
		{
			//logW << "No more rows to fetch for " << path << "." << std::endl;
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
				//logW << "Line " << path << ':' << lineNumber + 1 << " is malformed" << std::endl;
				throw std::runtime_error("Parsing error");
			}

			row[i] = trim(cell);
			
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


	void randomSplitCSV(float ratio)
	{
		auto fs_path = fs::path(path);

		std::string train_data_path = fs_path.parent_path().append("data.csv").string();
		std::string test_data_path = fs_path.parent_path().append("data_test.csv").string();

		std::string params_map_path = fs_path.parent_path().append("params_map.txt").string();
		std::string labels_path = fs_path.parent_path().append("labels.txt").string();

		std::ofstream train_file, test_file, params_file, labels_file;

		train_file.open(train_data_path);
		test_file.open(test_data_path);


		int classColumn = columnNames.size() - 1;

		typedef unsigned short Id;

		CounterDict<std::string, Id> ParamsDict;
		CounterDict<std::string, Id> ClassesDict;


		bool first = true;
		for (auto&& col : columnNames)
		{
			if (!first)
			{
				train_file << ",";
				test_file << ",";
			}
			train_file << col;
			test_file << col;
			first = false;
		}

		train_file << std::endl;
		test_file << std::endl;

		auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::mt19937 rng(seed);
		std::uniform_real_distribution<double> u_rand(0, 1);

		std::map<int, int> per_class_counter;

		typedef std::vector<std::vector<std::string>> rows_list;

		rows_list rows;

		std::unordered_map<int, std::vector<int>> per_class_indices;

		while (!isEOF())
		{
			auto row = nextRow();

			Id classId;

			for (int i = 0; i < columnNames.size(); ++i)
			{
				Id valueId;

				if (i == classColumn)
				{
					const auto& value = columnNames[i] + '_' + row[i];
					classId = ClassesDict.at(value);
					per_class_counter[classId]++;
				}
				else
				{
					const auto& value = columnNames[i] + '_' + row[i];
					valueId = ParamsDict.at(value);
				}
			}

			//rows[classId].push_back(row);

			per_class_indices[classId].push_back(rows.size());
			rows.push_back(row);

		}

		int class_column = columnNames.size() - 1;

		int type = 0;

		switch (type)
		{
		case 0: // random split
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
		}
			

		while (!isEOF())
		{
			auto row = nextRow();

			double r = u_rand(rng);

			for (int i = 0; i < columnNames.size(); ++i)
			{
				Id valueId;

				if (i != 0)
				{
					(r < ratio) ? test_file << "," : train_file << ",";
				}

				if (i == classColumn)
				{
					const auto& value = columnNames[i] + '_' + row[i];
					valueId = ClassesDict.at(value);
					(r < ratio) ? test_file << value : train_file << value;
				}
				else
				{
					const auto& value = columnNames[i] + '_' + row[i];
					valueId = ParamsDict.at(value);
					(r < ratio) ? test_file << valueId : train_file << valueId;
				}
			}

			(r < ratio) ? test_file << std::endl : train_file << std::endl;

		}

		train_file.close();
		test_file.close();

		auto params_map = ParamsDict.getDict();
		auto labels_map = ClassesDict.getDict();

		params_file.open(params_map_path);
		for (auto&& p : params_map)
		{
			params_file << p.first << " " << p.second << std::endl;
		}
		params_file.close();

		labels_file.open(labels_path);
		for (auto&& p : labels_map)
		{
			labels_file << p.first << " " << p.second << std::endl;
		}
		labels_file.close();
	}

private:
	std::string path;
	std::ifstream stream;
	std::vector<std::string> columnNames;
	int lineNumber = 0;
	std::string lineBuffer;
	bool eof = false;

	static bool isLineEmpty(const std::string& line)
	{
		if (line.empty()) return true;
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