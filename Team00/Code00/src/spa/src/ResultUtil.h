#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include "ResultsTable.h"

using namespace std;

class ResultUtil {
public:
	
	static set<string> getCommonSynonyms(vector<string> PKBResultSynonyms, set<string> currentResultSynonyms);

	static ResultsTable* getCartesianProduct(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
		ResultsTable* currentResults);

	static ResultsTable* ResultUtil::getNaturalJoin(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
		ResultsTable* currentResults, set<string> commonSynonyms);

private:
	static ResultsTable* ResultUtil::getNaturalJoinTwoSynonymsCommon(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
		ResultsTable* currentResults);

	static ResultsTable* ResultUtil::getNaturalJoinOneSynonymCommon(unordered_map <string, set<string>> PKBResults, string commonSynonym,
		string uncommonSynonym, bool isLeftSynonymCommon, ResultsTable* currentResults);
};