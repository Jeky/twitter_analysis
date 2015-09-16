#include "ml/classifier.h"

void NaiveBayes::reset() {
    clsProb.clear();
    clsFeatureProb.clear();
}

void NaiveBayes::train(const Dataset *dataset) {
    unordered_map<double, double> clsWordCount;
    unordered_set<string> featureSet;

    // collect features and compute class probability
    dataset->eachInstance([&](const Instance &instance) {
        double cls = instance.getClassValue();
        if (clsProb.find(cls) == clsProb.end()) {
            clsFeatureProb[cls] = unordered_map<string, double>();
        }

        mapAdd(clsProb, cls, 1.0);

        instance.eachFeature([&](const string &key, const double &value) {
            mapAdd(clsFeatureProb[cls], key, value);
            mapAdd(clsWordCount, cls, value);
            featureSet.insert(key);
        });
    });

    LOG_VAR(featureSet.size());

    // compute class probability
    for (auto &kv : clsProb) {
        clsProb[kv.first] = log(kv.second / dataset->size());
    };

    // compute class feature probability
    int featureSize = featureSet.size();
    for (auto &kv : clsFeatureProb) {
        for (auto &k : featureSet) {
            double v = 0.0;
            if (kv.second.find(k) != kv.second.end()) {
                v = kv.second[k];
            }

            kv.second[k] =
                log((v + 1.0) / (clsWordCount[kv.first] + featureSize));
        };
    };
}

double NaiveBayes::classify(const Instance &ins) {
    double cls = 0.0;
    double prob = -1.0;

    for (auto &kv : clsFeatureProb) {
        double thisCls = kv.first;
        double thisProb = clsProb[thisCls];

        ins.eachFeature([&](const string &key, const double &value) {
            if (clsFeatureProb[thisCls].find(key) !=
                clsFeatureProb[thisCls].end()) {
                thisProb += clsFeatureProb[thisCls][key] * value;
            }
        });

        if (thisProb > prob || prob == -1.0) {
            cls = thisCls;
            prob = thisProb;
        }
    };

    return cls;
}

/**
 * Featured Naive Bayes
 */
void FeaturedNaiveBayes::reset() {
    clsProb.clear();
    clsFeatureProb.clear();
}

void FeaturedNaiveBayes::train(const Dataset *dataset) {
    unordered_map<double, double> clsWordCount;
    unordered_set<string> featureSet;

    // collect features and compute class probability
    dataset->eachInstance([&](const Instance &instance) {
        double cls = instance.getClassValue();
        if (clsProb.find(cls) == clsProb.end()) {
            clsFeatureProb[cls] = unordered_map<string, double>();
        }

        mapAdd(clsProb, cls, 1.0);
        for (int i = 0; i < size; i++) {
            mapAdd(clsFeatureProb[cls], topFeatureList->at(i).first,
                   instance.at(topFeatureList->at(i).first));
            mapAdd(clsWordCount, cls, instance.at(topFeatureList->at(i).first));
            featureSet.insert(topFeatureList->at(i).first);
        }
    });

    LOG_VAR(featureSet.size());

    // compute class probability
    for (auto &kv : clsProb) {
        clsProb[kv.first] = log(kv.second / dataset->size());
    };

    // compute class feature probability
    int featureSize = featureSet.size();
    for (auto &kv : clsFeatureProb) {
        for (auto &k : featureSet) {
            double v = 0.0;
            if (kv.second.find(k) != kv.second.end()) {
                v = kv.second[k];
            }

            kv.second[k] =
                log((v + 1.0) / (clsWordCount[kv.first] + featureSize));
        };
    };
}

double FeaturedNaiveBayes::classify(const Instance &ins) {
    double cls = 0.0;
    double prob = -1.0;

    for (auto &kv : clsFeatureProb) {
        double thisCls = kv.first;
        double thisProb = clsProb[thisCls];

        for (int i = 0; i < size; i++) {
            if (clsFeatureProb[thisCls].find(topFeatureList->at(i).first) !=
                clsFeatureProb[thisCls].end()) {
                thisProb +=
                    clsFeatureProb[thisCls][topFeatureList->at(i).first] *
                    ins.at(topFeatureList->at(i).first);
            }
        }

        if (thisProb > prob || prob == -1.0) {
            cls = thisCls;
            prob = thisProb;
        }
    };

    return cls;
}
