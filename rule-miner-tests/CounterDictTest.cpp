#include "CppUnitTest.h"
#include "../eager-ova/CounterDict.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ruleminertests
{
	TEST_CLASS(CounterDictTest)
	{
	public:

		TEST_METHOD(simpleCounterDict)
		{
			CounterDict<std::string, int> dict;

			Assert::AreEqual(dict.at("A"), 0);
			Assert::AreEqual(dict.at("B"), 1);
			Assert::AreEqual(dict.at("C"), 2);

			Assert::AreEqual(dict.at("A"), 0);
			Assert::AreEqual(dict.at("C"), 2);
			Assert::AreEqual(dict.at("D"), 3);

			Assert::AreEqual(dict.getDict().size(), 4ull);
		}

	};
}