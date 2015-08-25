#include "ml/features.h"

Vector<Pair<String, double>> *FeatureSelector::getTopFeatureList() {
    Vector<Pair<String, double>> *v = new Vector<Pair<String, double>>();

    for (auto &kv : featureScoreMap) {
        v->push_back(make_pair(kv.first, kv.second));
    }

    sort(v->begin(), v->end(), [](const Pair<String, double> &left, const Pair<String, double> &right) {
        return left.second > right.second;
    });

    return v;
}


void BiClassMutualInformation::train(Dataset *dataset) {
    LOG("Training Mutual Information Feature Selector");

    Map<String, array<double, 4>> featureMatrix;
    Set<double> clsSet;

    // initialize feature matrix
    // row    = number of features
    // column = [1.0, 1.0, 1.0, 1.0]
    for (Instance i : *dataset) {
        clsSet.insert(i.getClassValue());

        for (auto &kv : i) {
            array<double, 4> score;
            for (int j = 0; j < 4; j++) {
                score[j] = 1.0;
            }
            featureMatrix[kv.first] = score;
        }
    }

    double cls = *clsSet.begin();

    LOG_VAR(featureMatrix.size());

    int featureCount = 0;
    for (auto &kv : featureMatrix) {
        if (featureCount % 1000 == 0) {
            LOG("Processed ", featureCount, " Features");
        }
        for (Instance instance : *dataset) {
            if (instance.hasAttribute(kv.first)) {
                if (cls == instance.getClassValue()) {
                    featureMatrix[kv.first][0] += 1;
                } else {
                    featureMatrix[kv.first][1] += 1;
                }
            } else {
                if (cls == instance.getClassValue()) {
                    featureMatrix[kv.first][2] += 1;
                } else {
                    featureMatrix[kv.first][3] += 1;
                }
            }
        }
        featureCount++;
    }

    LOG_VAR(featureMatrix);

    for (auto &kv : featureMatrix) {
        array<double, 4> fm = kv.second;
        int N = dataset->size();
        double score = fm[0] / N * (log(N) + log(fm[0]) - log(fm[0] + fm[1]) - log(fm[0] + fm[2])) +
                       fm[1] / N * (log(N) + log(fm[1]) - log(fm[0] + fm[1]) - log(fm[1] + fm[3])) +
                       fm[2] / N * (log(N) + log(fm[2]) - log(fm[2] + fm[3]) - log(fm[0] + fm[2])) +
                       fm[3] / N * (log(N) + log(fm[3]) - log(fm[2] + fm[3]) - log(fm[1] + fm[3]));
        featureScoreMap[kv.first] = score;
    }
}
