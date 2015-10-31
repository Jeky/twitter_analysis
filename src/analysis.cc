#include "data.h"
#include "ml/text.h"
#include "ml/classifier.h"
#include "ml/evaluator.h"
#include "ml/features.h"

#include "utils.h"
#include <iterator>

void saveIntArr(int *arr, int len, const string &path) {
    writeFile(path, [&](ofstream &out) {
        for (int i = 0; i < len; i++) {
            out << arr[i] << endl;
        }
    });
    delete arr;
}

void analyzeDataset(unordered_map<long, User> *users, bool isSpammer) {
    LOG("Analyzing");
    Dataset *dataset = new Dataset();
    int gramLen = 1;
    Counter<string> tokenFreq;
    int *tweetCount = new int[users->size()];
    int *retweetCount = new int[users->size()];
    int *mentionCount = new int[users->size()];
    int *urlCount = new int[users->size()];
    vector<int> tweetLens;
    string urlPrefix = "http://";

    int index = 0;
    for (auto &&kv : *users) {
        if (dataset->size() % 1000 == 0) {
            LOG("Processed ", dataset->size(), " users");
        }
        Instance ins;
        ins.setClassValue(isSpammer ? SPAMMER_VALUE : NON_SPAMMER_VALUE);
        bool isRetweet = false;
        bool isMention = false;
        bool isURL = false;

        for (auto &&t : kv.second.getTweets()) {
            vector<string> *grams = toGrams(t.getText(), gramLen);
            vector<string> *tokens = filterSpecialWords(grams);
            tokenFreq.count(tokens);
            tweetLens.push_back(tokens->size());

            for (auto &&g : *tokens) {
                ins[g] += 1.0;
                if (g[0] == '#') {
                    isMention = true;
                }
                if (g.compare(0, urlPrefix.size(), urlPrefix) == 0) {
                    isURL = true;
                }
            };

            if (t.isRetweet()) {
                isRetweet = true;
            }

            delete tokens;
            delete grams;
        };

        dataset->addInstance(ins);

        tweetCount[index] += kv.second.getTweets().size();
        if (isRetweet) {
            retweetCount[index]++;
        }
        if (isMention) {
            mentionCount[index] ++;
        }
        if (isURL) {
            urlCount[index] ++;
        }
        index++;
    };

    tokenFreq.save(isSpammer ? SUSPENDED_TOKEN_FREQ : NON_SUSPENDED_TOKEN_FREQ);
    saveObject(dataset, isSpammer ? SPAMMER_DS : NON_SPAMMER_DS);
    saveIntArr(tweetCount, users->size(),
               isSpammer ? SUSPENDED_TWEET_COUNT : NON_SUSPENDED_TWEET_COUNT);
    saveIntArr(retweetCount, users->size(), isSpammer
                                                ? SUSPENDED_RETWEET_COUNT
                                                : NON_SUSPENDED_RETWEET_COUNT);
    saveIntArr(mentionCount, users->size(), isSpammer
                                                ? SUSPENDED_MENTION_COUNT
                                                : NON_SUSPENDED_MENTION_COUNT);
    saveIntArr(urlCount, users->size(),
               isSpammer ? SUSPENDED_URL_COUNT : NON_SUSPENDED_URL_COUNT);
    writeFile(isSpammer ? SUSPENDED_TWEET_LEN : NON_SUSPENDED_TWEET_LEN,
              [&](ofstream &out) {
        for (auto &&len : tweetLens) {
            out << len << endl;
        }
    });
}

void analyzeAll() {
    auto *spammers = loadSpammers();
    analyzeDataset(spammers, true);
    delete spammers;

    auto *nonSpammers = loadNonSpammers();
    analyzeDataset(nonSpammers, false);
    delete nonSpammers;
}

void testClassification() {
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    Classifier *cls = new NaiveBayes();
    Evaluator eval;

    eval.crossValidate(10, cls, spammerDS, nonSpammerDS);
    for (auto &&item : eval.getConfusionMatrixVector()) {
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

    FeatureSelector *selector = new BIClassWAPMI();
    selector->train(all);
    selector->save(PATH + "selected-feature-wapmi-a2.txt");

    delete selector;
    delete all;
}

void testFeatureRelation() {
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    Evaluator eval;

    eval.featureSelectionValidate(spammerDS, nonSpammerDS,
                                  PATH + "selected-feature-wapmi-a2.txt",
                                  PATH + "wapmi-a2-test.txt", 100, 10000);

    eval.featureSelectionValidate(spammerDS, nonSpammerDS,
                                  PATH + "selected-feature-wapmi-a3.txt",
                                  PATH + "wapmi-a3-test.txt", 100, 10000);

    delete spammerDS;
    delete nonSpammerDS;
}

int main(int argc, char const *argv[]) {
	analyzeAll();

    return 0;
}
