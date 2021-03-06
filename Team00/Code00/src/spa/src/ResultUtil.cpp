#include "ResultUtil.h"
#include <map>

set<string> ResultUtil::getCommonSynonyms(vector<string> PKBResultSynonyms, set<string> currentResultSynonyms) {
	set<string> commonSynonyms;

	for (vector<string>::iterator it = PKBResultSynonyms.begin(); it != PKBResultSynonyms.end(); it++) {
		if (currentResultSynonyms.find(*it) != currentResultSynonyms.end()) {
			commonSynonyms.insert(*it);
		}
	}

	return commonSynonyms;

}

// used to merge PKB Map results with ResultsTable object that have no common synonyms
// synonyms assumed to be size 2
// all inputs assumed to be non empty
shared_ptr<ResultsTable> ResultUtil::getCartesianProductFromMap(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
	shared_ptr<ResultsTable> currentResults) {
	unordered_map<string, int> synonymIndex = currentResults->getSynonymIndexMap();
	vector<vector<string>> tableValues = currentResults->getTableValues();
	vector<vector<string>> newTableValues;

	string leftSynonym = synonyms.at(0);
	string rightSynonym = synonyms.at(1);
	synonymIndex.insert({ leftSynonym, synonymIndex.size() });
	synonymIndex.insert({ rightSynonym, synonymIndex.size() });

	for (unordered_map <string, set<string>>::iterator pkbResultIt = PKBResults.begin();
		pkbResultIt != PKBResults.end(); pkbResultIt++) {
		string leftSynonymValue = pkbResultIt->first;
		set<string> rightSynonymValues = pkbResultIt->second;

		for (set<string>::iterator synonymIt = rightSynonymValues.begin(); synonymIt != rightSynonymValues.end();
			synonymIt++) {
			string rightSynonymValue = *synonymIt;

			for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
				vector<string> rowCopy(*it);
				rowCopy.push_back(leftSynonymValue);
				rowCopy.push_back(rightSynonymValue);
				newTableValues.push_back(rowCopy);
			}
		}
	}

	currentResults->setTable(synonymIndex, newTableValues);

	return currentResults;
}

// used to merge PKB Set results with ResultsTable object that have no common synonyms
// all inputs assumed to be non empty
shared_ptr<ResultsTable> ResultUtil::getCartesianProductFromSet(set<string> PKBResults, string synonym,
	shared_ptr<ResultsTable> currentResults) {
	unordered_map<string, int> synonymIndex = currentResults->getSynonymIndexMap();
	synonymIndex.insert({ synonym, synonymIndex.size() });

	vector<vector<string>> tableValues = currentResults->getTableValues();
	vector<vector<string>> newTableValues;
	for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {

		for (set<string>::iterator pkbResultIt = PKBResults.begin(); pkbResultIt != PKBResults.end();
			pkbResultIt++) {
			vector<string> rowCopy(*it);
			string valueToBeAdded = *pkbResultIt;
			rowCopy.push_back(valueToBeAdded);
			newTableValues.push_back(rowCopy);
		}

	}

	currentResults->setTable(synonymIndex, newTableValues);

	return currentResults;
}


// used to merge PKB Map results with ResultsTable object that have some common synonyms
// Synonyms is assumed to be of size 2
shared_ptr<ResultsTable> ResultUtil::getNaturalJoinFromMap(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
	shared_ptr<ResultsTable> currentResults, set<string> commonSynonyms) {

	string leftSynonym = synonyms.at(0);
	string rightSynonym = synonyms.at(1);

	// both are common synonyms
	if (commonSynonyms.find(leftSynonym) != commonSynonyms.end() &&
		commonSynonyms.find(rightSynonym) != commonSynonyms.end()) {
		currentResults = getNaturalJoinTwoSynonymsCommon(PKBResults, synonyms, currentResults);
	}
	else { // exactly one synonym is common
		string commonSynonym;
		bool isLeftSynonymCommon = (commonSynonyms.find(leftSynonym) != commonSynonyms.end());
		currentResults = getNaturalJoinOneSynonymCommon(PKBResults, synonyms, isLeftSynonymCommon,
			currentResults);
	}

	return currentResults;
}

