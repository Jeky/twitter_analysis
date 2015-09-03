#include "data.h"
#include "utils.h"

void convertToDS() {
    unordered_map<long, User> *spammers = loadSpammers();
    unordered_map<long, User> *nonSpammers = loadSampledNonSpammers();

    LOG("User loaded. Convert users to Dataset...");

    Dataset *spammerDS = user2Dataset(spammers, 1);
    delete spammers;

    Dataset *nonSpammerDS = user2Dataset(nonSpammers, 1);
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

    int spammerTweet = 0;
    for (auto &kv: *spammers) {
        spammerTweet += kv.second.getTweets().size();
    }
    int nonSpammerTweet = 0;
    for (auto &kv: *nonSpammers) {
        nonSpammerTweet += kv.second.getTweets().size();
    }

    LOG_VAR(spammers->size());
    LOG_VAR(nonSpammers->size());
    LOG_VAR(spammerTweet);
    LOG_VAR(nonSpammerTweet);

    delete spammers;
    delete nonSpammers;
}

int main(int argc, char const *argv[]) {
    sampleNonSpammers();
    unordered_map<long, User> *nonSpammers = loadSampledNonSpammers();

    int count = 0;
    for (auto &kv : *nonSpammers) {
        count += kv.second.getTweets().size();
    }
    LOG_VAR(nonSpammers->size());
    LOG_VAR(count);

    delete nonSpammers;
    return 0;
}
