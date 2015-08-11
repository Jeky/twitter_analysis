#ifndef _UTILS_H_
#define _UTILS_H_

#include <fstream>
#include <iostream>
#include <unordered_set>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>

#include "log.h"
#include "io.h"

#define Map unordered_map
#define Vector vector
#define String string
#define Set unordered_set

using namespace std;

static const String PATH = String("/Users/jeky/data/thesis/ctweets/");

template <typename T>
void FOREACH(Vector<T> container, function<void (int, T&)> elementHandler){
    for(int i = 0; i < container.size(); i++){
        elementHandler(i, container[i]);
    }
}


template <typename K, typename V>
void FOREACH(Map<K, V> container, function<void (K&, V&)> elementHandler){
    for (auto &kv : container) {
        K k = kv.first;
        V v = kv.second;
        elementHandler(k, v);
    }
}


template <typename K>
void mapAdd(Map<K, double> map, K &key, double value){
    if(map.find(key) == map.end()){
        map[key] = 0.0;
    }

    map[key] += value;
}

#endif
