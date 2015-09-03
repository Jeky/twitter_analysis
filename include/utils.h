#ifndef _UTILS_H_
#define _UTILS_H_

#include <fstream>
#include <iostream>
#include <unordered_set>
#include <utility>
#include <algorithm>
#include <array>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>

#include "utils/log.h"
#include "utils/io.h"

using namespace std;

static const string PATH = string("/Users/jeky/data/thesis/ctweets/");
static const string SPAMMER_TWEET_PATH = PATH + string("spammer-tweets/");
static const string NON_SPAMMER_TWEET_PATH = PATH + string("non-spammer-tweets/");
static const string SAMPLED_NON_SPAMMER_ID_LIST = PATH + string("sampled-non-spammer-id.txt");
static const string NON_SPAMMER_ID_LIST = PATH + string("non-spammer-id.txt");
static const string SPAMMER_ID_LIST = PATH + string("spammer-id.txt");

static const string SPAMMER_DATA_PATH = PATH + string("spammers.obj");
static const string NON_SPAMMER_DATA_PATH = PATH + string("non-spammers.obj");

static const string SPAMMER_TOKEN_COUNTER = PATH + string("spammer-token-counter.obj");
static const string NON_SPAMMER_TOKEN_COUNTER = PATH + string("non-spammer-token-counter.obj");

static const int SAMPLE_TWEET_SIZE = 61;

static const double SPAMMER_VALUE = 1.0;
static const double NON_SPAMMER_VALUE = 0.0;


template<typename K>
void mapAdd(unordered_map<K, double> &m, const K &key, double value) {
    if (m.find(key) == m.end()) {
        m[key] = 0.0;
    }
    m[key] += value;
}


template<typename T>
class Counter {
public:
    void count(const T &t, int number) {
        if (counterMap.find(t) == counterMap.end()) {
            counterMap[t] = 0;
        }
        counterMap[t] += number;
    }

    void count(const T &t) {
        count(t, 1);
    }

    void count(const vector<T> &v) {
        for (auto &t : v) {
            count(t);
        }
    }

    void count(const vector<T> *vp) {
        for (auto &t : *vp) {
            count(t);
        }
    }

    int &operator[](const T &t) {
        if (counterMap.find(t) == counterMap.end()) {
            counterMap[t] = 0;
        }
        return counterMap[t];
    }

    vector<pair<T, int>> *getTop(int n) {
        vector<pair<T, int>> *v = getTop();
        v->erase(v->begin() + n, v->end());
        return v;
    }

    vector<pair<T, int>> *getTop() {
        vector<pair<T, int>> *v = new vector<pair<T, int>>();

        for (auto &kv : counterMap) {
            v->push_back(make_pair(kv.first, kv.second));
        }

        sort(v->begin(), v->end(), [](const pair<string, double> &left, const pair<string, double> &right) {
            return left.second > right.second;
        });

        return v;
    }

    int size(){
        return counterMap.size();
    }

    unordered_set<T> *getKeySet(){
        unordered_set<T> *keySet = new unordered_set<T>();
        keySet->reserve(counterMap.size());
        for(auto &kv : counterMap){
            keySet->insert(kv.first);
        }

        return keySet;
    }

    template<typename Archive>
    void serialize(Archive &ar) {
        ar(counterMap);
    }
private:
    unordered_map<T, int> counterMap;
};

#endif
