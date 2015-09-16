#include "data.h"
#include "ml/text.h"
#include "ml/classifier.h"
#include "ml/evaluator.h"
#include "ml/features.h"

#include "utils.h"
#include <iterator>

void convertToDS() {
    auto *spammers = loadSpammers();
    auto *nonSpammers = loadSampledNonSpammers();

    LOG("User loaded. Convert users to Dataset...");

    auto *spammerDS = user2Dataset(spammers);
    delete spammers;

    auto *nonSpammerDS = user2Dataset(nonSpammers);
    delete nonSpammers;

    LOG("Save datasets...");
    saveObject(spammerDS, SPAMMER_DS);

    delete spammerDS;

    saveObject(nonSpammerDS, NON_SPAMMER_DS);
    delete nonSpammerDS;
}

Counter<string> *countTokens(unordered_map<long, User> *users,
                             const string &path) {
    Counter<string> *tokenCounter = new Counter<string>();
    int i = 0;
    for (auto &kv : *users) {
        if (i % 1000 == 0) {
            LOG("Process ", i, " users");
        }
        for (auto &t : kv.second.getTweets()) {
            vector<string> *words = toGrams(t.getText());
            vector<string> *tokens = filterSpecialWords(words);
            tokenCounter->count(tokens);
            delete words;
            delete tokens;
        }
        i++;
    }

    return tokenCounter;
}

void countByLen(const Counter<string> *counter, const string &path) {
    LOG("Counting Tokens by Length into ", path);
    Counter<int> lenCounter;
    counter->eachEntry([&](const string &key, const int &value) {
        lenCounter.count(key.size(), value);
    });

    auto *top = lenCounter.getTop();
    writeFile(path, [&](ofstream &out) {
        for (auto &i : *top) {
            out << i.first << "\t" << i.second << endl;
        }
    });
}

int countTweets(unordered_map<long, User> *users) {
    int count = 0;
    for (auto &kv : *users) {
        count += kv.second.getTweets().size();
    }
    return count;
}

void printDatasetStatistic() {
    auto *spammers = loadSpammers();
    auto *nonSpammers = loadSampledNonSpammers();

    LOG_VAR(spammers->size());
    LOG_VAR(nonSpammers->size());
    LOG_VAR(countTweets(spammers));
    LOG_VAR(countTweets(nonSpammers));

    LOG("Counting Tokens in Tweets from Spammers");
    auto *spammerTokenCounter = countTokens(spammers, SPAMMER_TOKEN_COUNTER);

    LOG("Counting Tokens in Tweets from NonSpammers");
    auto *nonSpammerTokenCounter =
        countTokens(nonSpammers, NON_SPAMMER_TOKEN_COUNTER);

    LOG("Counting All Tokens in Tweets");
    auto *allTokenCounter = spammerTokenCounter->clone();
    allTokenCounter->addCounter(*nonSpammerTokenCounter);

    delete spammers;
    delete nonSpammers;

    countByLen(spammerTokenCounter, PATH + "spammer-len-dist.txt");
    countByLen(nonSpammerTokenCounter, PATH + "non-spammer-len-dist.txt");
    countByLen(allTokenCounter, PATH + "all-len-dist.txt");

    auto *spammerTokens = spammerTokenCounter->getKeySet();
    auto *nonSpammerTokens = nonSpammerTokenCounter->getKeySet();
    auto *sharedTokens = setIntersection(spammerTokens, nonSpammerTokens);
    auto *allTokens = setUnion(spammerTokens, nonSpammerTokens);

    LOG("Before removing stops");
    LOG_VAR(spammerTokens->size());
    LOG_VAR(nonSpammerTokens->size());
    LOG_VAR(sharedTokens->size());
    LOG_VAR(allTokens->size());

    delete spammerTokens;
    delete nonSpammerTokens;
    delete sharedTokens;
    delete allTokens;

    spammerTokenCounter->saveFrequency(SPAMMER_TOKEN_FREQ);
    nonSpammerTokenCounter->saveFrequency(NON_SPAMMER_TOKEN_FREQ);
    allTokenCounter->saveFrequency(ALL_TOKEN_FREQ);

    LOG("After removing stops");
    auto *stops = loadStops();
    for (auto &k : *stops) {
        spammerTokenCounter->deleteKey(k);
        nonSpammerTokenCounter->deleteKey(k);
        allTokenCounter->deleteKey(k);
    }

    countByLen(spammerTokenCounter, PATH + "spammer-no-stops-len-dist.txt");
    countByLen(nonSpammerTokenCounter,
               PATH + "non-spammer-no-stops-len-dist.txt");
    countByLen(allTokenCounter, PATH + "all-no-stops-len-dist.txt");

    spammerTokens = spammerTokenCounter->getKeySet();
    nonSpammerTokens = nonSpammerTokenCounter->getKeySet();
    sharedTokens = setIntersection(spammerTokens, nonSpammerTokens);
    allTokens = setUnion(spammerTokens, nonSpammerTokens);

    LOG_VAR(spammerTokens->size());
    LOG_VAR(nonSpammerTokens->size());
    LOG_VAR(sharedTokens->size());
    LOG_VAR(allTokens->size());

    delete stops;

    delete spammerTokenCounter;
    delete nonSpammerTokenCounter;
    delete allTokenCounter;
}

