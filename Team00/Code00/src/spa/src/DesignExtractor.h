#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <set>

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

	void setCalls(map<string, vector<string> >& edges);

	void insertConstant(string c);

	void setProcName(string name);

	shared_ptr<PKB> extractToPKB();

	vector<string> getUses(int index) const;

	vector<string> getModifies(int index) const;
private:
	int numberOfStatement;

	vector<vector<int>> nexts;	
	vector<vector<int>> revFlows; ///to calculate affect relationship
	vector<vector<int> > parents;
	vector<vector<int> > follows;
	vector<EntityType> types;
	vector<vector<string>> uses;
	vector<vector<string>> modifies;
	map<string, vector<string>> calls;
	vector<vector<Expression>> expressions;
	set<string> constants;
	string procName;

	void insertNext(int id1, int id2);
	void insertRevFlow(int id1, int id2);

	/// Return last statement of each block 
	int buildCFGBlock(int stmt);
	void buildCFG();
};
