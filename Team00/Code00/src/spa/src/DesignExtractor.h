#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <set>

using namespace std;


#include "PKB.h"
#include "EntityType.h"
#include "DesignExtractorHelper.h"

class DesignExtractor {
public:
	DesignExtractor();

	void increaseNumberOfStatement(EntityType type);

	void insertParent(int parent, int child);

    void insertFollow(int before, int after);

	void insertUses(int id, const string& variable);

	void insertModifies(int id, const string& variable);

	void insertExpression(int id, const Expression& expression);

	void insertStatementCall(int id, const string &procName);

	void setCalls(Direct<string>& edges);

	void insertConstant(string c);

	void setProcedure(string name, int low, int high);

	void setProcName(string name);

	shared_ptr<PKB> extractToPKB();

	vector<string> getUses(int index) const;

	vector<string> getModifies(int index) const;
private:
	int numberOfStatement;

	vector<vector<int>> nexts;	
	vector<vector<int> > parents;
	vector<vector<int> > follows;
	vector<EntityType> types;
	vector<vector<string>> uses;
	vector<vector<string>> modifies;
	vector<vector<int>> affects;
	unordered_map<string, vector<string>> calls;
	Ownership<string, int> procedures;
	Ownership<int, string> statementCalls;
	vector<vector<Expression> > expressions;
	set<string> constants;
	string procName;

	void insertNext(int id1, int id2);

	/// Return last statement of each block 
	int buildCFGBlock(int stmt);
	void buildCFG();

	void affectDFS(int startStmt, int curStmt, string var, vector<int>& visited);
	void buildDirectAffect(); 

	// Build Indirect relationships
	void buildIndirectRelationships();
};
