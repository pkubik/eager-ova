#include <iostream>

#include "CSVReader.h"
#include "CounterDict.h"


typedef unsigned short Id;

static void show_usage(std::string name)
{
	std::cerr << "Usage: " << name << " <option(s)>"
		<< "Options:\n"
		<< "\t-h,--help\t\tShow this help message\n"
		<< "\t-p,--path \tPath to dir with 'data_all.csv' file to process\n"
		<< "\t-m,--method METHOD\tSpecify the validation method to use (METHOD can be {holdout, subsampling, cross}\n"
		<< "\t-r,--ratio \tRatio of test-data (0-1)\n"
		<< "\t-k \tk param in random subsumpling or k-cross validation"
		<< std::endl;
}

int main(const int argc, const char* argv[])
{


	if (argc < 2 || argc > 9) {
		show_usage(argv[0]);
		return 1;
	}

	std::string path;
	std::string method = "cross";
	int k = 3;
	float ratio = 0.3;

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
					//algorithm = std::stoi(argv[++i]);
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
		else if ((arg == "-m") || (arg == "--method"))
		{
			if (i + 1 < argc)
			{ // Make sure we aren't at the end of argv
				try
				{
					//algorithm = std::stoi(argv[++i]);
					method = argv[++i];
				}
				catch (std::invalid_argument& ia)
				{
					std::cerr << "Invalid argument: " << ia.what() << '\n';
					return 1;
				}

			}
			else { // There was no argument to the run-algorith option
				std::cerr << "--method option requires one argument." << std::endl;
				return 1;
			}
		}
		else if ((arg == "-r") || (arg == "--ratio"))
		{
			if (i + 1 < argc)
			{ // Make sure we aren't at the end of argv
				try
				{
					//algorithm = std::stoi(argv[++i]);
					ratio = std::stof(argv[++i]);
					if (ratio < 0 || ratio > 1)
					{
						std::cerr << "Invalid argument - ratio should be in (0,1>" <<'\n';
						return 1;
					}
				}
				catch (std::invalid_argument& ia)
				{
					std::cerr << "Invalid argument: " << ia.what() << '\n';
					return 1;
				}

			}
			else { // There was no argument to the run-algorith option
				std::cerr << "--ratio option requires one argument." << std::endl;
				return 1;
			}
		}
		else if (arg == "-k")
		{
			if (i + 1 < argc)
			{ // Make sure we aren't at the end of argv
				try
				{
					//algorithm = std::stoi(argv[++i]);
					k = std::stoi(argv[++i]);
				}
				catch (std::invalid_argument& ia)
				{
					std::cerr << "Invalid argument: " << ia.what() << '\n';
					return 1;
				}

			}
			else { // There was no argument to the run-algorith option
				std::cerr << "-k option requires one argument." << std::endl;
				return 1;
			}
		}
		else
		{
			show_usage(argv[0]);
			return 1;
		}
	}


	//std::string path = "E:\\Programowanie\\eager-ova\\rule-based-classification\\Datasets\\car-test\\cars.csv";
	//std::string path2 = "E:\\Programowanie\\eager-ova\\rule-based-classification\\Datasets\\adult\\adult.csv";


	std::string csv_path = path + "\\data_full.csv";

	CSVReader reader(csv_path);

	if (method == "holdout")
	{
		reader.SplitCSV(SplitType::HOLDOUT, ratio, k);
	}
	else if (method == "subsampling")
	{
		reader.SplitCSV(SplitType::SUBSAMPLING, ratio, k);
	}
	else if (method == "cross")
	{
		reader.SplitCSV(SplitType::CROSS_VALIDATION, ratio, k);
	}
	else
	{
		std::cerr << "Wrong method!" << "\n";
	}

	return 0;
}