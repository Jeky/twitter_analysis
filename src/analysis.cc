#include "data.h"
#include "ml/text.h"
#include "utils.h"
#include <iterator>

void convertToDS() {
    unordered_map<long, User> *spammers = loadSpammers();
    unordered_map<long, User> *nonSpammers = loadSampledNonSpammers();

    LOG("User loaded. Convert users to Dataset...");

    Dataset *spammerDS = user2Dataset(spammers);
    delete spammers;

    Dataset *nonSpammerDS = user2Dataset(nonSpammers);
    delete nonSpammers;

    LOG("Save datasets...");
    saveObject(spammerDS, PATH + "spammer.dat");
    delete spammerDS;

    saveObject(nonSpammerDS, PATH + "nonspammer.dat");
    delete nonSpammerDS;
}


Counter<string> *countTokens(unordered_map<long, User> *users){
    Counter<string> *tokenCounter = new Counter<string>();
    for (auto &kv: *users) {
        for(auto &t : kv.second.getTweets()){
            vector<string> *words = toGrams(t.getText());
            tokenCounter->count(words);
            delete words;
        }
    }

    return tokenCounter;
}


void printDatasetStatistic() {
    unordered_map<long, User> *spammers = loadSpammers();
    unordered_map<long, User> *nonSpammers = loadSampledNonSpammers();

    LOG("Counting Tokens in Tweets from Spammers");
    Counter<string> *spammerTokenCounter = countTokens(spammers);

    LOG("Counting Tokens in Tweets from NonSpammers");
    Counter<string> *nonSpammerTokenCounter = countTokens(nonSpammers);

    LOG_VAR(spammerTokenCounter->size());
    LOG_VAR(nonSpammerTokenCounter->size());

    saveObject(spammerTokenCounter, SPAMMER_TOKEN_COUNTER);
    saveObject(nonSpammerTokenCounter, NON_SPAMMER_TOKEN_COUNTER);

    delete spammers;
    delete nonSpammers;
}

void countGramsInTweets(Counter<string> &counter, User &u, int gramLen) {
    for (auto &t : u.getTweets()) {
        vector<string> *grams = toGrams(t.getText(), gramLen);
        counter.count(grams);
        delete grams;
    }
}

int main(int argc, char const *argv[]) {
    printDatasetStatistic();
    return 0;
}