void testClassification() {
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    Classifier *cls = new NaiveBayes();
    Evaluator eval;

    eval.crossValidate(10, cls, spammerDS, nonSpammerDS);
    for (auto &item : eval.getConfusionMatrixVector()) {
        LOG(item);
    }
    LOG_VAR(eval.getAccuracy());
    LOG_VAR(eval.getRecall());
    LOG_VAR(eval.getPrecision());
    LOG_VAR(eval.getF1());

    delete cls;
    delete spammerDS;
    delete nonSpammerDS;
}

void testFeatureSelection() {
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    auto *all = spammerDS;
    all->addDataset(*nonSpammerDS);
    delete nonSpammerDS;

    auto *selector = new BiClassMutualInformation();
    selector->train(all);

    vector<pair<string, double>> *result = selector->getTopFeatureList();
    selector->save(PATH + "feature.txt");

    delete selector;
    delete all;
}

void randomSampleRetweets(int count = 20) {
    const string path = PATH + "spammer-retweets.obj";

    ifstream infile(path);
    vector<Tweet> *retweets;

    if (!infile.good()) {
        LOG("Cannot Find ", path, ". Collecting Retweets");
        infile.close();

        retweets = new vector<Tweet>();

        auto *spammers = loadSpammers();
        for (auto &kv : *spammers) {
            for (auto &t : kv.second.getTweets()) {
                if (t.isRetweet()) {
                    retweets->push_back(t);
                }
            }
        }

        delete spammers;
        saveObject(retweets, path);
    } else {
        LOG("Loading Reteets from ", path);
        infile.close();
        retweets = loadObject<vector<Tweet>>(path);
    }

    // shuffle retweets
    random_shuffle(retweets->begin(), retweets->end());

    for (int i = 0; i < count; i++) {
        LOG((*retweets)[i].getText());
    }
}

vector<double> *collectRR(unordered_map<long, User> *users) {
    vector<double> *rr = new vector<double>();

    for (auto &kv : *users) {
        int r = 0;
        for (auto &t : kv.second.getTweets()) {
            if (t.isRetweet()) {
                r++;
            }
        }
        rr->push_back(1.0 * r / kv.second.getTweets().size());
    }

    return rr;
}

void saveRR() {
    auto *spammers = loadSpammers();
    auto *nonSpammers = loadSampledNonSpammers();

    writeFile(PATH + "spammer-retweet-rate.txt", [&](ofstream &out) {
        vector<double> *rr = collectRR(spammers);
        for (auto &r : *rr) {
            out << r << endl;
        }
        delete rr;
    });

    writeFile(PATH + "non-spammer-retweet-rate.txt", [&](ofstream &out) {
        vector<double> *rr = collectRR(nonSpammers);
        for (auto &r : *rr) {
            out << r << endl;
        }
        delete rr;
    });

    delete spammers;
    delete nonSpammers;
}

vector<double> *collectUCR(unordered_map<long, User> *users) {
    vector<double> *rr = new vector<double>();

    for (auto &kv : *users) {
        if (rr->size() % 1000 == 0) {
            LOG("Process ", rr->size(), " users");
        }
        int r = 0;
        for (auto &t : kv.second.getTweets()) {
            if (t.containsUrl()) {
                r++;
            }
        }
        rr->push_back(1.0 * r / kv.second.getTweets().size());
    }

    return rr;
}

