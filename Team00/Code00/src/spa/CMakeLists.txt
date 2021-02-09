file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_library(spa ${srcs} ${headers} "src/QueryInput.h" "src/QueryInput.cpp" "src/QueryInputType.h" "src/Any.h" "src/Any.cpp" "src/Declaration.h" "src/Declaration.cpp" "src/EntityType.h" "src/Expression.h" "src/Expression.cpp" "src/Indent.h" "src/Indent.cpp" "src/StmtNum.h" "src/StmtNum.cpp" "src/SelectClause.h" "src/SelectClause.cpp" "src/RelationshipType.h" "src/RelationshipClause.h" "src/RelationshipClause.cpp" "src/PatternClause.h" "src/PatternClause.cpp" "src/Query.h" "src/Query.cpp" "src/QueryEvaluator.h" "src/QueryEvaluator.cpp" "src/ResultUtil.h")
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


