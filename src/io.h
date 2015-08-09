#ifndef _UTILS__IO_H_
#define _UTILS__IO_H_

#include <functional>
#include <fstream>

#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>

#include "log.h"

#define LINE_COUNT 10000

using namespace std;

void readFile(string &filename, function<bool (int, string&)> lineHandler);

template <typename T>
void saveObject(T &t, string filename){
    log() << "Start Saving Object to " << filename << endl;
    ofstream os(filename, ios::binary);
    cereal::BinaryOutputArchive oarchive(os);
    oarchive(t);
}


template <typename T>
T loadObject(string filename){
    log() << "Start Loading Object from " << filename << endl;
    ifstream is(filename, ios::binary);
    cereal::BinaryInputArchive iarchive(is);

    T t;
    iarchive(t);
    return t;
}

#endif