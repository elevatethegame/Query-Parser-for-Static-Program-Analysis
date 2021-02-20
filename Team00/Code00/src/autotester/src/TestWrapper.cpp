#include "TestWrapper.h"
#include "QueryParser.h"
#include "ResultsProjector.h"
#include "QueryInterface.h"
#include "QueryEvaluator.h"
#include <Parser.h>
#include <Query.h>

#include "catch.hpp"

#include <string>
#include <vector>
#include <fstream>



// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() { 
  // create any objects here as instance variables of this class
  // as well as any initialization required for your spa program

}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
	// call your parser to do the parsing

	vector<string> codes;
	// Open the File
	std::ifstream in(filename.c_str());
	// Check if object is valid
	if (!in) {
		std::cerr << "Cannot open the File : " << filename << std::endl;
	}

	std::string str;
	// Read the next line from File untill it reaches the end.
	while (std::getline(in, str))
	{
		// Line contains string of length > 0 then save it in vector
		if (str.size() > 0)
			codes.push_back(str);
	}
	//Close The File
	in.close();

	SIMPLETokenStream stream{ codes };

	SIMPLETokenStream secondStream{ stream };
	// while (secondStream.isEmpty() == false) {
	//     cerr << secondStream.getToken().toString() << endl;
	// }

	DesignExtractor extractor;
	Parser parser{ extractor };

	auto error = parser.parseProcedure(stream);
	
	pkb = extractor.extractToPKB();

	pkb->init();
}

// method to evaluating a query
void TestWrapper::evaluate(std::string input, std::list<std::string>& results) {
	// call your evaluator to evaluate the query here
	  // ...code to evaluate query...
	auto query = std::make_shared<Query>();
	QueryParser queryParser = QueryParser{ input, query };
	queryParser.parse();
	// store the answers to the query in the results list (it is initially empty)
	// each result must be a string.
	cout << query->getSelectClause()->getDeclaration()->getValue() << "\n";
	cout << query->getRelationshipClauses().size() << "\n";
	QueryEvaluator queryEvaluator = QueryEvaluator(query, pkb);
	auto evaluatedResults = queryEvaluator.evaluate();
	ResultsProjector::projectResults(evaluatedResults, query->getSelectClause(), pkb, results);

}
