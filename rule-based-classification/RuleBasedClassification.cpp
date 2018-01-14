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

	std::string data_path = "E:\\Programowanie\\eager-ova\\rule-based-classification\\Datasets\\car-test";

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