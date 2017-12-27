#include <iostream>
#include <string>
#include "IOUtils.h"
#include "CSVReader.h"
#include "Dataset.h"
#include "Miner.h"
#include "RuleWriter.h"

using namespace std;

constexpr auto PARAMS_PATH_SUFFIX = "\\rule-miner-params.txt";
constexpr auto CSV_PATH_SUFFIX = "\\data.csv";
constexpr auto RULES_PATH_SUFFIX = "\\rules.txt";

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
	auto dataset = Dataset::fromFile(csvPath);

	cout << "Mining the rules..." << endl;
	const auto& requiredValues = dataset.getRequiredValues();
	const std::vector<Id> classIds{ requiredValues.begin(), requiredValues.end() };
	Miner miner{ classIds };
	if (params.count("minSupport") > 0)
		miner.params.minRelSupport = params.at("minSupport");
	if (params.count("cp") > 0)
		miner.params.cp = params.at("cp");
	if (params.count("growthThreshold") > 0)
		miner.params.growthThreshold = params.at("growthThreshold");

	auto rules = miner.mine(dataset.getItems());
	cout << "Discovered " << std::to_string(rules.size()) << " rules..." << endl;

	cout << "Writing the rules..." << endl;
	const auto rulesPath = dataPath + RULES_PATH_SUFFIX;
	RuleWriter writer(rulesPath, dataset.getValueEncoding());
	for (const auto& rule : rules)
	{
		writer.writeRule(rule);
	}

	return 0;
}
