file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_library(spa ${srcs} ${headers} "src/InputStream.h" "src/Tokenizer.h" "src/Token.h" "src/StmtNum.h" "src/RelationshipType.h" "src/QueryParser.h" "src/QueryInputType.h" "src/Ident.h" "src/Expression.h" "src/EntityType.h" "src/Any.h" "src/Declaration.h" "src/Tokenizer.cpp" "src/Token.cpp" "src/StmtNum.cpp" "src/QueryParser.cpp" "src/QueryInput.cpp" "src/InputStream.cpp" "src/Ident.cpp" "src/Expression.cpp" "src/Declaration.cpp" "src/Any.cpp" "src/QueryInput.h" "src/TokenTypes.h" "src/QueryInterface.h")
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


