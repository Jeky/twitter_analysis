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
static const string NON_SPAMMER_DATA_PATH = PATH + string("nonspammers.obj");

static const double SPAMMER_VALUE = 1.0;
static const double NON_SPAMMER_VALUE = 0.0;


template<typename K>
void mapAdd(unordered_map<K, double> &m, const K &key, double value) {
    if (m.find(key) == m.end()) {
        m[key] = 0.0;
    }
    m[key] += value;
}

#endif
