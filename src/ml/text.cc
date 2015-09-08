#include "ml/text.h"

vector<string> *splitWords(const string &text) {
    regex rgx("((http)|(https))://(\\w+|\\.|/)+|\\w+");
    vector<string> *words = new vector<string>();
    for (sregex_iterator it(text.begin(), text.end(), rgx), it_end;
         it != it_end; ++it) {
        words->push_back((*it)[0]);
    }

    return words;
}

void toLowerString(string &word) {
    transform(word.begin(), word.end(), word.begin(), (int (*)(int))tolower);
}

void normalize(string &word) {
    toLowerString(word);
    if (word.find("http://") != 0) {
        Porter2Stemmer::stem(word);
    }
}

unordered_set<string> *loadStops() {
    unordered_set<string> *stops = new unordered_set<string>();
    readFile(STOP_WORDS_LIST, [&](int i, string &line) {
        stops->insert(line);
        return true;
    });

    return stops;
}

vector<string> *filterStopWords(vector<string> *tokens,
                                unordered_set<string> *stops) {
    vector<string> *filteredTokens = new vector<string>();
    for (auto &s : *tokens) {
        if (stops->find(s) == stops->end()) {
            filteredTokens->push_back(s);
        }
    }

    return filteredTokens;
}

vector<string> *toGrams(const string &text, const int gramLen) {
    vector<string> *words = splitWords(text);
    vector<string> *grams = new vector<string>();

    if (words->empty()) {
        delete words;
        return grams;
    }

    for (int i = 0; i < words->size(); i++) {
        normalize((*words)[i]);
    }

    for (int i = 0; i < words->size() - gramLen + 1; i++) {
        string g = string();
        for (int j = 0; j < gramLen; j++) {
            g += (*words)[i + j] + " ";
        }
        trim(g);
        grams->push_back(g);
    }

    delete words;
    return grams;
}
