#include "catch.hpp"

using namespace std;

template <typename Rel>
bool compareRelationships (Rel const &first, Rel const &second) {
    if (first.size() != second.size()) {
        return false;
    }

    for (auto& x: first) {
        if (second.find(x.first) == second.end()) {
            return false;
        }
        if (second.at(x.first) != x.second) {
            return false;
        }
    }

    return true;
}
