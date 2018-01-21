#pragma once
#include<boost/container/flat_map.hpp>
#include<vector>

#include "Rule.h"

//using namespace std;


// trie node
struct TrieNode
{
	//struct TrieNode *children[ALPHABET_SIZE];

	TrieNode() {};
	~TrieNode()
	{
		for (auto&& child : children)
		{
			delete(child.second);
		}
	}

	boost::container::flat_map<int, TrieNode*> children;

	Rule* rule = nullptr;

	// isEndOfWord is true if the node represents
	bool isEndOfWord = false;
};

// Returns new trie node (initialized to NULLs)
struct TrieNode *getNode(void)
{
	struct TrieNode *pNode = new TrieNode;

	return pNode;
}


void insertToTrie(struct TrieNode *root, Rule* rule)
{
	struct TrieNode *pCrawl = root;

	RulePre key = rule->poprzednik;

	for (int i = 0; i < key.size(); i++)
	{
		int index = key[i];

		auto insert_result = pCrawl->children.insert(std::make_pair(index, getNode()));
		pCrawl = insert_result.first->second;
	}

	// mark last node as leaf
	pCrawl->isEndOfWord = true;
	pCrawl->rule = rule;
}


void find_subsets(struct TrieNode *node, RulePre::iterator start, RulePre::iterator end, std::vector<Rule*> &rules)
{
	for (auto it = start; it != end; it++)
	{
		auto elem = node->children.find(*it);
		if (elem != node->children.end())
		{
			// check if rule
			if (elem->second->isEndOfWord)
			{
				rules.push_back(elem->second->rule);
			}
			find_subsets(elem->second, it + 1, end, rules);
		}
		else
		{
			continue;
		}
	}
}

std::vector<Rule*> find_all_subsets(struct TrieNode *root, RulePre test_case)
{
	auto it = test_case.begin();
	auto end = test_case.end();

	std::vector<Rule*> rules;

	find_subsets(root, it, end, rules);

	return rules;
}



