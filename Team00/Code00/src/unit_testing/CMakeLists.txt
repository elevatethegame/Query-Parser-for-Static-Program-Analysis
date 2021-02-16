file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_executable(unit_testing ${srcs} "src/TokenizerTester.cpp" "src/ParserTokenizerIntegrationTester.cpp" "src/TestQuery.cpp" "src/TestQueryEvaluator.cpp" "src/TestResultUtil.cpp" "src/TestResultsTable.cpp" "src/TestResultsTableUtil.h" "src/TestResultsTableUtil.cpp" "src/TestResultsProjector.cpp" "src/PKBStub.h" "src/PKBStub.cpp")


target_link_libraries(unit_testing spa)

