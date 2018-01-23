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

#include "../eager-ova/RowIndex.h"

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


enum SplitType
{
	HOLDOUT = 0,
	SUBSAMPLING,
	CROSS_VALIDATION
};


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
			throw std::logic_error("No more rows to fetch.");
		}

		if (!rowIndex.empty())
		{
			if (rowIndexIterator == rowIndex.end())
			{
				throw std::logic_error("No more rows to fetch.");
			}
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
				throw std::runtime_error("Parsing error");
			}

			row[i] = row[i] = trim(cell);;
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
		return eof || (!rowIndex.empty() && rowIndexIterator == rowIndex.end());
	}

	unsigned int getNumberOfReadLines() const
	{
		return lineNumber;
	}


	void SplitCSV(SplitType type, float ratio=0.2, int k=3)
	{
		auto fs_path = fs::path(path);

		std::string train_data_path = fs_path.parent_path().append("data.csv").string();
		std::string test_data_path = fs_path.parent_path().append("data_test.csv").string();
		std::string index_train_path = fs_path.parent_path().append("index.txt").string();
		std::string index_test_path = fs_path.parent_path().append("index_test.txt").string();
		std::string trainig_set_size_path = fs_path.parent_path().append("train_size.txt").string();

		std::string params_map_path = fs_path.parent_path().append("params_map.txt").string();
		std::string labels_path = fs_path.parent_path().append("labels.txt").string();

		std::ofstream train_file, test_file, params_file, labels_file, index_train_file, index_test_file;
		std::ofstream trainig_set_size_file;

		train_file.open(train_data_path);
		//test_file.open(test_data_path);
		index_train_file.open(index_train_path);
		index_test_file.open(index_test_path);
		


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
				//test_file << ",";
			}
			train_file << col;
			test_file << col;
			first = false;
		}

		train_file << std::endl;
		//test_file << std::endl;

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
					if (row[i] != "NA" && row[i] != "")
					{
						const auto& value = columnNames[i] + '_' + row[i];
						valueId = ParamsDict.at(value);
					}
					
				}
			}

			//rows[classId].push_back(row);

			per_class_indices[classId].push_back(rows.size());
			rows.push_back(row);

		}

		//int class_column = columnNames.size() - 1;

		for (int j = 0; j < rows.size(); j++)
		{
			auto row = rows[j];
			for (int i = 0; i < columnNames.size(); ++i)
			{
				Id valueId;

				if (i != 0)
				{
					train_file << ",";
				}

				if (i == classColumn)
				{
					const auto& value = columnNames[i] + '_' + row[i];
					train_file << value;
				}
				else
				{
					if (row[i] != "NA" && row[i] != "")
					{
						const auto& value = columnNames[i] + '_' + row[i];
						valueId = ParamsDict.at(value);
						train_file << valueId;
					}
					else
					{
						train_file << "NaN";
					}
					
				}
			}
			train_file << std::endl;
		}


		switch (type)
		{
		case SplitType::HOLDOUT: // random split
		{
			std::vector<int> training;
			std::vector<int> testing;

			for (int cl = 0; cl < per_class_indices.size(); cl++)
			{
				auto class_indices = per_class_indices[cl];

				std::shuffle(class_indices.begin(), class_indices.end(), rng);
				int border = ratio * class_indices.size();

				for (int j = 0; j < class_indices.size(); j++)
				{
					(j < border) ? testing.push_back(class_indices[j]) : training.push_back(class_indices[j]);
					//(j < border) ? index_test_file << class_indices[j] << std::endl : index_train_file << class_indices[j] << std::endl;
				}
			}

			std::sort(training.begin(), training.end());
			std::sort(testing.begin(), testing.end());

			for (auto i : training)
			{
				index_train_file << i << std::endl;
			}

			for (auto i : testing)
			{
				index_test_file << i << std::endl;
			}

			trainig_set_size_file.open(trainig_set_size_path);
			trainig_set_size_file << training.size() << std::endl;
			trainig_set_size_file.close();

			break;
		}
		case SplitType::SUBSAMPLING:
		{
			for (int i = 0; i < k; i++)
			{
				std::string index_train_path = fs_path.parent_path().append(std::string("index_") + std::to_string(i) + ".txt").string();
				std::string index_test_path = fs_path.parent_path().append(std::string("index_test_") + std::to_string(i) + ".txt").string();

				std::vector<int> training;
				std::vector<int> testing;

				std::ofstream index_train_file, index_test_file;

				index_train_file.open(index_train_path);
				index_test_file.open(index_test_path);

				for (int cl = 0; cl < per_class_indices.size(); cl++)
				{
					auto class_indices = per_class_indices[cl];

					std::shuffle(class_indices.begin(), class_indices.end(), rng);
					int border = ratio * class_indices.size();

					for (int j = 0; j < class_indices.size(); j++)
					{
						(j < border) ? testing.push_back(class_indices[j]) : training.push_back(class_indices[j]);
						//(j < border) ? index_test_file << class_indices[j] << std::endl : index_train_file << class_indices[j] << std::endl;
					}
				}

				std::sort(training.begin(), training.end());
				std::sort(testing.begin(), testing.end());

				for (auto i : training)
				{
					index_train_file << i << std::endl;
				}

				for (auto i : testing)
				{
					index_test_file << i << std::endl;
				}

				trainig_set_size_file.open(trainig_set_size_path);
				trainig_set_size_file << training.size() << std::endl;
				trainig_set_size_file.close();

				index_train_file.close();
				index_test_file.close();
			}
			break;
		}
		case SplitType::CROSS_VALIDATION: // cros validation
		{
			for (int i = 0; i < k; i++)
			{
				std::string index_train_path = fs_path.parent_path().append(std::string("index_") + std::to_string(i) + ".txt").string();
				std::string index_test_path = fs_path.parent_path().append(std::string("index_test_") + std::to_string(i) + ".txt").string();

				std::vector<int> training;
				std::vector<int> testing;

				std::ofstream index_train_file, index_test_file;

				index_train_file.open(index_train_path);
				index_test_file.open(index_test_path);

				for (int cl = 0; cl < per_class_indices.size(); cl++)
				{
					auto class_indices = per_class_indices[cl];

					//std::shuffle(class_indices.begin(), class_indices.end(), rng);
					int low_border = i * float(class_indices.size())/ k;
					int high_border = (i + 1) * float(class_indices.size() / k);

					for (int j = 0; j < class_indices.size(); j++)
					{
						(j >= low_border && j < high_border) ? testing.push_back(class_indices[j]) : training.push_back(class_indices[j]);
						//(j < border) ? index_test_file << class_indices[j] << std::endl : index_train_file << class_indices[j] << std::endl;
					}
				}

				std::sort(training.begin(), training.end());
				std::sort(testing.begin(), testing.end());

				for (auto i : training)
				{
					index_train_file << i << std::endl;
				}

				for (auto i : testing)
				{
					index_test_file << i << std::endl;
				}

				trainig_set_size_file.open(trainig_set_size_path);
				trainig_set_size_file << training.size() << std::endl;
				trainig_set_size_file.close();

				index_train_file.close();
				index_test_file.close();
			}
			break;
		}
		default:
			break;
		}


		//while (!isEOF())
		//{
		//	auto row = nextRow();

		//	double r = u_rand(rng);

		//	for (int i = 0; i < columnNames.size(); ++i)
		//	{
		//		Id valueId;

		//		if (i != 0)
		//		{
		//			(r < ratio) ? test_file << "," : train_file << ",";
		//		}

		//		if (i == classColumn)
		//		{
		//			const auto& value = columnNames[i] + '_' + row[i];
		//			valueId = ClassesDict.at(value);
		//			(r < ratio) ? test_file << value : train_file << value;
		//		}
		//		else
		//		{
		//			const auto& value = columnNames[i] + '_' + row[i];
		//			valueId = ParamsDict.at(value);
		//			(r < ratio) ? test_file << valueId : train_file << valueId;
		//		}
		//	}

		//	(r < ratio) ? test_file << std::endl : train_file << std::endl;

		//}

		train_file.close();
		//test_file.close();
		index_train_file.close();
		index_test_file.close();

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
