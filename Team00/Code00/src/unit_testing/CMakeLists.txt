file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_executable(unit_testing ${srcs} "src/main.cpp" "src/TestQueryParser.cpp" "src/TokenizerStub.h" "src/TokenizerStub.cpp" "src/TokenizerTester.cpp" "")


target_link_libraries(unit_testing spa)

