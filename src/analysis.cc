#include "data.h"
#include "ml/text.h"
#include "utils.h"
#include <iterator>

#include "ml/porter2_stemmer.h"

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

Counter<string> *countTokens(unordered_map<long, User> *users, const string &path) {
    ifstream infile(path);

    if (!infile.good()) {
        LOG("Cannot Find ", path, ". Counting Tokens...");
        infile.close();

        Counter<string> *tokenCounter = new Counter<string>();
        int i = 0;
        for (auto &kv : *users) {
            if (i % 1000 == 0) {
                LOG("Process ", i, " users");
            }
            for (auto &t : kv.second.getTweets()) {
                vector<string> *words = toGrams(t.getText());
                tokenCounter->count(words);
                delete words;
            }
            i++;
        }

        saveObject(tokenCounter, path);
        return tokenCounter;
    } else {
        LOG("Loading Data from ", path);
        infile.close();

        return loadObject<Counter<string>>(path);
    }
}

void printDatasetStatistic() {
    unordered_map<long, User> *spammers = loadSpammers();
    unordered_map<long, User> *nonSpammers = loadSampledNonSpammers();

    LOG("Counting Tokens in Tweets from Spammers");
    Counter<string> *spammerTokenCounter = countTokens(spammers, SPAMMER_TOKEN_COUNTER);

    LOG("Counting Tokens in Tweets from NonSpammers");
    Counter<string> *nonSpammerTokenCounter = countTokens(nonSpammers, NON_SPAMMER_TOKEN_COUNTER);

    delete spammers;
    delete nonSpammers;

    unordered_set<string> *spammerTokens = spammerTokenCounter->getKeySet();
    unordered_set<string> *nonSpammerTokens = nonSpammerTokenCounter->getKeySet();
    unordered_set<string> *sharedTokens = setIntersection(spammerTokens, nonSpammerTokens);
    unordered_set<string> *allTokens = setUnion(spammerTokens, nonSpammerTokens);

    LOG_VAR(spammerTokens->size());
    LOG_VAR(nonSpammerTokens->size());
    LOG_VAR(sharedTokens->size());
    LOG_VAR(allTokens->size());

    spammerTokenCounter->saveFrequency(SPAMMER_TOKEN_FREQ);
    nonSpammerTokenCounter->saveFrequency(NON_SPAMMER_TOKEN_FREQ);

    spammerTokenCounter->addCounter(*nonSpammerTokenCounter);
    spammerTokenCounter->saveFrequency(ALL_TOKEN_FREQ);

    delete spammerTokenCounter;
    delete nonSpammerTokenCounter;
    delete sharedTokens;
    delete allTokens;
}

int main(int argc, char const *argv[]) {
    printDatasetStatistic();
    return 0;
}