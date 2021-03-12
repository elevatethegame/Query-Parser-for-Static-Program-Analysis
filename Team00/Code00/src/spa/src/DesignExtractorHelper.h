#ifndef __DESIGNEXTRACTORHELPER__H__
#define __DESIGNEXTRACTORHELPER__H__
#include <unordered_map>
#include <vector>
#include <unordered_set>

using namespace std;

template<typename T>
using Indirect = unordered_map<T, vector<T> >; 

template<typename T>
using Direct = unordered_map<T, vector<T> >;

template<typename K, typename V>
using Ownership = unordered_map<K, vector<V> >;

template<typename T>
unordered_set<T> extractVertices(const Indirect<T>& edges) {
    unordered_set<T> answer;
    for (auto& x: edges) answer.insert(x.first);
    return answer;
}


template<typename T>
Indirect<T> extractStars(const Direct<T>& edges) {
    Indirect<T> results;
    unordered_map<T, bool> was;
    unordered_set<T> collecting;
    unordered_set<T> allVertices = extractVertices(edges);
    

    function<void(T, T)> dfs = [&](T u, T origin) {
        results[origin].push_back(u);
        for (auto &nxt: edges.at(u)) {
            collecting.insert(nxt);
            if (!was[nxt]) {
                was[nxt] = true;
                dfs(nxt, origin);
            }
        }
    };

    for (auto u: allVertices) {
        was.clear();
        collecting.clear();
        dfs(u, u);
        for (auto& v: collecting) {
            results[u].push_back(v);
        }
    }

    return results;
}


template<typename K, typename V>
Ownership<K, V> extractOwnerships(
	const Indirect<K> & indirectRelationships,
	const Ownership<K, V> & directOwnerships) {

    Ownership<K, V> results;
    unordered_set<K> allVertices = extractVertices(indirectRelationships);
    
    for (auto& u: allVertices) {
        unordered_set<V> collecting;
        for (auto& v: indirectRelationships.at(u)) {
            for (auto& w: directOwnerships.at(v)) {
                collecting.insert(w);
            }
        }
        copy(collecting.begin(), collecting.end(), std::back_inserter(results.at(u)));
    }

    return results;
}


template<typename A, typename B, typename C>
Ownership<A, C> convolute(const Ownership<A, B> &f, const Ownership<B, C> &g) {

}


template<typename T, typename K>
unordered_map<T, vector<K> > convertToMapForm(const vector<vector<K>>& vectorForm, int low, int high) {
	unordered_map<T, vector<K> > answer;

	for (int i = low; i <= high; i++) {
		answer[i] = vectorForm[i];
	}

	return answer;
}

#endif  //!__DESIGNEXTRACTORHELPER__H__