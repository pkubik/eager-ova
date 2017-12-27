#include <memory>
#include "CppUnitTest.h"
#include "../eager-ova/Miner.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ruleminertests
{
	void buildSampleTree(Node& root)
	{
		root.support = 33;
		root.classSupports = { 10, 11, 12 };
		root.classValidity = { true, true, true };

		root.children.emplace_back(std::make_unique<Node>(&root));
		root.children.back()->ids = { 1 };
		root.children.back()->support = 23;
		root.children.back()->classSupports = { 10, 7, 6 };
		root.children.back()->classValidity = { true, true, true };
		
		root.children.back()->children.emplace_back(std::make_unique<Node>(&root));
		root.children.back()->children.back()->ids = { 1, 2 };
		root.children.back()->children.back()->support = 19;
		root.children.back()->children.back()->classSupports = { 10, 7, 2 };
		root.children.back()->children.back()->classValidity = { true, true, false };
		
		root.children.back()->children.emplace_back(std::make_unique<Node>(&root));
		root.children.back()->children.back()->ids = { 1, 4 };
		root.children.back()->children.back()->support = 14;
		root.children.back()->children.back()->classSupports = { 3, 5, 6 };
		root.children.back()->children.back()->classValidity = { true, true, true };

		root.children.emplace_back(std::make_unique<Node>(&root));
		root.children.back()->ids = { 2 };
		root.children.back()->support = 20;
		root.children.back()->classSupports = { 10, 8, 2 };
		root.children.back()->classValidity = { true, true, true };
		
		root.children.back()->children.emplace_back(std::make_unique<Node>(&root));
		root.children.back()->children.back()->ids = { 2, 3 };
		root.children.back()->children.back()->support = 3;
		root.children.back()->children.back()->classSupports = { 1, 1, 1 };
		root.children.back()->children.back()->classValidity = { true, true, true };

		root.children.back()->children.emplace_back(std::make_unique<Node>(&root));
		root.children.back()->children.back()->ids = { 2, 4 };
		root.children.back()->children.back()->support = 20;
		root.children.back()->children.back()->classSupports = { 10, 8, 2 };
		root.children.back()->children.back()->classValidity = { true, true, true };
	}

	TEST_CLASS(MinerTest)
	{
	public:

		TEST_METHOD(simpleNode)
		{
			Node node;
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

			node.simplify();
			Assert::IsTrue(node.isSimplified());
			Assert::IsTrue(node.tidset.empty());
			Assert::AreEqual(4u, node.getSupport());
			Assert::AreEqual(3u, node.classSupports[0]);
			Assert::AreEqual(2u, node.classSupports[1]);
		}

		TEST_METHOD(subsetIteration)
		{
			Node node;
			buildSampleTree(node);

			decltype(Node::ids) set{ 1, 2, 4 };
			Node::SubsetIterator iter{ node, set };

			const Node* tmp = iter.next();
			int c = 0;
			while (tmp != nullptr)
			{
				Assert::IsTrue(
					std::includes(
						set.begin(), set.end(),
						tmp->ids.begin(), tmp->ids.end()));
				Assert::IsTrue(tmp->ids != decltype(Node::ids){2, 3});
				tmp = iter.next();
				++c;
			}
			Assert::AreEqual(3, c);
		}

		TEST_METHOD(subsetsMinSupport)
		{
			Node node;
			buildSampleTree(node);

			const auto subsetsMinSupport = node.subsetsMinSupport({ 1, 2, 4 });
			Assert::AreEqual(14u, subsetsMinSupport);
		}

		TEST_METHOD(subsetsMinClassSupports)
		{
			Node node;
			buildSampleTree(node);

			const auto subsetsMinClassSupports = node.subsetsMinClassSupports({ 1, 2, 4 });
			Assert::AreEqual(3u, subsetsMinClassSupports[0]);
			Assert::AreEqual(5u, subsetsMinClassSupports[1]);
			Assert::AreEqual(2u, subsetsMinClassSupports[2]);
		}

		TEST_METHOD(classValidity)
		{
			Node node;
			buildSampleTree(node);

			node.children[0]->invalidateNonGenerators();
			Assert::IsTrue(node.children[0]->isAnyClassValid());
			Assert::IsTrue(node.children[0]->classValidity[0]);
			Assert::IsTrue(node.children[0]->classValidity[1]);
			Assert::IsTrue(node.children[0]->classValidity[2]);

			node.children[1]->children[1]->invalidateNonGenerators();
			Assert::IsFalse(node.children[1]->children[1]->isAnyClassValid());
		}

		TEST_METHOD(nodeJoin)
		{
			const std::vector<std::vector<Id>> classTidsets
			{
				{ 1, 3, 4, 7 },
				{ 2, 5, 6 }
			};

			Node root = createRoot(classTidsets);
			Assert::AreEqual(7u, root.support);
			Assert::AreEqual(4u, root.classSupports[0]);
			Assert::AreEqual(3u, root.classSupports[1]);

			addRootChild(root, 0, { 1, 2, 3, 4 }, classTidsets);
			Assert::AreEqual(4u, root.children[0]->support);
			Assert::AreEqual(3u, root.children[0]->classSupports[0]);
			Assert::AreEqual(1u, root.children[0]->classSupports[1]);
			
			addRootChild(root, 1, { 1, 2, 5 }, classTidsets);
			Assert::AreEqual(3u, root.children[1]->support);
			Assert::AreEqual(1u, root.children[1]->classSupports[0]);
			Assert::AreEqual(2u, root.children[1]->classSupports[1]);

			auto joinedNode = root.children[0]->join(*root.children[1], classTidsets);
			Assert::AreEqual(2ull, joinedNode->ids.size());
			Assert::AreEqual(2u, joinedNode->support);
			Assert::AreEqual(1u, joinedNode->classSupports[0]);
			Assert::AreEqual(1u, joinedNode->classSupports[1]);

			joinedNode->invalidateNonGenerators();
			Assert::IsTrue(joinedNode->isAnyClassValid());
		}

		TEST_METHOD(sampleMine)
		{
			auto dataset = Dataset::fromFile("sample.csv");
			const auto& requiredValues = dataset.getRequiredValues();
			const std::vector<Id> classIds{ requiredValues.begin(), requiredValues.end() };
			Miner miner{ classIds };

			auto rules = miner.mine(dataset.getItems());
			Assert::AreEqual(2ull, rules.size());
		}

		TEST_METHOD(growthTest)
		{
			Assert::AreEqual(1.0, ruleGrowth(4, 3, 2, 6));
			Assert::AreEqual(0.0, ruleGrowth(4, 3, 0, 10));
		}
	};
}