#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <experimental/filesystem>

#include "Trie.h"


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
			//rules_map.insert(std::make_pair(rule.poprzednik, &rule));
			insertToTrie(rules_root, &rule);
		}

	}


	std::vector<int> classify(std::vector<RulePre> test_set, ClassifiationMethod method = ClassifiationMethod::K_MAX)
	{
		std::vector<int> results(test_set.size());

		for (unsigned int i = 0; i < test_set.size(); i++)
		{
			RulePre test_case = test_set[i];

			int max_subset_size = test_case.size();

			//std::vector<Rule*> good_rules;

			//std::vector< std::vector<int> > all_subsets = getAllSubsets(test_case);

			//for (auto&& subset : all_subsets)
			//{
			//	auto& it = rules_map.find(subset);
			//	if (it != rules_map.end())
			//	{
			//		good_rules.push_back(it->second);
			//		//std::cout << it->second->nastepnik;
			//	}
			//}

			std::vector<Rule*> good_rules = find_all_subsets(rules_root, test_case);


			std::sort(good_rules.begin(),
				good_rules.end(),
				[](auto &left, auto &right)
			{
				return left->wsparcie > right->wsparcie;
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
					float growth = rule->wzrost;
					float support = float(rule->wsparcie) / dataset_size;
					float strength;
					if (isinf(growth)) 
					{
						strength = support;
					}
					else
					{
						strength = support * (growth / (growth + 1));
					}
					votes[rule->nastepnik] += strength;
				}

				int case_result = std::distance(votes.begin(), std::max_element(votes.begin(), votes.end()));
				results[i] = case_result;

				break;

			}
			case ClassifiationMethod::PCL:
			{
				//PCL (Prediction by Collective Likelihood)

				//TODO - using parameters
				int m = 1000;
				int k = 15;

				std::vector<float> votes(num_classes);

				std::vector<std::vector<Rule*>> scores(num_classes);

				for (unsigned int i = 0; i < num_classes; i++)
				{
					auto it = std::copy_if(good_rules.begin(), good_rules.end(), std::back_inserter(scores[i]), [i](Rule* rule) {return rule->nastepnik == i; });

					float class_score = 0;

					for (unsigned int j = 0; j < scores[i].size(); j++)
					{
						if (j == k)
						{
							break;
						}

						//TODO: Generel EPs, not only matching rules
						class_score += float(scores[i][j]->wsparcie) / good_rules[j]->wsparcie;
						votes[i] = class_score;
					}
				}

				int case_result = std::distance(votes.begin(), std::max_element(votes.begin(), votes.end()));
				results[i] = case_result;

				//std::cout << "PCL: " << case_result;
				//std::cout << std::endl;
				break;

			}
			case ClassifiationMethod::CPAR:
			{
				//CPAR 

				// TODO: k as param
				int k = 5;

				std::vector<float> votes(num_classes);

				std::vector<std::vector<Rule*>> scores(num_classes);

				for (unsigned int i = 0; i < num_classes; i++)
				{
					auto it = std::copy_if(good_rules.begin(), good_rules.end(), std::back_inserter(scores[i]), [i](Rule* rule) {return rule->nastepnik == i; });

					float class_score = 0;

					for (unsigned int j = 0; j < scores[i].size(); j++)
					{
						if (j == k)
						{
							break;
						}

						/// total number of examples satysfying the rule's body
						float n_tot = std::round(scores[i][j]->wsparcie_poprzednika * dataset_size);

						/// number of examples which belong to c (predicted class of the rule) among n_tot
						float n_c = std::round(scores[i][j]->wsparcie * dataset_size);

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

				//std::cout << "CPAR: " << case_result;
				//std::cout << std::endl;
				break;


			}
			case ClassifiationMethod::K_MAX:
			default:
			{
				// Decyzja wiekszosciowa

				// TODO: Pierwsze k regul...

				std::vector<int> votes(num_classes);
				for (auto&& rule : good_rules)
				{
					//std::cout << rule->nastepnik << " (wsparcie: " << rule->wsparcie << ", wzrost: " << rule->wzrost << ")\n";
					votes[rule->nastepnik]++;
				}

				int case_result = std::distance(votes.begin(), std::max_element(votes.begin(), votes.end()));
				results[i] = case_result;

				//std::cout << "K_MAX: " << case_result;
				//std::cout << std::endl;
				break;
			}
			}

		}


		return results;
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

	
	/*template <typename Container> 
	struct container_hash {
		std::size_t operator()(Container const& c) const {
			return boost::hash_range(c.begin(), c.end());
		}
	};

	std::unordered_map<RulePre, Rule*, container_hash<RulePre>> rules_map;

	std::vector< std::vector<int> > getAllSubsets(std::vector<int> set)
	{
		std::vector< std::vector<int> > subset;
		std::vector<int> empty;
		subset.push_back(empty);

		for (int i = 0; i < set.size(); i++)
		{
			std::vector< std::vector<int> > subsetTemp = subset;

			for (int j = 0; j < subsetTemp.size(); j++)
				subsetTemp[j].push_back(set[i]);

			for (int j = 0; j < subsetTemp.size(); j++)
				subset.push_back(subsetTemp[j]);
		}
		return subset;
	}*/
};