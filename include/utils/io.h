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

void readFile(const string &filename, bool log, function<bool(int, string &)> lineHandler);

void readFile(const string &filename, function<bool(int, string &)> lineHandler);

void writeFile(const string &filename, function<void(ofstream &)> writer);

template <typename T> void saveObject(T *t, const string filename) {
    LOG("Start Saving Object to ", filename);
    ofstream os(filename, ios::binary);
    cereal::BinaryOutputArchive oarchive(os);
    oarchive(*t);
}

template <typename T> T *loadObject(const string filename) {
    LOG("Start Loading Object from ", filename);
    ifstream is(filename, ios::binary);
    cereal::BinaryInputArchive iarchive(is);

    T *t = new T();
    iarchive(*t);
    return t;
}

#endif