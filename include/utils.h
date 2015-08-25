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

#define Map unordered_map
#define Vector vector
#define String string
#define Set unordered_set
#define Pair pair
#define Array array;

using namespace std;

static const String PATH = String("/Users/jeky/data/thesis/ctweets/");


template <typename K>
void mapAdd(Map<K, double> &m, const K &key, double value){
    if(m.find(key) == m.end()){
        m[key] = 0.0;
    }
    m[key] += value;
}

#endif