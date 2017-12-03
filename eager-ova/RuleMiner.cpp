#include <iostream>
#include <string>
#include "IOUtils.h"

using namespace std;

constexpr auto PARAMS_PATH_SUFFIX = "\\rule-miner-params.txt";

void printHelp() {
	cout << "Specify data directory!" << endl;
	cout << "<exe-path> data_path" << endl;
	cout << "Data directory should contain `rule-miner-params.txt` and `data.csv`" << endl;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printHelp();
		return -1;
	}

	const std::string dataPath(argv[1]);

	cout << "Starting the rule miner..." << endl;
	cout << "Reading parameters..." << endl;
	const auto paramsPath = dataPath + PARAMS_PATH_SUFFIX;
	const auto params = readParams(paramsPath);

	cout << "Reading the data..." << endl;
	cerr << "Not implemented!" << endl;

	return 0;
}