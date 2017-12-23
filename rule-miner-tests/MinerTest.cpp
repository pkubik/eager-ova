#include "stdafx.h"
#include "CppUnitTest.h"
#include "../eager-ova/Miner.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ruleminertests
{
	void buildSampleTree(Miner::Node& root)
	{
		root.support = 10;
		root.classSupports = { 10, 11, 12 };

		root.children.emplace_back(&root);
		root.children.back().ids = { 1 };
		root.children.back().support = 10;
		root.children.back().classSupports = { 10, 8, 2 };
		
		root.children.back().children.emplace_back(&root);
		root.children.back().children.back().ids = { 1, 2 };
		root.children.back().children.back().support = 6;
		root.children.back().children.back().classSupports = { 10, 8, 2 };
		
		root.children.back().children.emplace_back(&root);
		root.children.back().children.back().ids = { 1, 4 };
		root.children.back().children.back().support = 3;
		root.children.back().children.back().classSupports = { 3, 5, 6 };

		root.children.emplace_back(&root);
		root.children.back().ids = { 2 };
		root.children.back().support = 8;
		root.children.back().classSupports = { 10, 8, 2 };
		
		root.children.back().children.emplace_back(&root);
		root.children.back().children.back().ids = { 2, 3 };
		root.children.back().children.back().support = 1;
		root.children.back().children.back().classSupports = { 1, 1, 1 };

		root.children.back().children.emplace_back(&root);
		root.children.back().children.back().ids = { 2, 4 };
		root.children.back().children.back().support = 4;
		root.children.back().children.back().classSupports = { 5, 4, 3 };
	}

	TEST_CLASS(MinerTest)
	{
	public:

		TEST_METHOD(simpleNode)
		{
			Miner::Node node;
			Assert::IsTrue(node.isRoot());
			Assert::IsTrue(node.children.empty());

			node.setTidset({ 1, 2, 4, 6 });
			const std::vector<std::vector<Id>> classTidsets
			{
				{1, 2, 3, 5, 6},
				{2, 5, 6}
			};

			Assert::AreEqual(4u, node.getSupport());
			node.calculateClassSupports(classTidsets);
			Assert::AreEqual(3u, node.classSupports[0]);
			Assert::AreEqual(2u, node.classSupports[1]);

			node.simplify(classTidsets);
			Assert::IsTrue(node.isSimplified());
			Assert::IsTrue(node.tidset.empty());
			Assert::AreEqual(4u, node.getSupport());
			Assert::AreEqual(3u, node.classSupports[0]);
			Assert::AreEqual(2u, node.classSupports[1]);
		}

		TEST_METHOD(subsetIteration)
		{
			Miner::Node node;
			buildSampleTree(node);

			decltype(Miner::Node::ids) set{ 1, 2, 4 };
			Miner::Node::SubsetIterator iter{ node, set };

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

		TEST_METHOD(subsetsMinSupport)
		{
			Miner::Node node;
			buildSampleTree(node);

			const auto subsetsMinClassSupports = node.subsetsMinClassSupports({ 1, 2, 4 });
			Assert::AreEqual(3u, subsetsMinClassSupports[0]);
			Assert::AreEqual(4u, subsetsMinClassSupports[1]);
			Assert::AreEqual(2u, subsetsMinClassSupports[2]);
		}
	};
}