void saveCUR() {
    auto *spammers = loadSpammers();
    auto *nonSpammers = loadSampledNonSpammers();

    writeFile(PATH + "spammer-url-rate.txt", [&](ofstream &out) {
        vector<double> *rr = collectUCR(spammers);
        for (auto &r : *rr) {
            out << r << endl;
        }
        delete rr;
    });

    writeFile(PATH + "non-spammer-url-rate.txt", [&](ofstream &out) {
        vector<double> *rr = collectUCR(nonSpammers);
        for (auto &r : *rr) {
            out << r << endl;
        }
        delete rr;
    });

    delete spammers;
    delete nonSpammers;
}
void collectTweetDist(unordered_map<long, User> *users, const string &fname) {
    Counter<int> allTokenCounter;
    Counter<int> removeSpecialTokenCounter;

    int i = 0;
    for (auto &kv : *users) {
        if (i % 1000 == 0) {
            LOG("Process ", i, " users");
        }
        for (auto &t : kv.second.getTweets()) {
            vector<string> *tokens = splitWords(t.getText());
            if (tokens->size() > 50) {
                LOG("Found Large Tweet:");
                LOG_VAR(kv.second.getId());
                LOG_VAR(t.getText());
                LOG_VAR(*tokens);
            }
            allTokenCounter.count(tokens->size());
            vector<string> *removeSpecialTokens = filterSpecialWords(tokens);
            if (removeSpecialTokens->size() > 50) {
                LOG("Found Large Tweet After Removing Special Tokens:");
                LOG_VAR(kv.second.getId());
                LOG_VAR(t.getText());
                LOG_VAR(*removeSpecialTokens);
            }
            removeSpecialTokenCounter.count(removeSpecialTokens->size());
            delete tokens;
            delete removeSpecialTokens;
        }
        i++;
    }

    writeFile(PATH + fname + "-tweet-len-dist-all.txt", [&](ofstream &out) {
        allTokenCounter.eachEntry(
            [&](const int &k, const int &v) { out << k << "\t" << v << endl; });
    });

    writeFile(PATH + fname + "-tweet-len-dist-rst.txt", [&](ofstream &out) {
        removeSpecialTokenCounter.eachEntry(
            [&](const int &k, const int &v) { out << k << "\t" << v << endl; });
    });
}

void tweetDistAnalysis() {
    auto *spammers = loadSpammers();
    collectTweetDist(spammers, "spammer");
    delete spammers;

    auto *nonSpammers = loadSampledNonSpammers();
    collectTweetDist(nonSpammers, "non-spammer");
    delete nonSpammers;
}

void loadAllNonSpammerUsers() {
    LOG("Start Loading Normal Users...");
    unordered_set<long> spammerIds;
    readFile(SPAMMER_ID_LIST, [&](int i, string &line) {
        spammerIds.insert(stol(line));
        return true;
    });
    LOG("Spammer User Count = ", spammerIds.size());

    unordered_map<long, User> *users = new unordered_map<long, User>();
    readFile(NON_SPAMMER_ID_LIST, [&](int i, string &line) {
        long id = stol(line);
        if (spammerIds.find(id) == spammerIds.end()) {
            User u(id, false);
            u.loadTweets();
            if (u.getTweets().size() >= SAMPLE_TWEET_SIZE) {
                (*users)[id] = u;
            }
        }
        if (users->size() % 100 == 0 && users->size() != 0) {
            LOG("Processed ", users->size(), " users");
        }
        return true;
    });
    LOG_VAR(users->size());

    saveObject<unordered_map<long, User>>(users, PATH + "all-non-spammers.obj");
    delete users;
}

void testFeatureRelation() {
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    Evaluator eval;
    eval.featureSelectionValidate(spammerDS, nonSpammerDS, PATH + "feature.txt",
                                  PATH + "feature-select-result.txt", 100);
    delete spammerDS;
    delete nonSpammerDS;
}

int main(int argc, char const *argv[]) {
    testFeatureRelation();
    return 0;
}
