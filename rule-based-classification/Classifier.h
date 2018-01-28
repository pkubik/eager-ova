#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <experimental/filesystem>

#include "Trie.h"
#include "IOUtils.h"


enum class ClassifiationMethod
{
	K_MAX,
	CAEP,
	PCL,
	CPAR
};

class RuleBasedClassifier
{

public:
	RuleBasedClassifier()
	{
	}

	~RuleBasedClassifier()
	{
		delete(rules_root);
	}

	void processRule(const std::string& rule)
	{
		int i = 0;
		int j = rule.find(';');

		std::string poprzedniki = rule.substr(i, j - i);
		i = ++j + 1;
		j = rule.find(';', j);

		std::string class_name = rule.substr(i, j - i);
		i = ++j;

		std::string rule_info_s = rule.substr(i, rule.length());

		RulePre pop;
		
		i = 0;
		j = poprzedniki.find(',');

		if (poprzedniki.length() == 1)
		{
			pop.push_back(std::stoi(poprzedniki));
		}
		else
		{
			while (j > 0)
			{
				pop.push_back(std::stoi(poprzedniki.substr(i, j - i)));
				i = ++j;
				j = poprzedniki.find(',', j);

				if (j < 0)
				{
					pop.push_back(std::stoi(poprzedniki.substr(i, poprzedniki.length())));
				}
			}
		}

		std::vector<double> rule_info(5);

		i = 0;
		j = rule_info_s.find(',');

		for (unsigned int n = 0; n < 5; n++)
		{
			rule_info[n] = std::stod(rule_info_s.substr(i, j - i));
			i = ++j;
			j = rule_info_s.find(',', j);
		}

		int class_id = class_ids[class_name];

		Rule processed_rule(pop, class_id, rule_info);

		rules.push_back(processed_rule);
	}

	void loadRules(std::string data_path)
	{
		namespace fs = std::experimental::filesystem;

		auto labels_path = fs::path(data_path).append("labels.txt").string();

		loadLabelsMap(labels_path);

		auto train_size_path = fs::path(data_path).append("train_size.txt").string();

		loadTrainDatasetSize(train_size_path);

		auto rules_path = fs::path(data_path).append("rules.txt").string();

		std::ifstream rules_file(rules_path);

		std::string tmp;

		while (!rules_file.eof())
		{
			getline(rules_file, tmp, '\n');
			if (!tmp.empty())
			{
				processRule(tmp);
			}
			tmp.clear();
		}

		for (auto& rule : rules)
		{
			insertToTrie(rules_root, &rule);
		}

	}



