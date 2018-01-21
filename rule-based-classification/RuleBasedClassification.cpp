#include <iostream>

#include <map>
#include <iostream>
#include <vector>
#include <set>
#include <cmath>   //because of the ceil function
#include <algorithm>
#include <iterator>

#include "../train-test-split/CSVReader.h"
#include "TestSet.h"
#include "Classifier.h"
#include "ConfusionMatrix.h"



int main(const int argc, const char* argv[])
{

	//std::string data_path = "E:\\Programowanie\\eager-ova\\rule-based-classification\\Datasets\\car-test";
	std::string data_path = "E:\\Programowanie\\eager-ova\\rule-based-classification\\Datasets\\adult";

	TestSet test(data_path);

	auto&& test_set = test.Get();
	auto&& class_ids = test.ClassIDs();

	RuleBasedClassifier classifier;
	classifier.loadRules(data_path);


	std::vector<int> results = classifier.classify(test_set);

	Confusion confusion_matrix(class_ids, results);
	confusion_matrix.print();


	results = classifier.classify(test_set, ClassifiationMethod::CAEP);

	Confusion confusion_matrix2(class_ids, results);
	confusion_matrix2.print();


	results = classifier.classify(test_set, ClassifiationMethod::CPAR);

	Confusion confusion_matrix3(class_ids, results);
	confusion_matrix3.print();

	results = classifier.classify(test_set, ClassifiationMethod::PCL);

	Confusion confusion_matrix4(class_ids, results);
	confusion_matrix4.print();
	


	return 0;
}


//// Test
//int main()
//{
//
//	std::vector<Rule> rules =
//	{
//		{ { 1,2,6,10 }, 1, 2, 3, 4, 5, 6 },
//		{ { 4,5,10 }, 1, 2, 3, 4, 5, 6 },
//		{ { 12 }, 1, 2, 3, 4, 5, 6 },
//		{ { 2, 4}, 1, 2, 3, 4, 5, 6 },
//		{ { 1 }, 1, 2, 3, 4, 5, 6 },
//		{ { 1, 2, 12 }, 1, 2, 3, 4, 5, 6 }
//	};
//
//	struct TrieNode *root = getNode();
//
//	// Construct trie
//	for (int i = 0; i < rules.size(); i++)
//		insertToTrie(root, &rules[i]);
//
//	RulePre test_case = { 1, 2, 4, 5, 10, 12 };
//
//	auto res = find_all_subsets(root, test_case);
//
//	return 0;
//}