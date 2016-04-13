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

void analyzeDataset(unordered_map<long, User> *users, bool isSpammer, int gramLen) {
    LOG("Analyzing");
    auto stops = loadStops();

    Dataset *dataset = new Dataset();
    Counter<string> tokenFreq;
    int *tweetCount = new int[users->size()];
    fill_n(tweetCount, users->size(), 0);
    int *retweetCount = new int[users->size()];
    fill_n(retweetCount, users->size(), 0);
    int *mentionCount = new int[users->size()];
    fill_n(mentionCount, users->size(), 0);
    int *urlCount = new int[users->size()];
    fill_n(urlCount, users->size(), 0);
    int *hashtagCount = new int[users->size()];
    fill_n(hashtagCount, users->size(), 0);
    int *tokenCount = new int[users->size()];
    fill_n(tokenCount, users->size(), 0);

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
            vector<string> *alltokens = toGrams(t.getText(), gramLen);
            vector<string> *tokens = filterStopWords(alltokens, stops);
            delete alltokens;
            //            vector<string> *tokens = filterSpecialWords(grams);
            tokenFreq.count(tokens);
            tokenCount[index] += tokens->size();
            tweetLens.push_back(tokens->size());
            bool isRetweet = false;
            bool isMention = false;
            bool isURL = false;
            bool isHashtag = false;

            for (auto &&g : *tokens) {
                ins[g] += 1.0;
                if (g[0] == '@') {
                    isMention = true;
                }
                if (g[0] == '#') {
                    isHashtag = true;
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
            if (isHashtag) {
                hashtagCount[index]++;
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
    saveIntArr(hashtagCount, users->size(), isSpammer
                                                ? SUSPENDED_HASHTAG_COUNT
                                                : NON_SUSPENDED_HASHTAG_COUNT);
    saveIntArr(tokenCount, users->size(),
               isSpammer ? SUSPENDED_TOKEN_PER_USER_COUNT
                         : NON_SUSPENDED_TOKEN_PER_USER_COUNT);
    writeFile(isSpammer ? SUSPENDED_TWEET_LEN : NON_SUSPENDED_TWEET_LEN,
              [&](ofstream &out) {
        for (auto &&len : tweetLens) {
            out << len << endl;
        }
    });
}

void analyzeAll(int gramLen) {
    auto *spammers = loadSpammers();
    analyzeDataset(spammers, true, gramLen);
    delete spammers;

    auto *nonSpammers = loadNonSpammers();
    analyzeDataset(nonSpammers, false, gramLen);
    delete nonSpammers;
}

void testPropClassification() {
    auto *spammerDS = loadPropDataset(true);
    auto *nonSpammerDS = loadPropDataset(false);

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

void testClassification(Classifier *cls) {
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    LOG_VAR(spammerDS->size());
    LOG_VAR(nonSpammerDS->size());

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

void testFeatureSelection(FeatureSelector *selector, const string &output) {
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    Classifier *classifier = new NaiveBayes();
    Evaluator eval;
    eval.featureSelectionValidate(10, selector, classifier, spammerDS,
                                  nonSpammerDS, PATH + output); 

    delete selector;
    delete classifier;
    delete spammerDS;
    delete nonSpammerDS;
}

void outputAll() {
    writeFile(PATH + "dataset.txt", [&](ofstream &out) {
        auto *spammers = loadSpammers();
        int i = 0;
        for (auto &kv : *spammers) {
            if (i % 1000 == 0) {
                LOG("Processed ", i, " users");
            }
            out << "!" << kv.first << "\t1\n";
            for (auto &t : kv.second.getTweets()) {
                vector<string> *tokens = toGrams(t.getText(), 1);
                for (auto &t : *tokens) {
                    out << t << " ";
                }
                out << endl;
                delete tokens;
            }
            i++;
        }
        delete spammers;

        auto *nonSpammers = loadNonSpammers();
        i = 0;
        for (auto &kv : *nonSpammers) {
            if (i % 1000 == 0) {
                LOG("Processed ", i, " users");
            }
            out << "!" << kv.first << "\t0\n";
            for (auto &t : kv.second.getTweets()) {
                vector<string> *tokens = toGrams(t.getText(), 1);
                for (auto &t : *tokens) {
                    out << t << " ";
                }
                out << endl;
                delete tokens;
            }
            i++;
        }
        delete nonSpammers;
    });
}

void testClassificationSizeRelation(Classifier *cls, const string &output){
    auto *spammerDS = Dataset::loadDataset(SPAMMER_DS, SPAMMER_VALUE);
    auto *nonSpammerDS =
        Dataset::loadDataset(NON_SPAMMER_DS, NON_SPAMMER_VALUE);

    Classifier *classifier = new NaiveBayes();
    Evaluator eval;
    eval.sizeValidation(classifier, spammerDS, nonSpammerDS, PATH + output); 

    delete classifier;
    delete spammerDS;
    delete nonSpammerDS;
}

int main(int argc, char const *argv[]) {
    // Analyze and generate dataset
    // unigram
    // analyzeAll(1);

    // bigram
    // analyzeAll(2);
    
    // Test classification
    // Classifier *cls = new NaiveBayes();
    // testClassification(cls);

    // cls = new BernoulliNaiveBayes();
    // testClassification(cls);

    // Test classification
    Classifier *cls = new NaiveBayes();
    testClassificationSizeRelation(cls, PATH + "mnb_size_result.txt");

    cls = new BernoulliNaiveBayes();
    testClassificationSizeRelation(cls, PATH + "bnb_size_result.txt");


    //Test feature selection
    // FeatureSelector *selector = new BiClassMutualInformation();
    // testFeatureSelection(selector, "mi");

    // selector = new BiClassPMI();
    // testFeatureSelection(selector, "pmi");

    // selector = new BiClassChi2();
    // testFeatureSelection(selector, "chi2");

    // selector = new BIClassWAPMI();
    // testFeatureSelection(selector, "wapmi");

    return 0;
}
