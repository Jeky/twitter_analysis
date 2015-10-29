#include "ml/features.h"

vector<pair<string, double>> *FeatureSelector::getTopFeatureList() {
    if (topFeatureList == nullptr) {
        topFeatureList = new vector<pair<string, double>>();

        for (auto &kv : featureScoreMap) {
            topFeatureList->push_back(make_pair(kv.first, kv.second));
        }

        sort(topFeatureList->begin(), topFeatureList->end(),
             [](const pair<string, double> &left,
                const pair<string, double> &right) {
            return left.second > right.second;
        });
    }

    return topFeatureList;
}

void FeatureSelector::save(const string &path) {
    auto *result = getTopFeatureList();
    writeFile(path, [&](ofstream &out) {
        for (auto &r : *result) {
            out << r.first << "\t" << r.second << endl;
        }
    });
}

double computeScore(int N, array<double, 4> &fm) {
    return fm[0] / N * (log2(N) + log2(fm[0]) - log2(fm[0] + fm[1]) -
                        log2(fm[0] + fm[2])) +
           fm[1] / N * (log2(N) + log2(fm[1]) - log2(fm[0] + fm[1]) -
                        log2(fm[1] + fm[3])) +
           fm[2] / N * (log2(N) + log2(fm[2]) - log2(fm[2] + fm[3]) -
                        log2(fm[0] + fm[2])) +
           fm[3] / N * (log2(N) + log2(fm[3]) - log2(fm[2] + fm[3]) -
                        log2(fm[1] + fm[3]));
}

void BiClassMutualInformation::train(Dataset *dataset) {
    LOG("Training Mutual Information Feature Selector");

    unordered_map<string, array<double, 4>> featureMatrix;

    LOG("Initialize Feature Matrix");
    // initialize feature matrix
    // row    = number of features
    // column = [1.0, 1.0, 1.0, 1.0]
    for (auto i = dataset->instances.begin(), dend = dataset->instances.end();
         i != dend; i++) {

        for (auto kv = i->values.begin(), iend = i->values.end(); kv != iend;
             kv++) {
            array<double, 4> score;
            for (int j = 0; j < 4; j++) {
                score[j] = 1.0;
            }
            featureMatrix[kv->first] = score;
        };
    };

    double cls = SPAMMER_VALUE;

    LOG_VAR(featureMatrix.size());

    int featureCount = 0;
    for (auto &kv : featureMatrix) {
        if (featureCount % 1000 == 0) {
            LOG("Processed ", featureCount, " Features");
        }
        for (int i = 0; i < dataset->size(); i++) {
            if ((*dataset)[i].hasAttribute(kv.first)) {
                if (cls == (*dataset)[i].getClassValue()) {
                    kv.second[0] += 1;
                } else {
                    kv.second[1] += 1;
                }
            } else {
                if (cls == (*dataset)[i].getClassValue()) {
                    kv.second[2] += 1;
                } else {
                    kv.second[3] += 1;
                }
            }
        }
        featureCount++;
    }

    int N = dataset->size();

    writeFile(PATH + "mi.txt", [&](ofstream &out) {
        for (auto &kv : featureMatrix) {
            featureScoreMap[kv.first] = computeScore(N, kv.second);
            out << kv.first << "\t" << kv.second[0] << "\t" << kv.second[1]
                << "\t" << kv.second[2] << "\t" << kv.second[3] << "\t"
                << featureScoreMap[kv.first] << endl;
        }
    });
}

void BIClassWAPMI::train(Dataset *dataset) {
    LOG("Training Weighted Average Pointwise Mutual Information Feature "
        "Selector");
    unordered_map<string, array<double, 2>> featureMatrix;
    unordered_map<double, int> instanceCounter;
    unordered_map<double, int> totalInstanceLen;
    int *insLenArr = new int[dataset->size()];

    LOG("Initialize Feature Matrix");
    int i = 0;
    for (auto &instance : dataset->instances) {
        // initialize instance length array
        insLenArr[i] = 0;
        for (auto &kv : instance.values) {
            insLenArr[i] += kv.second;
        }

        // initialize instance counter and totalInstanceLen
        if (instanceCounter.find(instance.getClassValue()) ==
            instanceCounter.end()) {
            instanceCounter[instance.getClassValue()] = 0;
        }
        instanceCounter[instance.getClassValue()]++;
        if (totalInstanceLen.find(instance.getClassValue()) ==
            totalInstanceLen.end()) {
            totalInstanceLen[instance.getClassValue()] = 0;
        }
        totalInstanceLen[instance.getClassValue()] += insLenArr[i];

        // initialize feature matrix
        // row    = number of features
        // column = {1.0, 1.0, 1.0, 1.0, |t in d_i|, |d_i|}
        for (auto &kv : instance.values) {
            featureMatrix[kv.first] = {1.0, 1.0};
        }

        i++;
    };

    double cls = SPAMMER_VALUE;

    LOG_VAR(featureMatrix.size());

    int featureCount = 0;
    for (auto &kv : featureMatrix) {
        if (featureCount % 1000 == 0) {
            LOG("Processed ", featureCount, " Features");
        }
        for (auto &instance : dataset->instances) {
            if (instance.hasAttribute(kv.first)) {
                if (cls == instance.getClassValue()) {
                    kv.second[0] += 1;
                } else {
                    kv.second[1] += 1;
                }
            }
        }
        featureCount++;
    }

    int N = dataset->size();
    LOG_VAR(instanceCounter);
    LOG_VAR(insLenArr[0]);
    LOG_VAR(totalInstanceLen);

    featureCount = 0;
    string f;
    for (auto &kv : featureMatrix) {
        if (featureCount % 1000 == 0) {
            LOG("Processed ", featureCount, " Features");
        }
        double score = 0.0;
        for (auto &instance : dataset->instances) {
            double j = instance.getClassValue();
            if (instance.hasAttribute(kv.first) && instance[kv.first] != 0) {
                score +=
                    // alpha_i (1) = p(c_j) * |d_i| /\sum_{d_i \in c_j}{|d_i|}
                    1.0 * instanceCounter[j] / N * insLenArr[i] /
                    totalInstanceLen[j] *
                    // alpha_i (2)
                    // alpha_i (3)

                    // p(w_t|d_i)
                    instance[kv.first] / insLenArr[i] *
                    // (log2(p(w_t,c_j)) - log2(p(w_t)) - log(p(c_j)))
                    (log2(N) + log2(kv.second[(int)j]) -
                     log2(kv.second[0] + kv.second[1]) -
                     log2(instanceCounter[j]));
            }
        }
        featureScoreMap[kv.first] = score;
        f = kv.first;
        featureCount++;
    }

    if (output) {
        writeFile(PATH + "mi.txt", [&](ofstream &out) {
                int i = 0;
                for (auto &instance : dataset->instances) {
                    if (instance.hasAttribute(f) &&
                        instance[f] != 0) {
                        out << f << "\t";
                        double j = instance.getClassValue();
                        out << "1.0 * " << instanceCounter[j] << "/" << N << "*" 
                            << insLenArr[i] << "/" << totalInstanceLen[j] << "*"
                            << instance[f] << "/" << insLenArr[i]
                            << "*(log2(" << N << ")+log2(" << featureMatrix[f][(int)j]
                            << ")-log2(" << featureMatrix[f][0] << "+" << featureMatrix[f][1]
                            << ")-log2(" << instanceCounter[j] << "))" << endl;
                    }
                    i++;
                }
                return;
        });
    }

    delete insLenArr;
}
