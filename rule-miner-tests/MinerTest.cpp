#include "stdafx.h"
#include "CppUnitTest.h"
#include "../eager-ova/Miner.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ruleminertests
{
	TEST_CLASS(CSVReaderTest)
	{
	public:

		TEST_METHOD(simpleNode)
		{
			Miner::Node node;
			Assert::IsTrue(node.isRoot());
			Assert::IsTrue(node.children.empty());

			node.tidset = { 1, 2, 4, 6 };
			const std::vector<std::vector<Id>> classTidsets
			{
				{1, 2, 3, 5, 6},
				{2, 5, 6}
			};

			Assert::AreEqual(4u, node.getSupport());
			auto classSupports1 = node.getClassSupports(classTidsets);
			Assert::AreEqual(3u, classSupports1[0]);
			Assert::AreEqual(2u, classSupports1[1]);

			node.simplify(classTidsets);
			Assert::IsTrue(node.isSimplified());
			Assert::IsTrue(node.tidset.empty());

			Assert::AreEqual(4u, node.getSupport());
			auto classSupports2 = node.getClassSupports(classTidsets);
			Assert::AreEqual(3u, classSupports2[0]);
			Assert::AreEqual(2u, classSupports2[1]);
		}

		TEST_METHOD(subsetIteration)
		{
			Miner::Node node;
			
			node.children.emplace_back();
			node.children.back().ids = { 1 };
			node.children.back().children.emplace_back();
			node.children.back().children.back().ids = { 1, 2 };
			node.children.back().children.emplace_back();
			node.children.back().children.back().ids = { 1, 4 };

			node.children.emplace_back();
			node.children.back().ids = { 2 };
			node.children.back().children.emplace_back();
			node.children.back().children.back().ids = { 2, 3 };
			node.children.back().children.emplace_back();
			node.children.back().children.back().ids = { 2, 4 };

			decltype(Miner::Node::ids) set{ 1, 2, 4 };
			Miner::Node::SubsetIterator iter{ node, {1, 2, 4} };

			const Miner::Node* tmp = iter.next();
			int c = 0;
			while (tmp != nullptr)
			{
				Assert::IsTrue(
					std::includes(
						set.begin(), set.end(),
						tmp->ids.begin(), tmp->ids.end()));
				Assert::IsTrue(tmp->ids != decltype(Miner::Node::ids){2, 3});
				tmp = iter.next();
				++c;
			}
			Assert::AreEqual(3, c);
		}
	};
}