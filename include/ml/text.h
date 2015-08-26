#ifndef _TEXT_H_
#define _TEXT_H_

#include <regex>
#include <iterator>
#include <algorithm>

#include "porter2_stemmer.h"
#include "../utils.h"


vector<string> *splitWords(const string &text);

void toLowerString(string &word);

void normalize(string &word);

vector<string> *toGrams(const string &text, const int gram);


#endif