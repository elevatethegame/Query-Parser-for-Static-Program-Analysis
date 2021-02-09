#include "ResultUtil.h"
#include "catch.hpp"

TEST_CASE("Converting PKB result", "[Two synonyms]") {
	string stmtSynonym = "s";
	string assignSynonym = "a";

	SECTION("Converting non empty pkb result success", "[One result row]") {
		set<string> setInput = { "1", "2", "3" };
		unordered_map<string, set<string>> pkbResult;
		string firstSynonymValue = "4";
		pkbResult.insert({ firstSynonymValue, setInput });
		vector<unordered_map<string, string>> convertedResult = ResultUtil::convertPKBResult(pkbResult, stmtSynonym, assignSynonym);
		for (auto map : convertedResult) {
			for (auto result : map) {
				bool isCorrectSynonym = result.first == stmtSynonym || result.first == assignSynonym;
				REQUIRE(isCorrectSynonym);

				if (result.first == stmtSynonym) {
					REQUIRE(result.second == firstSynonymValue);
				}

				if (result.first == assignSynonym) {
					REQUIRE(setInput.find(result.second) != setInput.end());
					setInput.erase(result.second);
				}

			}
		}
		REQUIRE(setInput.size() == 0);
	}

	SECTION("Converting non empty pkb result success", "[Multiple result rows]") {
		set<string> firstSetInput = { "11", "12", "13" };
		set<string> secondSetInput = { "21", "22", "23" };
		unordered_map<string, set<string>> pkbResult;
		string stmtSynonymfirstValue = "1";
		string stmtSynonymSecondValue = "2";
		pkbResult.insert({ stmtSynonymfirstValue, firstSetInput });
		pkbResult.insert({ stmtSynonymSecondValue, secondSetInput });
		set<string> synonymValues = { stmtSynonymfirstValue, stmtSynonymSecondValue };

		vector<unordered_map<string, string>> convertedResult = ResultUtil::convertPKBResult(pkbResult, stmtSynonym, assignSynonym);
		for (auto map : convertedResult) {
			REQUIRE(map.find(stmtSynonym) != map.end());
			string stmtSynonymValue = map.find(stmtSynonym)->second;
			REQUIRE(synonymValues.find(stmtSynonymValue) != synonymValues.end());
			synonymValues.erase(stmtSynonymValue);

			set<string> setInput;
			if (stmtSynonymValue == stmtSynonymfirstValue) {
				setInput = firstSetInput;
			}
			else {
				setInput = secondSetInput;
			}

			REQUIRE(map.find(assignSynonym) != map.end());
			string assignSynonymValue = map.find(assignSynonym)->second;
			REQUIRE(setInput.find(assignSynonymValue) != setInput.end());
			setInput.erase(assignSynonymValue);
		}
		REQUIRE(firstSetInput.size() == 0);
		REQUIRE(secondSetInput.size() == 0);
		REQUIRE(synonymValues.size() == 0);
	}
}