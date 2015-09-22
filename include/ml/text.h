#ifndef TWITTER_ANALYSIS_ML_TEXT_H
#define TWITTER_ANALYSIS_ML_TEXT_H

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
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(
                                                   std::isspace))).base(),
            s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) { return ltrim(rtrim(s)); }

bool isDigitStr(const string &str);

void unescapeHTML(string &s);

vector<string> *splitWords(const string &text);

void toLowerString(string &word);

void normalize(string &word);

unordered_set<string> *loadStops();

vector<string> *filterStopWords(vector<string> *tokens,
                                unordered_set<string> *stops);
/**
 * Remove Special Tokens from token vector. Special Token contains:
 * #hashtag
 * @username
 * digital numbers
 * URLs
 */
vector<string> *filterSpecialWords(vector<string> *tokens);

vector<string> *toGrams(const string &text, int gram = 1);

#endif
