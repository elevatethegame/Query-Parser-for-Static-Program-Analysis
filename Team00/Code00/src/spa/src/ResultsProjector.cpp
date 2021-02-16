#include "ResultsProjector.h"

void ResultsProjector::projectResults(shared_ptr<ResultsTable> evaluatedResults, shared_ptr<SelectClause> selectClause, shared_ptr<PKBInterface> PKB, list<string>& results) {
	if (evaluatedResults->isNoResult()) {
		return;
	}
	
	shared_ptr<Declaration> declaration = selectClause->getDeclaration();
	set<string> synonyms = evaluatedResults->getSynonyms();
	string synonym = declaration->getValue();
	if (synonyms.find(synonym) != synonyms.end()) {
		vector<vector<string>> resultValues = evaluatedResults->getTableValues();
		unordered_map<string, int> synonymIndexMap = evaluatedResults->getSynonymIndexMap();
		int synonymIndex = synonymIndexMap.find(synonym)->second;
		for (vector<vector<string>>::iterator it = resultValues.begin(); it != resultValues.end(); it++) {
			results.push_back(it->at(synonymIndex));
		}
		return;
	}

	set<string> PKBResults = PKB->getEntities(declaration->getEntityType());
	for (set<string>::iterator it = PKBResults.begin(); it != PKBResults.end(); it++) {
		results.push_back(*it);
	}
}
