#include <iostream>

#include <map>
#include <iostream>
#include <vector>
#include <set>
#include <cmath>   //because of the ceil function
#include <algorithm>
#include <iterator>
#include <fstream>
#include <experimental/filesystem>

#include "../train-test-split/CSVReader.h"
#include "TestSet.h"
#include "Classifier.h"
#include "ConfusionMatrix.h"
#include "IOUtils.h"


static void show_usage(std::string name)
{
	std::cerr << "Usage: " << name << " <option(s)>"
		<< "Options:\n"
		<< "\t-h,--help\t\tShow this help message\n"
		<< "\t-p,--path \tPath to dir with requierd files"
		<< std::endl;
}

int main(const int argc, const char* argv[])
{
	if (argc < 2 || argc > 4) {
		show_usage(argv[0]);
		return 1;
	}

	std::string path;

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if ((arg == "-h") || (arg == "--help"))
		{
			show_usage(argv[0]);
			return 0;
		}
		else if ((arg == "-p") || (arg == "--path"))
		{
			if (i + 1 < argc)
			{ // Make sure we aren't at the end of argv
				try
				{
					path = argv[++i];
				}
				catch (std::invalid_argument& ia)
				{
					std::cerr << "Invalid argument: " << ia.what() << '\n';
					return 1;
				}
			}
			else { // There was no argument to the run-algorith option
				std::cerr << "--path option requires one argument." << std::endl;
				return 1;
			}
		}
		else
		{
			show_usage(argv[0]);
			return 1;
		}
	}



	//std::string data_path = "E:\\Programowanie\\eager-ova\\rule-based-classification\\Datasets\\car-test";
	//std::string data_path = "E:\\Programowanie\\eager-ova\\rule-based-classification\\Datasets\\adult_test";
	//std::string params_path = "E:\\Programowanie\\eager-ova\\rule-based-classification\\Datasets\\adult_test\\classifier-params.txt";

	std::string data_path = path;
	auto params_path = fs::path(data_path).append("classifier-params.txt").string();

	const auto params = readParams(params_path);

	TestSet test(data_path);

	auto&& test_set = test.Get();
	auto&& class_ids = test.ClassIDs();

	RuleBasedClassifier classifier;
	classifier.loadRules(data_path);

	auto& labels_map = classifier.getLabelsMap();

	std::vector<int> results = classifier.classify(test_set, params);

	Confusion confusion_matrix(class_ids, results, labels_map);
	confusion_matrix.print();

	
	std::ofstream outfile;
	outfile.open(data_path + "\\classification_results_full.txt", fstream::app);

	for (int i = 0; i < class_ids.size(); i++)
	{
		outfile << class_ids[i] << " " << results[i] << std::endl;
	}

	return 0;
}