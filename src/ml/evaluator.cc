#include "ml/evaluator.h"

int *computeFolds(int totalSize, int foldN) {
    int *folds = new int[foldN + 1];
    int each = totalSize / foldN;

    for (int i = 0; i < foldN; i++) {
        folds[i] = each * i;
    }

    for (int i = 0; i < totalSize % each; i++) {
        folds[i + 1] += i + 1;
    }

    folds[foldN] = totalSize;

    return folds;
}

Dataset *mergeTrainingDataset(Dataset *ds1, Dataset *ds2, int *folds1,
                              int *folds2, int index) {
    Dataset *d = new Dataset();
    for (int i = 0; i < folds1[index]; i++) {
        d->addInstance((*ds1)[i]);
    }
    for (int i = folds1[index + 1]; i < ds1->size(); i++) {
        d->addInstance((*ds1)[i]);
    }
    for (int i = 0; i < folds2[index]; i++) {
        d->addInstance((*ds2)[i]);
    }
    for (int i = folds2[index + 1]; i < ds2->size(); i++) {
        d->addInstance((*ds2)[i]);
    }

    return d;
}

Dataset *mergeTestingDataset(Dataset *ds1, Dataset *ds2, int *folds1,
                             int *folds2, int index) {
    Dataset *d = new Dataset();
    for (int i = folds1[index]; i < folds1[index + 1]; i++) {
        d->addInstance((*ds1)[i]);
    }
    for (int i = folds2[index]; i < folds2[index + 1]; i++) {
        d->addInstance((*ds2)[i]);
    }

    return d;
}

void Evaluator::crossValidate(int foldN, Classifier *classifier, Dataset *ds1,
                              Dataset *ds2) {
    ds1->shuffle();
    ds2->shuffle();

    int *folds1 = computeFolds(ds1->size(), foldN);
    int *folds2 = computeFolds(ds2->size(), foldN);

    double posCls = (*ds1)[0].getClassValue();

    for (int i = 0; i < foldN; i++) {
        LOG("Evaluate ", i + 1, " Time");
        LOG("Merging Dataset...");
        Dataset *trainingDataset =
            mergeTrainingDataset(ds1, ds2, folds1, folds2, i);
        Dataset *testingDataset =
            mergeTestingDataset(ds1, ds2, folds1, folds2, i);
        trainingDataset->shuffle();
        testingDataset->shuffle();

        unordered_map<string, double> confusionMatrix;

        LOG("Training Classifier...");
        classifier->reset();
        classifier->train(trainingDataset);

        LOG("Testing...");
        int insCount = 0;

        testingDataset->eachInstance([&](const Instance &instance) {
            double cls = classifier->classify(instance);
            if (instance.getClassValue() == posCls) {
                if (cls == posCls) {
                    mapAdd<string>(confusionMatrix, "TP", 1);
                } else {
                    mapAdd<string>(confusionMatrix, "FP", 1);
                }
            } else {
                if (cls == posCls) {
                    mapAdd<string>(confusionMatrix, "FN", 1);
                } else {
                    mapAdd<string>(confusionMatrix, "TN", 1);
                }
            }
            insCount++;
        });

        result.push_back(confusionMatrix);

        delete trainingDataset;
        delete testingDataset;
    }

    delete[] folds1;
    delete[] folds2;
}

void Evaluator::featureSelectionValidate(Dataset *ds1, Dataset *ds2,
                                         const string &path,
                                         const string &output, int step,
                                         int maxSize) {
    LOG("Loading Top Feature List");
    auto *topFeatureList = new vector<pair<string, double>>();
    readFile(path, false, [&](int i, string &line) {
        stringstream ss(line);
        string k;
        double v;
        ss >> k >> v;
        topFeatureList->push_back(make_pair(k, v));
        return true;
    });

    FeaturedNaiveBayes *classifier = new FeaturedNaiveBayes(topFeatureList);

    int foldN = 10;
    double posCls = (*ds1)[0].getClassValue();
    int *folds1 = computeFolds(ds1->size(), foldN);
    int *folds2 = computeFolds(ds2->size(), foldN);

    Dataset *trainingDataset =
        mergeTrainingDataset(ds1, ds2, folds1, folds2, 1);
    Dataset *testingDataset = mergeTestingDataset(ds1, ds2, folds1, folds2, 1);
    trainingDataset->shuffle();
    testingDataset->shuffle();

    writeFile(output, [&](ofstream &out) {
        int m = maxSize == 0 ? topFeatureList->size() : maxSize;
        for (int i = 1; i < m; i += step) {
            LOG("Evaluating with Feature Size = ", i);
            classifier->setTopSize(i);
            classifier->reset();
            classifier->train(trainingDataset);

            unordered_map<string, double> cm;
            testingDataset->eachInstance([&](const Instance &instance) {
                double cls = classifier->classify(instance);
                if (instance.getClassValue() == posCls) {
                    if (cls == posCls) {
                        mapAdd<string>(cm, "TP", 1);
                    } else {
                        mapAdd<string>(cm, "FP", 1);
                    }
                } else {
                    if (cls == posCls) {
                        mapAdd<string>(cm, "FN", 1);
                    } else {
                        mapAdd<string>(cm, "TN", 1);
                    }
                }
            });

            out << i << "\t" << cm["TP"] << "\t" << cm["FP"] << "\t" << cm["FN"]
                << "\t" << cm["TN"] << endl;
        }
    });

    delete folds1;
    delete folds2;
    delete trainingDataset;
    delete testingDataset;
    delete classifier;
}

unordered_map<string, double> Evaluator::getConfusionMatrix() {
    unordered_map<string, double> cm;

    for (unordered_map<string, double> &i : result) {
        mapAdd<string>(cm, "TP", i["TP"]);
        mapAdd<string>(cm, "FP", i["FP"]);
        mapAdd<string>(cm, "FN", i["FN"]);
        mapAdd<string>(cm, "TN", i["TN"]);
    }

    return cm;
}

double Evaluator::getAccuracy() {
    unordered_map<string, double> cm = getConfusionMatrix();
    return (cm["TP"] + cm["TN"]) / (cm["TP"] + cm["FP"] + cm["FN"] + cm["TN"]);
}

double Evaluator::getRecall() {
    unordered_map<string, double> cm = getConfusionMatrix();
    return cm["TP"] / (cm["TP"] + cm["FN"]);
}

double Evaluator::getPrecision() {
    unordered_map<string, double> cm = getConfusionMatrix();
    return cm["TP"] / (cm["TP"] + cm["FP"]);
}

double Evaluator::getF1() {
    unordered_map<string, double> cm = getConfusionMatrix();
    return 2 * cm["TP"] / (2 * cm["TP"] + cm["FP"] + cm["FN"]);
}

vector<unordered_map<string, double>> Evaluator::getConfusionMatrixVector() {
    return result;
}
