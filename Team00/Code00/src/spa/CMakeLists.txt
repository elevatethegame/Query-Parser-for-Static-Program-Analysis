file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_library(spa ${srcs} ${headers} "src/InputStream.h" "src/Tokenizer.h" "src/Token.h" "src/QueryParser.h" "src/Tokenizer.cpp" "src/Token.cpp" "src/QueryParser.cpp" "src/InputStream.cpp" "src/TokenTypes.h" "src/QueryInput.h" "src/QueryInput.cpp"  "src/Any.h" "src/Any.cpp" "src/Declaration.h" "src/Declaration.cpp"  "src/Expression.h" "src/Expression.cpp" "src/Ident.h" "src/Ident.cpp" "src/StmtNum.h" "src/StmtNum.cpp" "src/SelectClause.h" "src/SelectClause.cpp" "src/RelationshipClause.h" "src/RelationshipClause.cpp" "src/PatternClause.h" "src/PatternClause.cpp" "src/Query.h" "src/Query.cpp" "src/QueryInterface.h" "src/ExpressionType.h")

# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)





