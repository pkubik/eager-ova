#include <iostream>
#include <string>
#include <chrono>
#include "IOUtils.h"
#include "CSVReader.h"
#include "Dataset.h"
#include "Miner.h"
#include "RuleWriter.h"

using namespace std;

constexpr auto PARAMS_PATH_SUFFIX = "\\rule-miner-params.txt";
constexpr auto CSV_PATH_SUFFIX = "\\data.csv";
constexpr auto INDEX_PATH_SUFFIX = "\\index.txt";
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

	if (params.rawData && params.tabularOutput)
	{
		cout << "Unsupported parameters: 'rawData' and 'tabularOutput' cannot be both true at the same time";
		return -1;
	}

	cout << "Reading the data..." << endl;
	const auto csvPath = dataPath + CSV_PATH_SUFFIX;
	const auto indexPath = dataPath + INDEX_PATH_SUFFIX;
	
	auto dataset = (params.rawData) ?
		Dataset::fromFile(csvPath, true, params.columnUsage, indexPath) :
		Dataset::fromFile(csvPath, false, params.columnUsage, indexPath);
	cout << "Processed " << dataset.getNumberOfTransactions() << " transactions" << endl;

	cout << "Mining the rules..." << endl;
	const auto& requiredValues = dataset.getRequiredValues();
	const std::vector<Id> classIds{ requiredValues.begin(), requiredValues.end() };

	const auto miningStartTime = std::chrono::steady_clock::now();

	Miner miner{ classIds };
	miner.params.minRelSupport = params.minSupport;
	miner.params.cp = params.cp;
	miner.params.growthThreshold = params.growthThreshold;

	auto rules = miner.mine(dataset.getItems());

	const auto miningFinishTime = std::chrono::steady_clock::now();
	const auto miningDuration = miningFinishTime - miningStartTime;

	cout << "Discovered " << std::to_string(rules.size()) << " rules..." << endl;
	cout.precision(16);
	cout << "Mining took " <<
		std::chrono::duration_cast<std::chrono::duration<double>>(miningDuration).count() <<
		"s" << endl;

	cout << "Writing the rules..." << endl;
	const auto rulesPath = dataPath + RULES_PATH_SUFFIX;
	if (params.tabularOutput)
	{
		TableRuleWriter writer(rulesPath, dataset.getValueEncoding(), dataset.getColumnNames());
		for (const auto& rule : rules)
		{
			writer.writeRule(rule);
		}
	}
	else
	{
		RuleWriter writer(rulesPath, dataset.getValueEncoding());
		for (const auto& rule : rules)
		{
			writer.writeRule(rule);
		}
	}

	return 0;
}
