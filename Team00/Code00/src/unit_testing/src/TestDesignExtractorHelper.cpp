#include "DesignExtractorHelper.h"
#include "TestFrontEndUtils.h"

#include "catch.hpp"
#include <unordered_set>
#include <iostream>

using namespace std;

template<typename T>
void normalize(Direct<T>& edges) {
    for (auto& x: edges) {
        sort(x.second.begin(), x.second.end());
    }
}

template<typename K, typename V>
void normalize(Ownership<K, V> edges) {
    for (auto& x: edges) {
        sort(x.second.begin(), x.second.end());
    }
}

TEST_CASE("Test extract vertices") {
    Indirect<string> edges = {
        {"Tuan", {"dep", "trai"}},
        {"Quang", {}}
    };

    auto vertices = extractVertices(edges);
    unordered_set<string> answer = {"Tuan", "Quang"};

    REQUIRE(vertices == answer);
}

TEST_CASE("Extract stars simple") {
    Direct<int> edges = {
        {
            1,
            {2, 3}
        },
        {
            2,
            {3, 4}
        },
        {
            3,
            {4}
        }
    };
    //1 -> 2
    //1 -> 3
    //2 -> 3
    //2 -> 4
    //3 -> 4
    auto stars = extractStars<int> (edges);
    
    Indirect<int> answer;
    answer[1] = {2, 3, 4};
    answer[2] = {3, 4};
    answer[3] = {4};

    normalize(answer);
    normalize(stars);

    REQUIRE(compareRelationships(answer, stars));
}