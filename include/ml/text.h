#ifndef _TEXT_H_
#define _TEXT_H_

#include <regex>
#include <iterator>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#include "porter2_stemmer.h"
#include "../utils.h"
#include "../data.h"

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

vector<string> *splitWords(const string &text);

void toLowerString(string &word);

void normalize(string &word);

vector<string> *toGrams(const string &text, int gram = 1);

void countGramsInTweets(Counter &counter, const User &u, int gramLen = 1);

#endif