#pragma once

#include <experimental/filesystem>
#include <fstream>

#include "../train-test-split/CSVReader.h"

namespace fs = std::experimental::filesystem;

typedef std::vector<int> RulePre;

class TestSet
{
public:
	TestSet(std::string data_dir)
	{
		auto test_set_path = fs::path(data_dir).append("data_test.csv").string();
		auto labels_path = fs::path(data_dir).append("labels.txt").string();

		loadLabelsMap(labels_path);
		CSVReader reader(test_set_path);

		auto column_names = reader.getColumnNames();
		auto class_column = column_names.size() - 1;
		
		while (!reader.isEOF())
		{
			auto row = reader.nextRow();

			RulePre test_case(column_names.size() - 1);

			for (int i = 0; i < column_names.size(); i++)
			{
				if (i == class_column)
				{
					classes.push_back(class_ids[row[i]]);
				}
				else
				{
					test_case[i] = std::stoi(row[i]);
				}
			}

			std::sort(test_case.begin(), test_case.end());
			test_set.push_back(test_case);
		}
	}

	std::vector<RulePre> Get()
	{
		return test_set;
	}

	std::vector<int> ClassIDs()
	{
		return classes;
	}


private:
	std::vector<RulePre> test_set;
	std::vector<int> classes;
	std::unordered_map<std::string, unsigned short int> class_ids;
	std::unordered_map<unsigned short int, std::string> labels;

	void loadLabelsMap(std::string labels_path)
	{
		std::ifstream labels_file(labels_path);

		std::string label;
		unsigned short int class_id;
		while (labels_file >> label >> class_id)
		{
			class_ids.insert(std::make_pair(label, class_id));
			labels.insert(std::make_pair(class_id, label));
		}
	}

};