#include "PKBInterface.h"
#include <memory>
#include <unordered_map>
#include <set>
#include <vector>


using namespace std;

class PKBStub : public PKBInterface {
public:

	vector<unordered_map<string, set<string>>> mapResults;
	int mapResultsCount;

	vector<set<string>> setResults;
	int setResultsCount;

	vector<bool> boolReturnValues;
	int boolCount;

	// original api being stubbed

	set<string> getEntities(const EntityType& type);

	bool getBooleanResultOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);

	unordered_map<string, set<string>> getMapResultsOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);

	set<string> getSetResultsOfRS(const RelationshipType& type,
		shared_ptr<QueryInput> input1, shared_ptr<QueryInput> input2);

	unordered_map<string, set<string>> getMapResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression);

	set<string> getSetResultsOfAssignPattern(
		shared_ptr<QueryInput> input, Expression& expression);

	unordered_map<string, set<string>> getMapResultsOfContainerPattern(
		const EntityType& type, shared_ptr<QueryInput> input);

	set<string> getSetResultsOfContainerPattern(
		const EntityType& type, shared_ptr<QueryInput> input);

	// Methods to add the return values for each different test case
	void addBooleanResult(bool value);

	void addMapResult(unordered_map<string, set<string>> value);

	void addSetResult(set<string> value);

	// Other methods
	PKBStub();
	void resetCounts();

};