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

#endif
