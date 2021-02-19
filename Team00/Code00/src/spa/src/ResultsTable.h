#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

using namespace std;

class ResultsTable {
private:
	unordered_map<string, int> aSynonymIndex;
	vector<vector<string>> aValues;
	bool noResult = false;

public:
	ResultsTable();
	unordered_map<string, int> getSynonymIndexMap();
	vector<vector<string>> getTableValues();
	set<string> getSynonyms();
	void setTable(unordered_map<string, int> synonymIndex, vector<vector<string>> values);
	bool isNoResult();
	void setIsNoResult();

	bool isTableEmpty();
	void populate(unordered_map<string, set<string>> PKBResult, vector<string> synonyms);
};