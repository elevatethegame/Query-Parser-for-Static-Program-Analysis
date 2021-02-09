#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <set>

using namespace std;

class ResultUtil {
public:

	static vector<unordered_map<string, string>> convertPKBResult(unordered_map<string, set<string>> PKBResults, string leftSynonym, string rightSynonym);

	static vector<unordered_map<string, string>> convertPKBResult(unordered_map<string, set<string>> PKBResults, string synonym);

	static set<string> getCommonSynonyms(vector<unordered_map<string, string>> firstResult, vector<unordered_map<string, string>> secondResult);

	static vector<unordered_map<string, string>> getCartesianProduct(vector<unordered_map<string, string>> firstResult, vector<unordered_map<string, string>> secondResult);

	static vector<unordered_map<string, string>> getNaturalJoin(vector<unordered_map<string, string>> firstResult, vector<unordered_map<string, string>> secondResult, set<string> commonSynonyms);

};