	std::vector<int> classify(std::vector<RulePre> test_set, Params classification_params)
	{
		std::string method_param = classification_params.METHOD;

		ClassifiationMethod method = ClassifiationMethod::K_MAX;
		if (method_param == "K_MAX")
		{
			method = ClassifiationMethod::K_MAX;
		}
		else if (method_param == "PCL")
		{
			method = ClassifiationMethod::PCL;
		}
		else if (method_param == "CAEP")
		{
			method = ClassifiationMethod::CAEP;
		}
		else if (method_param == "CPAR")
		{
			method = ClassifiationMethod::CPAR;
		}
		else
		{
			std::cerr << "Wrong METHOD param! Using default (K_MAX)" << std::endl;
		}

		std::vector<int> results(test_set.size());

		for (unsigned int i = 0; i < test_set.size(); i++)
		{
			RulePre test_case = test_set[i];

			int max_subset_size = test_case.size();

			std::vector<Rule*> good_rules = find_all_subsets(rules_root, test_case);


			std::sort(good_rules.begin(),
				good_rules.end(),
				[](auto &left, auto &right)
			{
				return left->support > right->support;
			});


			switch (method)
			{
			case ClassifiationMethod::CAEP:
			{
				// CAEP (Classification by Aggregating Emerging Patterns)
				// Version without normalization

				std::vector<float> votes(num_classes);

				for (auto&& rule : good_rules)
				{
					float growth = rule->growth;
					float support = float(rule->support) / dataset_size;
					float strength;
					if (isinf(growth)) 
					{
						strength = support;
					}
					else
					{
						strength = support * (growth / (growth + 1));
					}
					votes[rule->consequent] += strength;
				}

				int case_result = std::distance(votes.begin(), std::max_element(votes.begin(), votes.end()));
				results[i] = case_result;

				break;

			}
			case ClassifiationMethod::PCL:
			{
				//PCL (Prediction by Collective Likelihood)

				int k = classification_params.K_PCL;

				std::vector<float> votes(num_classes);

				std::vector<std::vector<Rule*>> scores(num_classes);

				for (unsigned int i = 0; i < num_classes; i++)
				{
					auto it = std::copy_if(good_rules.begin(), good_rules.end(), std::back_inserter(scores[i]), [i](Rule* rule) {return rule->consequent == i; });

					float class_score = 0;

					for (unsigned int j = 0; j < scores[i].size(); j++)
					{
						if (j == k)
						{
							break;
						}

						class_score += float(scores[i][j]->support) / good_rules[j]->support;
						votes[i] = class_score;
					}
				}

				int case_result = std::distance(votes.begin(), std::max_element(votes.begin(), votes.end()));
				results[i] = case_result;

				break;

			}
			case ClassifiationMethod::CPAR:
			{
				//CPAR 

				int k = classification_params.K_CPAR;

				std::vector<float> votes(num_classes);

				std::vector<std::vector<Rule*>> scores(num_classes);

				for (unsigned int i = 0; i < num_classes; i++)
				{
					auto it = std::copy_if(good_rules.begin(), good_rules.end(), std::back_inserter(scores[i]), [i](Rule* rule) {return rule->consequent == i; });

					float class_score = 0;

					for (unsigned int j = 0; j < scores[i].size(); j++)
					{
						if (j == k)
						{
							break;
						}

						/// total number of examples satysfying the rule's body
						float n_tot = scores[i][j]->antecedent_support;

						/// number of examples which belong to c (predicted class of the rule) among n_tot
						float n_c = scores[i][j]->support;

						float laplace_accuracy = (n_c + 1) / (n_tot + num_classes);

						class_score += laplace_accuracy;
					}

					if (scores[i].size() > 0)
					{
						votes[i] = class_score / std::min(k, int(scores[i].size()));
					}

				}

				int case_result = std::distance(votes.begin(), std::max_element(votes.begin(), votes.end()));
				results[i] = case_result;

				break;


			}
			case ClassifiationMethod::K_MAX:
			default:
			{
				// Decyzja wiekszosciowa

				int k = classification_params.K_MAX;

				std::vector<int> votes(num_classes);

				for (int i = 0; i < good_rules.size(); i++)
				{
					if (i == k)
					{
						break;
					}
					votes[good_rules[i]->consequent]++;
				}


				int case_result = std::distance(votes.begin(), std::max_element(votes.begin(), votes.end()));
				results[i] = case_result;

				break;
			}
			}

		}

		return results;
	}

	std::unordered_map<unsigned short int, std::string> getLabelsMap()
	{
		return labels;
	}

private:

	int dataset_size = 10;

	int num_classes;

	std::unordered_map<std::string, unsigned short int> class_ids;
	std::unordered_map<unsigned short int, std::string> labels;

	std::vector<Rule> rules;
	TrieNode *rules_root = getNode();


	void loadLabelsMap(std::string labels_path)
	{
		std::ifstream labels_file(labels_path);

		std::string label;
		unsigned short int class_id;
		while (labels_file >> label >> class_id)
		{
			class_ids.insert(std::make_pair(label, class_id));
			labels.insert(std::make_pair(class_id, label));
		}
		num_classes = class_ids.size();
	}

	void loadTrainDatasetSize(std::string train_size_path)
	{
		std::ifstream train_size_file(train_size_path);

		int train_size;

		train_size_file >> train_size;

		dataset_size = train_size;
	}

};