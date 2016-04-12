#include "ml/classifier.h"

void NaiveBayes::reset() {
    clsProb.clear();
    clsFeatureProb.clear();
}

void NaiveBayes::train(const Dataset *dataset) {
    unordered_map<double, double> clsWordCount;
    unordered_set<string> featureSet;

    clsFeatureProb[SPAMMER_VALUE] = unordered_map<string, double, hashString>();
    clsFeatureProb[NON_SPAMMER_VALUE] =
        unordered_map<string, double, hashString>();

    // collect features and compute class probability
    for (auto instance = dataset->instances.begin(),
              end = dataset->instances.end();
         instance != end; instance++) {
        double cls = instance->getClassValue();
        clsProb[cls] += 1.0;
        for (auto kv = instance->values.begin(), iend = instance->values.end();
             kv != iend; kv++) {
            clsFeatureProb[cls][kv->first] += kv->second;
            clsWordCount[cls] += kv->second;
            featureSet.insert(kv->first);
        }
    }

    // compute class probability
    for (auto &kv : clsProb) {
        clsProb[kv.first] = log(kv.second / dataset->size());
    };

    LOG_VAR(featureSet.size());
    LOG_VAR(clsWordCount);
    LOG_VAR(clsProb);

    writeFile(PATH + "MNB-" + dataset->name, [&](ofstream &out) {
        // compute class feature probability
        int featureSize = featureSet.size();

        for (auto &kv : clsFeatureProb) {
            out << "Class Label = " << kv.first << endl;

            for (auto &k : featureSet) {
                double v = 0.0;
                if (kv.second.find(k) != kv.second.end()) {
                    v = kv.second[k];
                }

                out << fixed << k << "\t" << v << "\t";

                kv.second[k] =
                    log((v + 1.0) / (clsWordCount[kv.first] + featureSize));

                out << kv.second[k] << endl;
            };
        };
    });
}

double NaiveBayes::classify(const Instance &ins) {
    double cls = 0.0;
    double prob = -1.0;

    for (auto &ckv : clsFeatureProb) {
        double thisCls = ckv.first;
        double thisProb = clsProb[thisCls];

        for (auto kv = ins.values.begin(), end = ins.values.end(); kv != end;
             kv++) {
            if (clsFeatureProb[thisCls].find(kv->first) !=
                clsFeatureProb[thisCls].end()) {
                thisProb += clsFeatureProb[thisCls][kv->first] * kv->second;
            }
        }


        if (thisProb > prob || prob == -1.0) {
            cls = thisCls;
            prob = thisProb;
        }else if (thisProb == prob) {
            LOG("equal when classifying");
            return -1.0;
        }
    }

    return cls;
}

void BernoulliNaiveBayes::reset() {
    clsProb.clear();
    clsFeatureProb.clear();
}

void BernoulliNaiveBayes::train(const Dataset *dataset) {
    unordered_set<string> featureSet;
    unordered_map<double, double> instanceCounter;

    clsFeatureProb[SPAMMER_VALUE] = unordered_map<string, double, hashString>();
    clsFeatureProb[NON_SPAMMER_VALUE] =
        unordered_map<string, double, hashString>();

    // collect features and compute class probability
    for (auto instance = dataset->instances.begin(),
              end = dataset->instances.end();
         instance != end; instance++) {
        double cls = instance->getClassValue();
        clsProb[cls] += 1.0;
        instanceCounter[cls] += 1.0;
        for (auto kv = instance->values.begin(), iend = instance->values.end();
             kv != iend; kv++) {
            if (kv->second != 0) {
                clsFeatureProb[cls][kv->first]++;
                featureSet.insert(kv->first);
            }
        }
    }

    // compute class probability
    for (auto &kv : clsProb) {
        clsProb[kv.first] = log(kv.second / dataset->size());
        totalFalseValues[kv.first] = clsProb[kv.first];
    };

    LOG_VAR(featureSet.size());
    LOG_VAR(instanceCounter);
    LOG_VAR(clsProb);

    writeFile(PATH + "BNB-" + dataset->name, [&](ofstream &out) {
        // compute class feature probability
        for (auto &kv : clsFeatureProb) {
            out << "Class Label = " << kv.first << endl;

            for (auto &k : featureSet) {
                double v = 0.0;
                if (kv.second.find(k) != kv.second.end()) {
                    v = kv.second[k];
                }
                out << fixed << k << "\t" << v << "\t";

                kv.second[k] = (v + 1.0) / (instanceCounter[kv.first] + 2.0);
                
                out << kv.second[k] << endl;
            };
        };
    });

    for (auto &ckv : clsProb) {
        for (auto &kv : clsFeatureProb[ckv.first]) {
            totalFalseValues[ckv.first] += log(1 - kv.second);
        }
    };
}

double BernoulliNaiveBayes::classify(const Instance &ins) {
    double cls = 0.0;
    double prob = -1.0;
    counter++;

    writeFile(PATH + "bnb/" + to_string(counter) + ".txt", [&](ofstream &out) {
        for (auto &ckv : clsFeatureProb) {
            double thisCls = ckv.first;
            double thisProb = totalFalseValues[thisCls];

            out << "Label = " << thisCls << endl;
            out << "Total False = " << thisProb << endl;

            for (auto &kv : ins.values) {
                out << kv.first << "\t";

                if (clsFeatureProb[thisCls].find(kv.first) !=
                    clsFeatureProb[thisCls].end()) {
                    out << clsFeatureProb[thisCls][kv.first] << endl;
                    thisProb = thisProb + log(clsFeatureProb[thisCls][kv.first]) -
                               log(1 - clsFeatureProb[thisCls][kv.first]);
                }else{
                    out << 1 - clsFeatureProb[thisCls][kv.first] << endl;
                }
            }

            out << "Prob = " << thisProb << endl;

            if (thisProb > prob || prob == -1.0) {
                cls = thisCls;
                prob = thisProb;
            }
        }
    });

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

    clsFeatureProb[SPAMMER_VALUE] = unordered_map<string, double, hashString>();
    clsFeatureProb[NON_SPAMMER_VALUE] =
        unordered_map<string, double, hashString>();

    // collect features and compute class probability
    LOG("Training");
    int count = 0;
    for (auto instance = dataset->instances.begin(),
              end = dataset->instances.end();
         instance != end; instance++) {
        if (count % 1000 == 0) {
            LOG("Processed ", count, " instances");
        }
        double cls = instance->getClassValue();
        clsProb[cls] += 1.0;

        int i = 0;
        for (auto kv = topFeatureList->begin(); i < size; kv++, i++) {
            string k = kv->first;

            clsFeatureProb[cls][k] += instance->at(k);
            clsWordCount[cls] += instance->at(k);
            featureSet.insert(k);
        }
        count++;
    }

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

        int i = 0;
        for (auto kv = topFeatureList->begin(); i < size; kv++, i++) {
            if (clsFeatureProb[thisCls].find(kv->first) !=
                clsFeatureProb[thisCls].end()) {
                thisProb +=
                    clsFeatureProb[thisCls][kv->first] * ins.at(kv->first);
            }
        }
        if (thisProb > prob || prob == -1.0) {
            cls = thisCls;
            prob = thisProb;
        }
    };

    return cls;
}
