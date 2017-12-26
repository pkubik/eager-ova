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

		TEST_METHOD(reverseEncodingTest)
		{
			std::unordered_map<std::string, Id> encoding;
			encoding["A"] = 0;
			encoding["B"] = 1;
			encoding["C"] = 2;

			auto reversedEncoding = reverseEncoding(encoding);
			Assert::AreEqual(std::string("A"), reversedEncoding[0]);
			Assert::AreEqual(std::string("B"), reversedEncoding[1]);
			Assert::AreEqual(std::string("C"), reversedEncoding[2]);
		}

		TEST_METHOD(decodeRuleToString)
		{
			const std::vector<std::string> reversedEncoding = { "A", "B", "C" };
			Rule rule;
			rule.lhs = { 0u, 2u };
			rule.rhs = 1u;
			rule.support = 10u;
			rule.confidence = 0.5;

			RuleDecoder decoder{ reversedEncoding };
			const auto ruleString = decoder.decodeToString(rule);

			Assert::AreEqual(std::string("A, C -> B; 10, " + std::to_string(rule.confidence)), ruleString);
		}
	};
}