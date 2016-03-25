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

Dataset *FeatureSelector::filterDataset(Dataset *ds, int top) {
    LOG("Filtering Features");
    Dataset *filtered = new Dataset();
    auto *features = getTopFeatureList();

    for (auto &instance : ds->instances) {
        Instance fi;
        fi.setClassValue(instance.getClassValue());
        for (int i = 0; i < top; i++) {
            string k = features->at(i).first;
            if (instance.values.find(k) != instance.values.end()) {
                fi.values[k] = instance.values[k];
            }
        }
        filtered->addInstance(fi);
    }

    return filtered;
}

void FeatureSelector::loadTopFeatureList(const string &path) {
    if (topFeatureList != nullptr) {
        delete topFeatureList;
    }
    topFeatureList = new vector<pair<string, double>>();

    readFile(path, [&](int i, string &l) {
        stringstream ss(l);
        string f;
        double s;
        ss >> f >> s;
        topFeatureList->push_back(make_pair(f, s));

        return true;
    });
}

double computeMIScore(int N, array<double, 4> &fm) {
    return fm[0] / N * (log2(N) + log2(fm[0]) - log2(fm[0] + fm[1]) -
                        log2(fm[0] + fm[2])) +
           fm[1] / N * (log2(N) + log2(fm[1]) - log2(fm[0] + fm[1]) -
                        log2(fm[1] + fm[3])) +
           fm[2] / N * (log2(N) + log2(fm[2]) - log2(fm[2] + fm[3]) -
                        log2(fm[0] + fm[2])) +
           fm[3] / N * (log2(N) + log2(fm[3]) - log2(fm[2] + fm[3]) -
                        log2(fm[1] + fm[3]));
}


double computeChi2Score(int N, array<double, 4> &fm) {
    return (fm[0] + fm[1] + fm[2] + fm[3]) * (fm[0] * fm[3] - fm[1] * fm[2]) * (fm[0] * fm[3] - fm[1] * fm[2])
    		/ ((fm[0] + fm[2]) * (fm[0] + fm[1]) * (fm[1] + fm[3]) * (fm[2] + fm[3]));
}

void BiClassMutualInformation::train(Dataset *dataset) {
    LOG("Training Mutual Information Feature Selector");

    unordered_map<string, array<double, 4>> featureMatrix;
    int NP = 0;
    int NN = 0;

    LOG("Initialize Feature Matrix");
    // initialize feature matrix
    // row    = number of features
    // column = [1.0, 1.0, 1.0, 1.0]
    //           N11, N10, N01, N00
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

    int count = 0;
    for (auto &instance : dataset->instances) {
        if (count % 1000 == 0) {
            LOG("Processed ", count, " users");
        }
        if (instance.getClassValue() == cls) {
            NP++;
        } else {
            NN++;
        }
        for (auto &kv : instance.values) {
            if (instance.getClassValue() == cls) {
                featureMatrix[kv.first][0]++;
            } else {
                featureMatrix[kv.first][1]++;
            }
        }
        count++;
    }

    int N = dataset->size();
    count = 0;
    for (auto &kv : featureMatrix) {
        if (count % 1000 == 0) {
            LOG("Processed ", count, " features");
        }
        kv.second[2] = NP + 2 - kv.second[0];
        kv.second[3] = NN + 2 - kv.second[1];

        count++;
    }

    //    for (auto &kv : featureMatrix) {
    //        if (featureCount % 1000 == 0) {
    //            LOG("Processed ", featureCount, " Features");
    //        }
    //        for (int i = 0; i < dataset->size(); i++) {
    //            if ((*dataset)[i].hasAttribute(kv.first)) {
    //                if (cls == (*dataset)[i].getClassValue()) {
    //                    kv.second[0] += 1;
    //                } else {
    //                    kv.second[1] += 1;
    //                }
    //            } else {
    //                if (cls == (*dataset)[i].getClassValue()) {
    //                    kv.second[2] += 1;
    //                } else {
    //                    kv.second[3] += 1;
    //                }
    //            }
    //        }
    //        featureCount++;
    //    }

    writeFile(PATH + "mi-" + dataset->name + ".txt", [&](ofstream &out) {
        for (auto &kv : featureMatrix) {
            featureScoreMap[kv.first] = computeMIScore(N, kv.second);
            out << kv.first << "\t" << kv.second[0] << "\t" << kv.second[1]
                << "\t" << kv.second[2] << "\t" << kv.second[3] << "\t"
                << featureScoreMap[kv.first] << endl;
        }
    });
}


