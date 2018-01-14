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
			rule.lhsSupport = 20u;
			rule.rhsSupport = 30u;
			rule.support = 10u;
			rule.confidence = 0.5;
			rule.growth = 3.0;

			RuleDecoder decoder{ reversedEncoding };
			const auto decodedRule = decoder.decode(rule);

			Assert::AreEqual(rule.support, decodedRule.originalRule.support);
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
			rule.lhsSupport = 20u;
			rule.rhsSupport = 30u;
			rule.support = 10u;
			rule.confidence = 0.5;
			rule.growth = 3.0;

			RuleDecoder decoder{ reversedEncoding };
			const auto ruleString = decoder.decodeToString(rule);

			const std::string refRuleString =
				std::string("A, C; B; 20, 30, 10, " +
					std::to_string(rule.confidence)) + ", " +
					std::to_string(rule.growth);
			Assert::AreEqual(refRuleString, ruleString);
		}

		TEST_METHOD(decomposeValue)
		{
			std::unordered_map<std::string, Id> encoding;
			encoding["C1-A"] = 0;
			encoding["C1-B"] = 1;
			encoding["C2-A"] = 2;

			TableRuleDecoder tableDecoder(encoding, {"C1", "C2"});
			
			const auto c1a = tableDecoder.decomposeValue("C1-A");
			Assert::AreEqual(c1a.first, 0ull);
			Assert::AreEqual(c1a.second, std::string("A"));

			const auto c1b = tableDecoder.decomposeValue("C1-B");
			Assert::AreEqual(c1b.first, 0ull);
			Assert::AreEqual(c1b.second, std::string("B"));

			const auto c2a = tableDecoder.decomposeValue("C2-A");
			Assert::AreEqual(c2a.first, 1ull);
			Assert::AreEqual(c2a.second, std::string("A"));
		}

		TEST_METHOD(decodeRuleToTableString)
		{
			std::unordered_map<std::string, Id> encoding;
			encoding["C1-A"] = 0;
			encoding["C1-B"] = 1;
			encoding["C2-Z"] = 2;
			encoding["C3-X"] = 3;
			TableRuleDecoder tableDecoder(encoding, { "C1", "C2", "C3" });

			Rule rule;
			rule.lhs = { 0u, 2u };
			rule.rhs = 3u;
			rule.lhsSupport = 20u;
			rule.rhsSupport = 30u;
			rule.support = 10u;
			rule.confidence = 0.5;
			rule.growth = 3.0;

			const auto ruleString = tableDecoder.decodeToRow(rule);

			const std::string refRuleString =
				std::string("A, Z, X, 20, 30, 10, " +
					std::to_string(rule.confidence)) + ", " +
				std::to_string(rule.growth);
			Assert::AreEqual(refRuleString, ruleString);

			const std::string refHeader =
				"C1, C2, C3, lhsSupport, rhsSupport, support, confidence, growth";
			Assert::AreEqual(refHeader, tableDecoder.headerString());
		}
	};
}