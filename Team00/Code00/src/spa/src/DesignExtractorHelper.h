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

	function<void(T)> dfs = [&](T u) {
		vector<T> &answer = results[u];
		for (auto& other: edges.at(u)) {
			if (!was[other]) {
				was[other] = true;
				dfs(other);
			}
			for (auto& v: results[other]) {
				answer.push_back(v);
			}
			answer.push_back(other);
		}
		sort(answer.begin(), answer.end());
		answer.resize(unique(answer.begin(), answer.end()) - answer.begin());
	};

	unordered_set<T> allVertices;
	for (auto& x: edges) {
		allVertices.insert(x.first);
	}

	for (auto& u: allVertices) if (!was[u]) {
		was[u] = true;
		dfs(u);
	}

	return results;
}

template<typename K, typename V>
Ownership<K, V> extractOwnerships(
	const Indirect<K> & indirectRelationships,
	const Ownership<K, V> & directOwnerships) {
		
	unordered_map<K, vector<V> > answer;
	unordered_set<K> allVertices;
	for (auto &x: indirectRelationships) {
		allVertices.insert(x.first);
	}
	unordered_map<K, bool> was;
	function<void(K)> dfs = [&](const K& u){
		vector<V> &currentAnswer = answer[u];

		for (auto &other: directOwnerships[u]) {
			if (!was[other]) {
				was[other] = true;
				dfs(other);
			}
			copy(answer[other].begin(), answer[other].end(), std::back_inserter(currentAnswer));
		}

		sort(currentAnswer.begin(), currentAnswer.end());
		currentAnswer.resize(unique(currentAnswer.begin(), currentAnswer.end()) - currentAnswer.begin());
	};

	for (auto& u: allVertices) if (!was[u]) {
		was[u] = true;
		dfs(u);
	}

	return answer;
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