#include "CppUnitTest.h"
#include "../eager-ova/RuleWriter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ruleminertests
{
	TEST_CLASS(RuleWriterTest)
	{
	public:

		TEST_METHOD(decodeRule)
		{
			const std::vector<std::string> reversedEncoding = { "A", "B", "C" };
			Rule rule;
			rule.lhs = { 0u, 2u };
			rule.rhs = 1u;
			rule.support = 10u;
			rule.confidence = 0.5;

			RuleDecoder decoder{ reversedEncoding };
			const auto decodedRule = decoder.decode(rule);

			Assert::AreEqual(rule.support, decodedRule.support);
			Assert::AreEqual(rule.confidence, decodedRule.confidence);
			Assert::AreEqual(std::string("B"), decodedRule.rhs);

			const std::vector<std::string> referenceLhs = { "A", "C" };
			Assert::IsTrue(vectorIntersection(decodedRule.lhs, referenceLhs).size() == vectorUnion(decodedRule.lhs, referenceLhs).size());
		}
	};
}