#include <iostream>
#include <string>
#include "IOUtils.h"
#include "CSVReader.h"

using namespace std;

constexpr auto PARAMS_PATH_SUFFIX = "\\rule-miner-params.txt";
constexpr auto CSV_PATH_SUFFIX = "\\data.csv";

void printHelp()
{
	cout << "Specify data directory!" << endl;
	cout << "<exe-path> data_path" << endl;
	cout << "Data directory should contain `rule-miner-params.txt` and `data.csv`" << endl;
}

int main(const int argc, const char* argv[])
{
	if (argc < 2)
	{
		printHelp();
		return -1;
	}

	const std::string dataPath(argv[1]);

	cout << "Starting the rule miner..." << endl;
	
	cout << "Reading parameters..." << endl;
	const auto paramsPath = dataPath + PARAMS_PATH_SUFFIX;
	const auto params = readParams(paramsPath);

	cout << "Reading the data..." << endl;
	const auto csvPath = dataPath + CSV_PATH_SUFFIX;
	auto csvReader = CSVReader(csvPath);
	while (!csvReader.isEOF())
	{
		auto row = csvReader.nextRow();
		for (const auto& cell : row)
		{
			cout << cell << ", ";
		}
		cout << endl;
	}

	cout << endl;
	
	for (const auto& pair : csvReader.getEncoding())
	{
		cout << pair.first << ", " << pair.second << endl;
	}

	cerr << "Not implemented!" << endl;

	return 0;
}