// used to merge PKB Set results with ResultsTable object that have a common synonym
shared_ptr<ResultsTable> ResultUtil::getNaturalJoinFromSet(set<string> PKBResults, string synonym,
	shared_ptr<ResultsTable> currentResults, set<string> commonSynonyms) {
	unordered_map<string, int> synonymIndex = currentResults->getSynonymIndexMap();
	vector<vector<string>> tableValues = currentResults->getTableValues();
	vector<vector<string>> newTableValues;

	int index = synonymIndex.find(synonym)->second;

	for (set<string>::iterator pkbResultIt = PKBResults.begin(); pkbResultIt != PKBResults.end();
		pkbResultIt++) {
		string valueFromPKB = *pkbResultIt;

		for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
			vector<string> row = *it;
			if (row.at(index) == valueFromPKB) {
				newTableValues.push_back(row);
			}
		}
	}

	currentResults->setTable(synonymIndex, newTableValues);

	return currentResults;
}

shared_ptr<ResultsTable> ResultUtil::getNaturalJoinTwoSynonymsCommon(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
	shared_ptr<ResultsTable> currentResults) {
	unordered_map<string, int> synonymIndex = currentResults->getSynonymIndexMap();
	string leftSynonym = synonyms.at(0);
	string rightSynonym = synonyms.at(1);
	int leftSynonymIndex = synonymIndex.find(leftSynonym)->second;
	int rightSynonymIndex = synonymIndex.find(rightSynonym)->second;

	vector<vector<string>> tableValues = currentResults->getTableValues();
	vector<vector<string>> newTableValues;

	for (unordered_map <string, set<string>>::iterator pkbResultsIt = PKBResults.begin(); pkbResultsIt != PKBResults.end();
		pkbResultsIt++) {
		string leftSynonymValue = pkbResultsIt->first;
		set<string> rightSynonymValues = pkbResultsIt->second;

		for (set<string>::iterator rowIt = rightSynonymValues.begin(); rowIt != rightSynonymValues.end(); rowIt++) {
			string rightSynonymValue = *rowIt;
			for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
				vector<string> row = *it;
				if (row.at(leftSynonymIndex) == leftSynonymValue && row.at(rightSynonymIndex) == rightSynonymValue) {
					vector<string> rowCopy(row);
					newTableValues.push_back(rowCopy);
				}
			}
		}
	}

	currentResults->setTable(synonymIndex, newTableValues);
	return currentResults;
}

shared_ptr<ResultsTable> ResultUtil::getNaturalJoinOneSynonymCommon(unordered_map <string, set<string>> PKBResults, vector<string> synonyms,
	bool isLeftSynonymCommon, shared_ptr<ResultsTable> currentResults) {
	unordered_map<string, int> synonymIndex = currentResults->getSynonymIndexMap();
	string leftSynonym = synonyms.at(0);
	string rightSynonym = synonyms.at(1);
	string commonSynonym;
	string uncommonSynonym;
	if (isLeftSynonymCommon) {
		commonSynonym = leftSynonym;
		uncommonSynonym = rightSynonym;
	}
	else {
		commonSynonym = rightSynonym;
		uncommonSynonym = leftSynonym;
	}
	int commonSynonymIndex = synonymIndex.find(commonSynonym)->second;
	synonymIndex.insert({ uncommonSynonym, synonymIndex.size() });

	vector<vector<string>> tableValues = currentResults->getTableValues();
	vector<vector<string>> newTableValues;

	if (isLeftSynonymCommon) {
		for (unordered_map <string, set<string>>::iterator pkbResultIt = PKBResults.begin();
			pkbResultIt != PKBResults.end(); pkbResultIt++) {
			string leftSynonymValue = pkbResultIt->first;
			set<string> rightSynonymValues = pkbResultIt->second;
			
			for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
				vector<string> row = *it;
				if (row.at(commonSynonymIndex) == leftSynonymValue) {
					for (set<string>::iterator setIt = rightSynonymValues.begin(); setIt != rightSynonymValues.end(); setIt++) {
						vector<string> rowCopy(row);
						rowCopy.push_back(*setIt);
						newTableValues.push_back(rowCopy);
					}
				}
			}
		}
	}
	else {
		for (unordered_map <string, set<string>>::iterator pkbResultIt = PKBResults.begin();
			pkbResultIt != PKBResults.end(); pkbResultIt++) {
			string leftSynonymValue = pkbResultIt->first;
			set<string> rightSynonymValues = pkbResultIt->second;

			for (set<string>::iterator setIt = rightSynonymValues.begin(); setIt != rightSynonymValues.end(); setIt++) {
				string rightSynonymValue = *setIt;

				for (vector<vector<string>>::iterator it = tableValues.begin(); it != tableValues.end(); it++) {
					vector<string> row = *it;
					if (row.at(commonSynonymIndex) == rightSynonymValue) {
						vector<string> rowCopy(row);
						rowCopy.push_back(leftSynonymValue);
						newTableValues.push_back(rowCopy);
					}
				}
			}
		}
	}

	currentResults->setTable(synonymIndex, newTableValues);
	return currentResults;
}