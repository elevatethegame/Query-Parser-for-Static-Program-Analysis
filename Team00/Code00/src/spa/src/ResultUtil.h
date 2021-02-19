#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include <memory>
#include "ResultsTable.h"

using namespace std;

class ResultUtil {
public:
	
	static set<string> getCommonSynonyms(vector<string> PKBResultSynonyms, set<string> currentResultSynonyms);

	static shared_ptr<ResultsTable> getCartesianProduct(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults);

	static shared_ptr<ResultsTable> ResultUtil::getNaturalJoin(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults, set<string> commonSynonyms);

private:
	static shared_ptr<ResultsTable> ResultUtil::getNaturalJoinTwoSynonymsCommon(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
		shared_ptr<ResultsTable> currentResults);

	static shared_ptr<ResultsTable> ResultUtil::getNaturalJoinOneSynonymCommon(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
		bool isLeftSynonymCommon, shared_ptr<ResultsTable> currentResults);
};