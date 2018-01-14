#include <iostream>

#include "CSVReader.h"
#include "CounterDict.h"

typedef unsigned short Id;

int main(const int argc, const char* argv[])
{

	std::string path = "E:\\Programowanie\\eager-ova\\rule-based-classification\\Datasets\\car-test\\cars.csv";


	CSVReader reader(path);

	reader.randomSplitCSV(0.3);



	return 0;
}