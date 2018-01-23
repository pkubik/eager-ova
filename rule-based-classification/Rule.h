#pragma once
#include<vector>


typedef std::vector<int> RulePre;

class Rule
{
public:

	Rule(RulePre pop, int nast, int ws_p, int ws_n, int ws, double zau, double wzr)
		:
		antecedent{ pop },
		consequent{ nast },
		antecedent_support{ ws_p },
		consequent_support{ ws_n },
		support{ ws },
		confidence{ zau },
		growth{ wzr } {}

	Rule(RulePre pop, int nas, std::vector<double> rule_info)
		:
		antecedent{ pop },
		consequent{ nas },
		antecedent_support{ int(rule_info[0]) },
		consequent_support{ int(rule_info[1]) },
		support{ int(rule_info[2]) },
		confidence{ rule_info[3] },
		growth{ rule_info[4] } {}

	RulePre antecedent;
	int consequent;
	int antecedent_support;
	int consequent_support;
	int support;
	double confidence;
	double growth;
};