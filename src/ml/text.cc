#include "ml/text.h"

vector <string> splitWords(const string &text) {
    regex rgx("\\w+");
    vector <string> words;
    for (sregex_iterator it(text.begin(), text.end(), rgx), it_end; it != it_end; ++it) {
        words.push_back((*it)[0]);
    }

    return words;
}


void toLowerString(string &word) {
    transform(word.begin(), word.end(), word.begin(), (int (*)(int)) tolower);
}


void normalize(string &word) {
    toLowerString(word);
    Porter2Stemmer::stem(word);
}


vector <string> toGrams(const string &text, const int gramLen) {
    vector <string> words = splitWords(text);

    for (int i = 0; i < words.size(); i++) {
        normalize(words[i]);
    }

    vector <string> grams;

    for (int i = 0; i < words.size() - gramLen + 1; i++) {
        string g = string();
        for (int j = 0; j < gramLen; j++) {
            g += words[i + j] + " ";
        }
        grams.push_back(g);
    }


    return grams;
}
