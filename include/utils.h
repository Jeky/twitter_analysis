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
static const string SPAMMER_TWEET_PATH =
    PATH + string("spammer-tweets/");
static const string NON_SPAMMER_TWEET_PATH =
    PATH + string("non-spammer-tweets/");
static const string SAMPLED_NON_SPAMMER_ID_LIST =
    PATH + string("sampled-non-spammer-id.txt");
static const string NON_SPAMMER_ID_LIST =
    PATH + string("non-spammer-id.txt");
static const string SPAMMER_ID_LIST = PATH + string("spammer-id.txt");

static const string SPAMMER_DATA_PATH = PATH + string("spammers.obj");
static const string NON_SPAMMER_DATA_PATH =
    PATH + string("non-spammers.obj");

static const string SPAMMER_TOKEN_COUNTER =
    PATH + string("spammer-token-counter.obj");
static const string NON_SPAMMER_TOKEN_COUNTER =
    PATH + string("non-spammer-token-counter.obj");

static const string SPAMMER_TOKEN_FREQ =
    PATH + string("spammer-token-frequency.txt");
static const string NON_SPAMMER_TOKEN_FREQ =
    PATH + string("non-spammer-token-frequency.txt");
static const string ALL_TOKEN_FREQ =
    PATH + string("all-token-frequency.txt");

static const int SAMPLE_TWEET_SIZE = 61;

static const double SPAMMER_VALUE = 1.0;
static const double NON_SPAMMER_VALUE = 0.0;

/*
#define LOAD_OR_COLLECT(T, NAME, COLLECT_FUN) \
        T *NAME(const string &dataPath, const string &path){\
            ifstream infile(dataPath);\
            if (!infile.good()) { \
                LOG("Cannot Find ", dataPath, ". Loading File from ",
path);\
                infile.close();\
                T *data = new T();\
                saveObject(data, dataPath);\
                return data;\
            } else {\
                LOG("Loading Data from ", dataPath);\
                infile.close();\
                return loadObject<T>(dataPath);\
            }\
        }
*/


struct hashString {
    size_t operator()(const string &s) const {
        size_t hash = 0;
        for(size_t i = 0; i < s.length(); i++){
            hash = hash * 101  +  s[i];
        }
        return hash;
    }
};


template <typename K>
void mapAdd(unordered_map<K, double> &m, const K &key, double value) {
    if (m.find(key) == m.end()) {
        m[key] = 0.0;
    }
    m[key] += value;
}

template <typename T>
unordered_set<T> *setIntersection(unordered_set<T> *s1,
                                  unordered_set<T> *s2) {
    unordered_set<T> *r = new unordered_set<T>();
    unordered_set<T> *sp = s1->size() < s2->size() ? s1 : s2;
    unordered_set<T> *sc = s1->size() < s2->size() ? s2 : s1;
    for (auto &item : *sp) {
        if (sc->find(item) != sc->end()) {
            r->insert(item);
        }
    }
    return r;
}

template <typename T>
unordered_set<T> *setUnion(unordered_set<T> *s1,
                           unordered_set<T> *s2) {
    unordered_set<T> *r = new unordered_set<T>();
    for (auto &item : *s1) {
        r->insert(item);
    }
    for (auto &item : *s2) {
        r->insert(item);
    }
    return r;
}

template <typename K, typename V>
vector<pair<K, V>> *mapToVector(unordered_map<K, V> *map) {
    vector<pair<K, V>> *v = new vector<pair<K, V>>();

    for (auto &kv : *map) {
        v->push_back(make_pair(kv.first, kv.second));
    }

    return v;
}

template <typename T> class Counter {
  public:
    void count(const T &t, int number) {
        if (counterMap.find(t) == counterMap.end()) {
            counterMap[t] = 0;
        }
        counterMap[t] += number;
    }

    void count(const T &t) { count(t, 1); }

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
        vector<pair<T, int>> *v = mapToVector(&counterMap);

        sort(v->begin(), v->end(),
             [](const pair<string, double> &left,
                const pair<string, double> &right) {
            return left.second > right.second;
        });

        return v;
    }

    int size() { return counterMap.size(); }

    unordered_set<T> *getKeySet() {
        unordered_set<T> *keySet = new unordered_set<T>();
        keySet->reserve(counterMap.size());
        for (auto &kv : counterMap) {
            keySet->insert(kv.first);
        }

        return keySet;
    }

    void saveFrequency(const string &path) {
        unordered_map<int, int> freq;
        for (auto &kv : counterMap) {
            if (freq.find(kv.second) == freq.end()) {
                freq[kv.second] = 0;
            }
            freq[kv.second]++;
        }

        vector<pair<int, int>> *v = mapToVector(&freq);

        sort(v->begin(), v->end(), [](const pair<int, int> &left,
                                      const pair<int, int> &right) {
            return left.second > right.second;
        });

        writeFile(path, [&](ofstream &out) {
            for (auto &i : *v) {
                out << i.first << "\t" << i.second << endl;
            }
        });

        delete v;
    }

    void addCounter(const Counter<T> &c) {
        for (auto &kv : c.counterMap) {
            if (counterMap.find(kv.first) != counterMap.end()) {
                counterMap[kv.first] += kv.second;
            } else {
                counterMap[kv.first] = kv.second;
            }
        }
    }

    Counter<T> &operator+=(const Counter<T> &c) {
        addCounter(c);
        return *this;
    }

    template <typename Archive> void serialize(Archive &ar) {
        ar(counterMap);
    }

  private:
    unordered_map<T, int> counterMap;
};

#endif
