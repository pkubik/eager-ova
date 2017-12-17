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

			auto row1 = reader.nextRow();
			Assert::AreEqual(row1[0], std::string{ "sunny" });
			Assert::AreEqual(row1[1], std::string{ "hot" });
			Assert::AreEqual(row1[2], std::string{ "high" });

			auto row2 = reader.nextRow();
			Assert::AreEqual(row2[0], std::string{ "sunny" });
			Assert::AreEqual(row2[1], std::string{ "hot" });
			Assert::AreEqual(row2[2], std::string{ "low" });
		}

	};
}