#include "ResultUtil.h"
#include <map>

vector<unordered_map<string, string>> ResultUtil::convertPKBResult(unordered_map<string, set<string>> PKBResults, string leftSynonym, string rightSynonym) {
	vector<unordered_map<string, string>> results;
	unordered_map<string, set<string>>::iterator it;
	for (it = PKBResults.begin(); it != PKBResults.end(); it++) {

		for (auto rightSynonymValue : it->second) {
			unordered_map<string, string> result;
			result.insert({ leftSynonym, it->first });
			result.insert({ rightSynonym, rightSynonymValue });
			results.push_back(result);
		}
	}

	return results;
}

vector<unordered_map<string, string>> ResultUtil::convertPKBResult(unordered_map<string, set<string>> PKBResults, string synonym) {
	vector<unordered_map<string, string>> results;
	unordered_map<string, set<string>>::iterator it;
	for (it = PKBResults.begin(); it != PKBResults.end(); it++) {

		for (auto synonymValue : it->second) {
			unordered_map<string, string> result;
			result.insert({ synonym, synonymValue });
			results.push_back(result);
		}
	}

	return results;
}

set<string> ResultUtil::getCommonSynonyms(vector<unordered_map<string, string>> firstResult, vector<unordered_map<string, string>> secondResult) {
	set<string> commonSynonyms;

	if (firstResult.size() == 0 || secondResult.size() == 0) {
		return commonSynonyms;
	}

	map<string, string> firstMap(firstResult.at(0).begin(), firstResult.at(0).end());
	map<string, string> secondMap(secondResult.at(0).begin(), secondResult.at(0).end());

	map<string, string>::iterator it1 = firstMap.begin();
	map<string, string>::iterator it2 = secondMap.begin();

	// using fact that map entries are sorted by default according to key, this comparison takes linear time
	while (it1 != firstMap.end() && it2 != secondMap.end()) {
		if (it1->first < it2->first) {
			++it1;
		}
		else if (it2->first < it1->first) {
			++it2;
		}
		else { // equal keys
			commonSynonyms.insert(it1->first);
			++it1;
			++it2;
		}
	}

	return commonSynonyms;

}

// used to merge 2 PKB results that have no common synonyms
vector<unordered_map<string, string>> ResultUtil::getCartesianProduct(vector<unordered_map<string, string>> firstResult,
	vector<unordered_map<string, string>> secondResult) {
	vector<unordered_map<string, string>> results;

	for (vector<unordered_map<string, string>>::iterator firstResultIt = firstResult.begin();
		firstResultIt != firstResult.end(); firstResultIt++) {
		unordered_map<string, string> firstMap = *firstResultIt;

		for (vector<unordered_map<string, string>>::iterator secondResultIt = secondResult.begin();
			secondResultIt != secondResult.end(); secondResultIt++) {
			unordered_map<string, string> secondMap = *secondResultIt;

			unordered_map<string, string> firstMapCopy(firstMap);
			unordered_map<string, string> secondMapCopy(secondMap);
			firstMapCopy.insert(secondMapCopy.begin(), secondMapCopy.end());

			results.push_back(firstMapCopy);
		}
	}

	return results;
}

// used to merge 2 PKB results that have some common synonym
vector<unordered_map<string, string>> ResultUtil::getNaturalJoin(vector<unordered_map<string, string>> firstResult,
	vector<unordered_map<string, string>> secondResult, set<string> commonSynonyms) {
	vector<unordered_map<string, string>> results;

	for (vector<unordered_map<string, string>>::iterator firstResultIt = firstResult.begin();
		firstResultIt != firstResult.end(); firstResultIt++) {
		unordered_map<string, string> firstMap = *firstResultIt;

		for (vector<unordered_map<string, string>>::iterator secondResultIt = secondResult.begin();
			secondResultIt != secondResult.end(); secondResultIt++) {
			unordered_map<string, string> secondMap = *secondResultIt;

			bool matchAll = true;
			for (auto synonym : commonSynonyms) {
				if (firstMap.find(synonym)->second != secondMap.find(synonym)->second) {
					matchAll = false;
					break;
				}
			}

			if (matchAll) {
				unordered_map<string, string> firstMapCopy(firstMap);
				unordered_map<string, string> secondMapCopy(secondMap);
				firstMapCopy.insert(secondMapCopy.begin(), secondMapCopy.end());
				results.push_back(firstMapCopy);

			}

		}
	}

	return results;
}