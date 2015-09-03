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

void printDatasetStatistic() {
    unordered_map<long, User> *spammers = loadSpammers();
    unordered_map<long, User> *nonSpammers = loadSampledNonSpammers();

    Counter<string> spammerTokenCounter;
    Counter<string> nonSpammerTokenCounter;

    int spammerTweet = 0;
    for (auto &kv: *spammers) {
        spammerTweet += kv.second.getTweets().size();
        for(auto &t : kv.second.getTweets()){
            vector<string> *words = toGrams(t.getText());
            spammerTokenCounter.count(words);
            delete words;
        }
    }
    int nonSpammerTweet = 0;
    for (auto &kv: *nonSpammers) {
        nonSpammerTweet += kv.second.getTweets().size();
        for(auto &t : kv.second.getTweets()){
            vector<string> *words = toGrams(t.getText());
            nonSpammerTokenCounter.count(words);
            delete words;
        }
    }

    LOG_VAR(spammers->size());
    LOG_VAR(nonSpammers->size());
    LOG_VAR(spammerTweet);
    LOG_VAR(nonSpammerTweet);
    LOG_VAR(spammerTokenCounter.size());
    LOG_VAR(nonSpammerTokenCounter.size());

    saveObject(&spammerTokenCounter, PATH + string("spammer-token-counter.obj"));
    saveObject(&nonSpammerTokenCounter, PATH + string("non-spammer-token-counter.obj"));

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
