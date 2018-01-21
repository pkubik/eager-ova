#include <iostream>

#include "CSVReader.h"
#include "CounterDict.h"

typedef unsigned short Id;

//static void show_usage(std::string name)
//{
//	std::cerr << "Usage: " << name << " <option(s)>"
//		<< "Options:\n"
//		<< "\t-h,--help\t\tShow this help message\n"
//		<< "\t-r,--run-algorithm ALGORITHM\tSpecify the algorithm to use (ALGORITHM can be {1,2,3,4,5}\n"
//		<< "\t-b,--benchmark \tRun benchmark mode - test all algorithms, show dominating set sizes and run times of algorithms"
//		<< std::endl;
//}

int main(const int argc, const char* argv[])
{


	//if (argc < 2 || argc > 3) {
	//	show_usage(argv[0]);
	//	return 1;
	//}

	//for (int i = 1; i < argc; ++i)
	//{
	//	std::string arg = argv[i];
	//	if ((arg == "-h") || (arg == "--help"))
	//	{
	//		show_usage(argv[0]);
	//		return 0;
	//	}
	//	else if ((arg == "-r") || (arg == "--run-algorithm"))
	//	{
	//		if (i + 1 < argc)
	//		{ // Make sure we aren't at the end of argv
	//			try
	//			{
	//				algorithm = std::stoi(argv[++i]);
	//			}
	//			catch (std::invalid_argument& ia)
	//			{
	//				std::cerr << "Invalid argument: " << ia.what() << '\n';
	//				return 1;
	//			}

	//			if (algorithm > DominatingSet::Algorithm::LAST)
	//			{
	//				std::cerr << "Wrong algorithm specified. Choose one of {1,2,3,4,5} \n";
	//				return 1;
	//			}

	//		}
	//		else { // There was no argument to the run-algorith option
	//			std::cerr << "--run-algorith option requires one argument." << std::endl;
	//			return 1;
	//		}
	//	}
	//	else if ((arg == "-b") || (arg == "--benchmark"))
	//	{
	//		benchmark_mode = true;
	//	}
	//	else
	//	{
	//		show_usage(argv[0]);
	//		return 1;
	//	}
	//}



	std::string path = "E:\\Programowanie\\eager-ova\\rule-based-classification\\Datasets\\car-test\\cars.csv";
	//std::string path = "E:\\Programowanie\\eager-ova\\rule-based-classification\\Datasets\\adult\\adult.csv";


	CSVReader reader(path);

	reader.randomSplitCSV(0.3);



	return 0;
}