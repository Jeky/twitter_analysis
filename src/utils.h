#ifndef _UTILS_H_
#define _UTILS_H_

#include <fstream>
#include <iostream>

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

using namespace std;

static const String PATH = String("/Users/jeky/data/thesis/ctweets/");

template <typename T>
void FOREACH(Vector<T> container, function<void (int, T&)> elementHandler){
    for(int i = 0; i < container.size(); i++){
        elementHandler(i, container[i]);
    }
}

template <typename K, typename V>
void FOREACH(Map<K, V> container, function<void (K, V&)> elementHandler){
    for (auto &kv : container) {
        elementHandler(kv.first, kv.second);        
    }
}

#endif
