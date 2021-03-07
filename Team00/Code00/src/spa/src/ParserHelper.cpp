#include "ParserHelper.h"

using namespace std;

bool checkCyclicCalls(map<string, vector<string> > edges) {
    map<string, int> degree;

    for (auto& x: edges) {
        degree[x] += 0;
        for (auto& y: x.second) {
            degree[y]++;
        }
    }

    int needed = degree.size();
    queue<string> qu(0);
    for (auto& x: degree) {
        if (x.second == 0) {
            qu.push(x.second);
        }
    }

    int collected = 0;
    while (qu.size() > 0) {
        auto u = qu.front();
        ++collected;
        for (auto& v: edges[u]) {
            degree[v]--;
            if (degree[v] == 0) {
                qu.push(v);
            }
        }
    }
    
    return collected == needed;
}