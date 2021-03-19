#include "TestResultsTableUtil.h"

void TestResultsTableUtil::checkTable(vector<vector<string>> actualTable, vector<vector<string>> expectedTable) {
	REQUIRE(actualTable.size() == expectedTable.size());

	for (int i = 0; i < actualTable.size(); i++) {
		vector<string> actualRow = actualTable.at(i);
		vector<string> expectedRow = expectedTable.at(i);

		REQUIRE(actualRow.size() == expectedRow.size());

		for (int j = 0; j < actualRow.size(); j++) {
			REQUIRE(actualRow.at(j) == expectedRow.at(j));
		}
	}
}

void TestResultsTableUtil::checkMap(unordered_map<string, int> actualMap, unordered_map<string, int> expectedMap) {
	REQUIRE(actualMap.size() == expectedMap.size());

	for (unordered_map<string, int>::iterator it = expectedMap.begin(); it != expectedMap.end(); it++) {
		string expectedKey = it->first;
		int expectedValue = it->second;

		unordered_map<string, int>::iterator actualIt = actualMap.find(expectedKey);
		REQUIRE(actualIt != actualMap.end());
		REQUIRE(actualIt->second == expectedValue);
		actualMap.erase(expectedKey);
	}

	REQUIRE(actualMap.size() == 0);
}

void  TestResultsTableUtil::checkList(list<string> actualList, list<string> expectedList) {
	REQUIRE(actualList.size() == expectedList.size());

	list<string>::iterator actualListIt = actualList.begin();
	list<string>::iterator expectedListIt = expectedList.begin();

	while (actualListIt != actualList.end() || expectedListIt != expectedList.end()) {
		REQUIRE(*actualListIt == *expectedListIt);
		actualListIt++;
		expectedListIt++;
	}
}