#ifndef _TEXT_H_
#define _TEXT_H_

#include <regex>
#include <iterator>
#include <algorithm>

#include "porter2_stemmer.h"
#include "utils.h"


Vector<String> splitWords(const String &text);

void toLowerString(String &word);

void normalize(String &word);

Vector<String> toGrams(const String &text, const int gram);


#endif