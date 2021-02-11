#include "ResultsTable.h"

ResultsTable::ResultsTable() {
	this->aSynonymIndex = unordered_map<string, int>();
	this->aValues = vector<vector<string>>();
}

unordered_map<string, int> ResultsTable::getSynonymIndexMap() {
	return this->aSynonymIndex;
}

vector<vector<string>> ResultsTable::getTableValues() {
	return this->aValues;
}

set<string> ResultsTable::getSynonyms() {
	set<string> synonyms;
	for (unordered_map<string, int>::iterator it = this->aSynonymIndex.begin(); it != this->aSynonymIndex.end(); it++) {
		synonyms.insert(it->first);
	}
	return synonyms;
}
bool ResultsTable::isNoResult() {
	return this->noResult;
}


void ResultsTable::setIsNoResult() {
	this->noResult = true;
}

void ResultsTable::setTable(unordered_map<string, int> synonymIndex, vector<vector<string>> values) {
	this->aSynonymIndex = synonymIndex;
	this->aValues = values;
}

bool ResultsTable::isTableEmpty() {
	return this->aValues.size();
}

void ResultsTable::populate(unordered_map<string, set<string>> PKBResult, vector<string> synonyms) {
	if (synonyms.size() == 1) {
		string synonym = synonyms.at(0);
		this->aSynonymIndex.insert({ synonym, 0 });
		for (unordered_map<string, set<string>>::iterator it = PKBResult.begin(); it != PKBResult.end(); it++) {
			set<string> values = it->second;
			for (set<string>::iterator setIt = values.begin(); setIt != values.end(); setIt++) {
				vector<string> row = { *setIt };
				this->aValues.push_back(row);
			}
		}
		return;
	}

	if (synonyms.size() == 2) {
		string leftSynonym = synonyms.at(0);
		string rightSynonym = synonyms.at(1);
		this->aSynonymIndex.insert({ leftSynonym, 0 });
		this->aSynonymIndex.insert({ rightSynonym, 1 });
		for (unordered_map<string, set<string>>::iterator it = PKBResult.begin(); it != PKBResult.end(); it++) {
			string leftSynonymValue = it->first;
			set<string> values = it->second;

			for (set<string>::iterator setIt = values.begin(); setIt != values.end(); setIt++) {
				string rightSynonymValue = *setIt;
				vector<string> row = { leftSynonymValue, rightSynonymValue };
				this->aValues.push_back(row);
			}
		}
		return;
	}
}