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

	};
}