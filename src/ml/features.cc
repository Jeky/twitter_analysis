#include "ml/features.h"

vector<pair<string, double>> *FeatureSelector::getTopFeatureList() {
    vector<pair<string, double>> *v = new vector<pair<string, double>>();

    for (auto &kv : featureScoreMap) {
        v->push_back(make_pair(kv.first, kv.second));
    }

    sort(v->begin(), v->end(), [](const pair<string, double> &left,
                                  const pair<string, double> &right) {
        return left.second > right.second;
    });

    return v;
}

double computeScore(int N, array<double, 4> &fm) {
    return fm[0] / N *
               (log(N) + log(fm[0]) - log(fm[0] + fm[1]) - log(fm[0] + fm[2])) +
           fm[1] / N *
               (log(N) + log(fm[1]) - log(fm[0] + fm[1]) - log(fm[1] + fm[3])) +
           fm[2] / N *
               (log(N) + log(fm[2]) - log(fm[2] + fm[3]) - log(fm[0] + fm[2])) +
           fm[3] / N *
               (log(N) + log(fm[3]) - log(fm[2] + fm[3]) - log(fm[1] + fm[3]));
}

void BiClassMutualInformation::train(const Dataset *dataset) {
    LOG("Training Mutual Information Feature Selector");

    unordered_map<string, array<double, 4>> featureMatrix;
    unordered_set<double> clsSet;

    LOG("Initialize Feature Matrix");
    // initialize feature matrix
    // row    = number of features
    // column = [1.0, 1.0, 1.0, 1.0]
    dataset->eachInstance([&](const Instance &i) {
        clsSet.insert(i.getClassValue());

        i.eachFeature([&](const string &key, const double &value) {
            array<double, 4> score;
            for (int j = 0; j < 4; j++) {
                score[j] = 1.0;
            }
            featureMatrix[key] = score;
        });
    });

    double cls = *clsSet.begin();

    LOG_VAR(featureMatrix.size());

    int featureCount = 0;
    for (auto &kv : featureMatrix) {
        if (featureCount % 1000 == 0) {
            LOG("Processed ", featureCount, " Features");
        }
        dataset->eachInstance([&](const Instance &instance) {
            if (instance.hasAttribute(kv.first)) {
                if (cls == instance.getClassValue()) {
                    kv.second[0] += 1;
                } else {
                    kv.second[1] += 1;
                }
            } else {
                if (cls == instance.getClassValue()) {
                    kv.second[2] += 1;
                } else {
                    kv.second[3] += 1;
                }
            }
        });
        featureCount++;
    }

    int N = dataset->size();
    for (auto &kv : featureMatrix) {
        featureScoreMap[kv.first] = computeScore(N, kv.second);
    }
}
