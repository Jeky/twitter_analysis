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
    fill_n(tweetCount, users->size(), 0);
    int *retweetCount = new int[users->size()];
    fill_n(retweetCount, users->size(), 0);
    int *mentionCount = new int[users->size()];
    fill_n(mentionCount, users->size(), 0);
    int *urlCount = new int[users->size()];
    fill_n(urlCount, users->size(), 0);

    vector<int> tweetLens;
    string urlPrefix = "http://";

    int index = 0;
    for (auto &&kv : *users) {
        if (dataset->size() % 1000 == 0) {
            LOG("Processed ", dataset->size(), " users");
        }
        Instance ins;
        ins.setClassValue(isSpammer ? SPAMMER_VALUE : NON_SPAMMER_VALUE);
        for (auto &&t : kv.second.getTweets()) {
            vector<string> *tokens = toGrams(t.getText(), gramLen);
//            vector<string> *tokens = filterSpecialWords(grams);
            tokenFreq.count(tokens);
            tweetLens.push_back(tokens->size());
            bool isRetweet = false;
            bool isMention = false;
            bool isURL = false;


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
                retweetCount[index]++;
            }
            if (isMention) {
                mentionCount[index]++;
            }
            if (isURL) {
                urlCount[index]++;
            }

            delete tokens;
//            delete grams;
        };

        dataset->addInstance(ins);

        tweetCount[index] += kv.second.getTweets().size();
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

void testPropClassification() {
    auto *spammerDS = loadPropDataset(true);
    auto *nonSpammerDS =loadPropDataset(false);

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
    FeatureSelector *selector;
    // mi
    selector = new BiClassMutualInformation();
    selector->train(all);
    selector->save(PATH + "selected-feature-mi.txt");
    delete selector;

    // wapmi
    for (int i = 1; i <= 3; i++) {
        FeatureSelector *selector = new BIClassWAPMI(i);
        selector->train(all);
        selector->save(PATH + "selected-feature-wapmi-a" + to_string(i) +
                       ".txt");

        delete selector;
    }
    delete all;
}

void testFeatureRelation() {
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    Evaluator eval;

    eval.featureSelectionValidate(spammerDS, nonSpammerDS,
                                  PATH + "selected-feature-mi.txt",
                                  PATH + "mi-test.txt", 10, 1000001);

    for (int i = 1; i <= 3; i++) {
        eval.reset();
        eval.featureSelectionValidate(
            spammerDS, nonSpammerDS,
            PATH + "selected-feature-wapmi-a" + to_string(i) + ".txt",
            PATH + "wapmi-a" + to_string(i) + "-test.txt", 10, 1000001);
    }

    delete spammerDS;
    delete nonSpammerDS;
}

void toUserMatrix(int size = 100){
    auto *spammerDS = loadPropDataset(true);
    auto *nonSpammerDS =loadPropDataset(false);

    vector<string> features {"__COUNT__", "__URL__", "__MENTION__", "__HASHTAG__"};
    size = feature.size();

    writeFile(PATH + "user-matrix.txt", [&](ofstream &out){
    	for(auto &instance : spammerDS->instances){
    		for(int i = 0; i < size; i++){
    			out << instance[features[i]] << "\t";
    		}
    		out << endl;
    	}
    	for(auto &instance : nonSpammerDS->instances){
    		for(int i = 0; i < size; i++){
    			out << instance[features[i]] << "\t";
    		}
    		out << endl;
    	}
    });

    delete spammerDS;
    delete nonSpammerDS;
}

int main(int argc, char const *argv[]) {
//    analyzeAll();
//    testClassification();
//    testFeatureSelection();
//    testFeatureRelation();
    testPropClassification();
    return 0;
}
