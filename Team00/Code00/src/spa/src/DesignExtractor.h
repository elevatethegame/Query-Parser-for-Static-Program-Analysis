#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;


#include "PKB.h"
#include "EntityType.h"

class DesignExtractor {
public:
	DesignExtractor();

	void increaseNumberOfStatement(EntityType type);

	void insertParent(int parent, int child);

    void insertFollow(int before, int after);

	void insertUses(int id, const string& variable);

	void insertModifies(int id, const string& variable);

	void insertExpression(int id, const Expression& expression);

	void insertConstant(string c);

	void setProcName(string name);

	shared_ptr<PKB> extractToPKB();

	vector<string> getUses(int index) const;

	vector<string> getModifies(int index) const;
private:
	int numberOfStatement;
	vector<vector<int> > parents;
	vector<vector<int> > follows;
	vector<EntityType> types;
	vector<vector<string>> uses;
	vector<vector<string>> modifies;
	vector<vector<Expression>> expressions;
	vector<string> constants;
	string procName;
};
