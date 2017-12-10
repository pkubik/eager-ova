#include "stdafx.h"
#include "CppUnitTest.h"
#include "../eager-ova/CSVReader.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ruleminertests
{		
	TEST_CLASS(CSVReaderTest)
	{
	public:
		
		TEST_METHOD(readSampleCSV)
		{
			CSVReader reader("sample.csv");
			Assert::IsFalse(reader.isEOF());
			const auto& encoding = reader.getEncoding();

			auto row1 = reader.nextRow();
			Assert::AreEqual(row1[0], encoding.at("outlook-sunny"));
			Assert::AreEqual(row1[1], encoding.at("temperature-hot"));
			Assert::AreEqual(row1[2], encoding.at("humidity-high"));

			auto row2 = reader.nextRow();
			Assert::AreEqual(row2[0], encoding.at("outlook-sunny"));
			Assert::AreEqual(row2[1], encoding.at("temperature-hot"));
			Assert::AreEqual(row2[2], encoding.at("humidity-low"));

			Assert::AreEqual(encoding.size(), 4ull);
		}

	};
}