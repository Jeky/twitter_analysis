#include "ml/text.h"

Vector <String> splitWords(const String &text) {
    regex rgx("\\w+");
    Vector <String> words;
    for (sregex_iterator it(text.begin(), text.end(), rgx), it_end; it != it_end; ++it) {
        words.push_back((*it)[0]);
    }

    return words;
}


void toLowerString(String &word) {
    transform(word.begin(), word.end(), word.begin(), (int (*)(int)) tolower);
}


void normalize(String &word) {
    toLowerString(word);
    Porter2Stemmer::stem(word);
}


Vector <String> toGrams(const String &text, const int gramLen) {
    Vector <String> words = splitWords(text);

    for (int i = 0; i < words.size(); i++) {
        normalize(words[i]);
    }

    Vector <String> grams;

    for (int i = 0; i < words.size() - gramLen + 1; i++) {
        String g = String();
        for (int j = 0; j < gramLen; j++) {
            g += words[i + j] + " ";
        }
        grams.push_back(g);
    }


    return grams;
}
