// #include "ResultUtil.h"
// #include "TestResultsTableUtil.h"
// #include "catch.hpp"

// TEST_CASE("Getting common synonyms between a vector and a list") {
// 	string stmtSynonym = "s";
// 	string assignSynonym = "a";
// 	string varSynonym = "v";
// 	string whileSynonym = "w";
// 	string ifSynonym = "if";
	

// 	SECTION("Getting one common synonym from size 2 vector and non empty list") {
// 		vector<string> PKBResultSynonyms = { whileSynonym, stmtSynonym };
// 		set<string> currentResultSynonyms = { whileSynonym, ifSynonym, varSynonym, assignSynonym };

// 		set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
// 		REQUIRE(commonSynonyms.size() == 1);
// 		REQUIRE(commonSynonyms.find(whileSynonym) != commonSynonyms.end());
// 	}

// 	SECTION("Getting two common synonym from size 2 vector and non empty list") {
// 		vector<string> PKBResultSynonyms = { whileSynonym, assignSynonym };
// 		set<string> currentResultSynonyms = { whileSynonym, ifSynonym, varSynonym, assignSynonym };

// 		set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
// 		REQUIRE(commonSynonyms.size() == 2);
// 		REQUIRE(commonSynonyms.find(whileSynonym) != commonSynonyms.end());
// 		REQUIRE(commonSynonyms.find(assignSynonym) != commonSynonyms.end());
// 	}

// 	SECTION("Getting one common synonyms from size 1 vector and non empty list") {
// 		vector<string> PKBResultSynonyms = { stmtSynonym };
// 		set<string> currentResultSynonyms = { whileSynonym, ifSynonym, stmtSynonym, assignSynonym };

// 		set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
// 		REQUIRE(commonSynonyms.size() == 1);
// 		REQUIRE(commonSynonyms.find(stmtSynonym) != commonSynonyms.end());
// 	}

// 	SECTION("Getting no common synonyms from size 1 vector and non empty list") {
// 		vector<string> PKBResultSynonyms = { varSynonym };
// 		set<string> currentResultSynonyms = { whileSynonym, ifSynonym, stmtSynonym, assignSynonym };

// 		set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
// 		REQUIRE(commonSynonyms.size() == 0);
// 	}

// 	SECTION("Getting no common synonyms from size 2 vector and non empty list") {
// 		vector<string> PKBResultSynonyms = { varSynonym, whileSynonym };
// 		set<string> currentResultSynonyms = { ifSynonym, stmtSynonym, assignSynonym };

// 		set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
// 		REQUIRE(commonSynonyms.size() == 0);
// 	}

// 	SECTION("Getting no common synonyms from empty vector and list") {
// 		vector<string> PKBResultSynonyms = {};
// 		set<string> currentResultSynonyms = {};

// 		set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
// 		REQUIRE(commonSynonyms.size() == 0);
// 	}

// 	SECTION("Getting no common synonyms from non empty vector and empty list") {
// 		vector<string> PKBResultSynonyms = { varSynonym, whileSynonym };
// 		set<string> currentResultSynonyms = {};

// 		set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
// 		REQUIRE(commonSynonyms.size() == 0);
// 	}

// 	SECTION("Getting no common synonyms from empty vector and non empty list") {
// 		vector<string> PKBResultSynonyms = {};
// 		set<string> currentResultSynonyms = { ifSynonym, stmtSynonym, assignSynonym };

// 		set<string> commonSynonyms = ResultUtil::getCommonSynonyms(PKBResultSynonyms, currentResultSynonyms);
// 		REQUIRE(commonSynonyms.size() == 0);
// 	}
// }

// TEST_CASE("Getting cartesian product from two results") {
// 	string stmtSynonym = "s";
// 	string assignSynonym = "a";
// 	string varSynonym = "v";
// 	string whileSynonym = "w";

// 	SECTION("Getting cartesian product from PKB Result with 2 synonym values") {
// 		unordered_map<string, set<string>> pkbResult = { {"1", {"2", "3"}}, {"4", {"5"}} };
// 		vector<vector<string>> table = { {"a", "c"}, {"b", "d"} };
// 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {whileSynonym, 1} };
// 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
// 		resultsTable->setTable(indexMap, table);

// 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getCartesianProduct(pkbResult, { stmtSynonym, assignSynonym }, resultsTable);
		
		
// 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
// 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

// 		vector<vector<string>> expectedTable = { {"a", "c", "1", "2"}, {"b", "d", "1", "2"}, {"a", "c", "1", "3"},
// 			{"b", "d", "1", "3"},{"a", "c", "4", "5"}, {"b", "d", "4", "5"} };
// 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {whileSynonym, 1},
// 			{stmtSynonym, 2}, {assignSynonym, 3} };

// 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
// 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
// 	}

// 	SECTION("Getting cartesian product from PKB results with one synonym values") {
// 		unordered_map<string, set<string>> pkbResult = { { "dummy", {"1", "2", "3", "4"} } };
// 		vector<vector<string>> table = { {"a", "c"}, {"b", "d"} };
// 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {whileSynonym, 1} };
// 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
// 		resultsTable->setTable(indexMap, table);

// 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getCartesianProduct(pkbResult, { stmtSynonym }, resultsTable);


// 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
// 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

// 		vector<vector<string>> expectedTable = { {"a", "c", "1"}, {"a", "c", "2"}, {"a", "c", "3"}, {"a", "c", "4"},
// 			{"b", "d", "1"}, {"b", "d", "2"}, {"b", "d", "3"}, {"b", "d", "4"} };
// 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {whileSynonym, 1},
// 			{stmtSynonym, 2} };

// 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
// 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
// 	}
// }

