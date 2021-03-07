#ifndef __PARSERHELPER__H__
#define __PARSERHELPER__H__
#include <vector>
#include <map>
#include <queue>

using namespace std;

bool checkCyclicCalls(map<string, vector<string> >& edges) {
    map<string, int> degree;

    for (auto& x: edges) {
        degree[x.first] += 0;
        for (auto& y: x.second) {
            degree[y]++;
        }
    }

    int needed = degree.size();
    queue<string> qu;
    for (auto& x: degree) {
        if (x.second == 0) {
            qu.push(x.first);
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

#endif  //!__PARSERHELPER__H__