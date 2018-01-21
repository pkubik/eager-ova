#pragma once
#include<vector>


typedef std::vector<int> RulePre;

class Rule
{
public:

	Rule(RulePre pop, int nast, int ws_p, int ws_n, int ws, double zau, double wzr)
		:
		poprzednik{ pop },
		nastepnik{ nast },
		wsparcie_poprzednika{ ws_p },
		wsparcie_nastepnika{ ws_n },
		wsparcie{ ws },
		zaufanie{ zau },
		wzrost{ wzr } {}

	Rule(RulePre pop, int nas, std::vector<double> rule_info)
		:
		poprzednik{ pop },
		nastepnik{ nas },
		wsparcie_poprzednika{ int(rule_info[0]) },
		wsparcie_nastepnika{ int(rule_info[1]) },
		wsparcie{ int(rule_info[2]) },
		zaufanie{ rule_info[3] },
		wzrost{ rule_info[4] } {}

	RulePre poprzednik;
	int nastepnik;
	int wsparcie_poprzednika;
	int wsparcie_nastepnika;
	int wsparcie;
	double zaufanie;
	double wzrost;
};