void BiClassChi2::train(Dataset *dataset) {
    LOG("Training Mutual Information Feature Selector");

    unordered_map<string, array<double, 4>> featureMatrix;
    int NP = 0;
    int NN = 0;

    LOG("Initialize Feature Matrix");
    // initialize feature matrix
    // row    = number of features
    // column = [1.0, 1.0, 1.0, 1.0]
    //           N11, N10, N01, N00
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

    int count = 0;
    for (auto &instance : dataset->instances) {
        if (count % 1000 == 0) {
            LOG("Processed ", count, " users");
        }
        if (instance.getClassValue() == cls) {
            NP++;
        } else {
            NN++;
        }
        for (auto &kv : instance.values) {
            if (instance.getClassValue() == cls) {
                featureMatrix[kv.first][0]++;
            } else {
                featureMatrix[kv.first][1]++;
            }
        }
        count++;
    }

    int N = dataset->size();
    count = 0;
    for (auto &kv : featureMatrix) {
        if (count % 1000 == 0) {
            LOG("Processed ", count, " features");
        }
        kv.second[2] = NP + 2 - kv.second[0];
        kv.second[3] = NN + 2 - kv.second[1];

        count++;
    }

    writeFile(PATH + "chi2-" + dataset->name + ".txt", [&](ofstream &out) {
        for (auto &kv : featureMatrix) {
            featureScoreMap[kv.first] = computeChi2Score(N, kv.second);
            out << kv.first << "\t" << kv.second[0] << "\t" << kv.second[1]
                << "\t" << kv.second[2] << "\t" << kv.second[3] << "\t"
                << featureScoreMap[kv.first] << endl;
        }
    });
}

void BIClassWAPMI::train(Dataset *dataset) {
    LOG("Training Weighted Average Pointwise Mutual Information Feature "
        "Selector");
    unordered_map<string, array<double, 4>> featureMatrix;
    // map from class label to the instance count
    unordered_map<double, int> instanceCounter;
    // map from class label to total words count
    unordered_map<double, int> totalInstanceLen;
    // words in each instance
    int *insLenArr = new int[dataset->size()];
    // number of S
    int NP = 0;
    // number of N
    int NN = 0;

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
        // column = {1.0, 1.0, 1.0, 1.0}
        //            S    N
        for (auto &kv : instance.values) {
            featureMatrix[kv.first] = {1.0, 1.0, 1.0, 1.0};
        }

        i++;
    };

    double cls = SPAMMER_VALUE;

    LOG_VAR(featureMatrix.size());

    int count = 0;
    for (auto &instance : dataset->instances) {
        if (count % 1000 == 0) {
            LOG("Processed ", count, " users");
        }
        if (instance.getClassValue() == cls) {
            NP++;
        } else {
            NN++;
        }
        for (auto &kv : instance.values) {
            if (instance.getClassValue() == cls) {
                featureMatrix[kv.first][0]++;
            } else {
                featureMatrix[kv.first][1]++;
            }
        }
        count++;
    }

    int N = dataset->size();
    count = 0;
    for (auto &kv : featureMatrix) {
        kv.second[2] = NP + 2 - kv.second[0];
        kv.second[3] = NN + 2 - kv.second[1];

        featureScoreMap[kv.first] = 0.0;
        count++;
    }

    count = 0;
    for (auto &instance : dataset->instances) {
        if (count % 1000 == 0) {
            LOG("Processed ", count, " users");
        }

        double j = instance.getClassValue();
        for (auto &kv : instance.values) {
            auto ns = featureMatrix[kv.first];
        /*    LOG_VAR(kv.first);
            LOG_VAR(ns);
            LOG_VAR(instanceCounter[j]);
            LOG_VAR(N);
            LOG_VAR(insLenArr[count]);
            LOG_VAR(totalInstanceLen[j]);
*/
            double score = 0.0;
            double a = 1.0;
            switch (mode) {
            case 1:
                // alpha_i (1) = p(c_j) * |d_i| /\sum_{d_i \in c_j}{|d_i|}
                a = 1.0 * instanceCounter[j] / N * insLenArr[count] /
                     totalInstanceLen[j];
                break;
            case 2:
                // alpha_i (2) = 1 / \sum_{j=1}^{C}{|c_j|}
                a = 1.0 / N;
                break;
            case 3:
                // alpha_i (3) = 1 / (|c_j| * |C|)
                a = 1.0 / (instanceCounter[j] * 2);
                break;
            }
            score = a *
                    // p(w_t|d_i)
                    instance[kv.first] / insLenArr[count];

 //           LOG_VAR(score);
            // (log2(p(w_t,c_j)) - log2(p(w_t)) - log(p(c_j)))
            if(j == SPAMMER_VALUE){
                score *= log2(N) + log2(ns[0]) - log2(ns[0] + ns[1]) - log2(ns[0] + ns[2]);
            }else{
                score *= log2(N) + log2(ns[1]) - log2(ns[0] + ns[1]) - log2(ns[1] + ns[3]);
            }
   //         LOG_VAR(score);

            featureScoreMap[kv.first] = featureScoreMap[kv.first] + score;
        }
        count++;
    }
    
    writeFile(PATH + "wapmi-" + dataset->name + ".txt", [&](ofstream &out) {
        for (auto &kv : featureMatrix) {
            out << kv.first << "\t" << kv.second[0] << "\t" << kv.second[1]
                << "\t" << kv.second[2] << "\t" << kv.second[3] << "\t"
                << featureScoreMap[kv.first] << endl;
        }
    });

    delete insLenArr;
}

unordered_set<string> *loadFeatures(const string &filename) {
    unordered_set<string> *features = new unordered_set<string>();

    readFile(filename, [&](int i, string &l) {
        stringstream ss(l);
        string token;
        double score;
        ss >> token >> score;
        features->insert(token);
        return true;
    });

    return features;
}

void filterDataset(Dataset *ds, unordered_set<string> *features) {
    for (auto &instance : ds->instances) {
        for (auto &kv : instance.values) {
            if (features->find(kv.first) != features->end()) {
                instance.values.erase(kv.first);
            }
        }
    }
}
