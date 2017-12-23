#include "CppUnitTest.h"
#include "../eager-ova/Dataset.h"
#include "../eager-ova/CSVReader.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ruleminertests
{
	TEST_CLASS(DatasetTest)
	{
	public:

		TEST_METHOD(readSampleDataset)
		{
			CSVReader reader("sample.csv");
			const auto& columnNames = reader.getColumnNames();
			Dataset dataset{ columnNames };

			while (!reader.isEOF())
			{
				dataset.append(reader.nextRow());
			}

			Assert::AreEqual(2ull, dataset.getRequiredValues().size());
			Assert::AreEqual(4ull, dataset.getValueEncoding().size());
			const auto& encoding = dataset.getValueEncoding();
			
			const auto sunnyValue = encoding.at("outlook-sunny");
			const auto& sunnyTidset = dataset.getItems().at(sunnyValue);
			Assert::AreEqual(2ull, sunnyTidset.size());
			Assert::AreEqual(0u, static_cast<uint>(sunnyTidset[0]));
			Assert::AreEqual(1u, static_cast<uint>(sunnyTidset[1]));

			const auto hlowValue = encoding.at("humidity-low");
			const auto& hlowTidset = dataset.getItems().at(hlowValue);
			Assert::AreEqual(1ull, hlowTidset.size());
			Assert::AreEqual(1u, static_cast<uint>(hlowTidset[0]));

			const auto& columnByValue = dataset.getColumnsByValues();
			Assert::AreEqual(encoding.size(), columnByValue.size());
			
			auto sunnyColumnId = columnByValue[sunnyValue];
			const auto& sunnyColumnName = columnNames[sunnyColumnId];
			Assert::AreEqual({ "outlook" }, sunnyColumnName);

			auto hlowColumnId = columnByValue[hlowValue];
			const auto& hlowColumnName = columnNames[hlowColumnId];
			Assert::AreEqual({ "humidity" }, hlowColumnName);
		}

	};
}