// TEST_CASE("Getting natural join from two results") {
// 	string stmtSynonym = "s";
// 	string assignSynonym = "a";
// 	string varSynonym = "v";
// 	string whileSynonym = "w";
// 	string readSynonym = "r";

// 	SECTION("Getting natural join from PKB result with 1 synonym") {
// 		unordered_map<string, set<string>> pkbResult = { { "dummy", {"1", "2", "3", "4"} } };
// 		vector<vector<string>> table = { {"a", "2", "c"}, {"b", "10", "d"}, {"e", "2", "f"}, {"b", "4", "d"} };
// 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2} };
// 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
// 		resultsTable->setTable(indexMap, table);

// 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getNaturalJoin(pkbResult, { stmtSynonym }, resultsTable, { stmtSynonym });


// 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
// 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

// 		vector<vector<string>> expectedTable = { {"a", "2", "c"}, {"e", "2", "f"}, {"b", "4", "d"} };
// 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2} };

// 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
// 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
// 	}

// 	SECTION("Getting natural join from PKB result with 2 common synonyms") {
// 		unordered_map<string, set<string>> pkbResult = { {"1", {"2", "3", "12", "10"}}, {"4", {"5", "2", "23", "12"}} };
// 		vector<vector<string>> table = { {"a", "1", "c", "10"}, {"b", "10", "d", "2"}, {"e", "4", "f", "2"}, {"b", "4", "d", "12"},
// 			{"b", "10", "a", "222"}, {"ee", "1", "ff", "2"} };
// 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 } };
// 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
// 		resultsTable->setTable(indexMap, table);

// 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getNaturalJoin(pkbResult, { stmtSynonym, assignSynonym }, resultsTable,
// 			{ stmtSynonym, assignSynonym });


// 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
// 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

// 		vector<vector<string>> expectedTable = { {"a", "1", "c", "10"}, {"ee", "1", "ff", "2"},  {"b", "4", "d", "12"},
// 			{"e", "4", "f", "2"} };
// 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 } };
		
// 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
// 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
// 	}

// 	SECTION("Getting natural join from PKB result with common left synonym and uncommon right synonym") {
// 		unordered_map<string, set<string>> pkbResult = { {"1", {"2", "10"}}, {"4", {"5", "2", "23"}}, {"10", {"5", "2", "23"}} };
// 		vector<vector<string>> table = { {"a", "1", "c", "10"}, {"b", "10", "d", "2"}, {"e", "14", "f", "2"}, {"b", "41", "d", "12"},
// 			{"b", "10", "a", "222"}, {"ee", "1", "ff", "2"} };
// 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 } };
// 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
// 		resultsTable->setTable(indexMap, table);

// 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getNaturalJoin(pkbResult, { stmtSynonym, readSynonym }, resultsTable,
// 			{ stmtSynonym });


// 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
// 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

// 		vector<vector<string>> expectedTable = { {"b", "10", "d", "2", "2"}, {"b", "10", "d", "2", "23"}, {"b", "10", "d", "2", "5"},
// 			{"b", "10", "a", "222", "2"}, {"b", "10", "a", "222", "23"}, {"b", "10", "a", "222", "5"},
// 			{"a", "1", "c", "10", "10"}, {"a", "1", "c", "10", "2"},
// 			{"ee", "1", "ff", "2", "10"}, {"ee", "1", "ff", "2", "2"} };
// 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {stmtSynonym, 1}, {whileSynonym, 2}, {assignSynonym, 3 }, 
// 			{readSynonym, 4} };
		
// 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
// 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
// 	}
	
// 	SECTION("Getting natural join from PKB result with uncommon left synonym and common right synonym") {
// 		unordered_map<string, set<string>> pkbResult = { {"1", {"2", "10"}}, {"4", {"5", "21", "23"}}, {"10", {"15", "222", "2"}} };
// 		vector<vector<string>> table = { {"a", "1", "c", "10"}, {"b", "10", "d", "2"}, {"e", "14", "f", "2"}, {"b", "41", "d", "5"},
// 			{"b", "10", "a", "222"}, {"ee", "1", "ff", "2"} };
// 		unordered_map<string, int> indexMap = { {varSynonym, 0}, {assignSynonym, 1}, {whileSynonym, 2}, {readSynonym, 3 } };
// 		shared_ptr<ResultsTable> resultsTable = make_shared<ResultsTable>();
// 		resultsTable->setTable(indexMap, table);

// 		shared_ptr<ResultsTable> cartProdResult = ResultUtil::getNaturalJoin(pkbResult, { stmtSynonym, readSynonym }, resultsTable,
// 			{ readSynonym });


// 		vector<vector<string>> cartProdTable = cartProdResult->getTableValues();
// 		unordered_map<string, int> cartProdIndexMap = cartProdResult->getSynonymIndexMap();

// 		vector<vector<string>> expectedTable = { {"b", "10", "d", "2", "10"}, {"e", "14", "f", "2", "10"}, {"ee", "1", "ff", "2", "10"},
// 			{"b", "10", "a", "222", "10"} , 
// 			{"a", "1", "c", "10", "1"},  {"b", "10", "d", "2", "1"}, {"e", "14", "f", "2", "1"}, {"ee", "1", "ff", "2", "1"},
// 			{"b", "41", "d", "5", "4"} };
// 		unordered_map<string, int> expectedMap = { {varSynonym, 0}, {assignSynonym, 1}, {whileSynonym, 2}, {readSynonym, 3 },
// 			{stmtSynonym, 4} };
		
// 		TestResultsTableUtil::checkTable(cartProdTable, expectedTable);
// 		TestResultsTableUtil::checkMap(cartProdIndexMap, expectedMap);
// 	}
// }
