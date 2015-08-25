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

Dataset *mergeTrainingDataset(Dataset *ds1, Dataset *ds2, int *folds1, int *folds2, int index) {
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

Dataset *mergeTestingDataset(Dataset *ds1, Dataset *ds2, int *folds1, int *folds2, int index) {
    Dataset *d = new Dataset();
    for (int i = folds1[index]; i < folds1[index + 1]; i++) {
        d->addInstance((*ds1)[i]);
    }
    for (int i = folds2[index]; i < folds2[index + 1]; i++) {
        d->addInstance((*ds2)[i]);
    }

    return d;
}

void Evaluator::crossValidate(int foldN, Classifier *classifier, Dataset *ds1, Dataset *ds2) {
    ds1->shuffle();
    ds2->shuffle();

    int *folds1 = computeFolds(ds1->size(), foldN);
    int *folds2 = computeFolds(ds2->size(), foldN);

    double posCls = (*ds1)[0].getClassValue();

    for (int i = 0; i < foldN; i++) {
        Dataset *trainingDataset = mergeTrainingDataset(ds1, ds2, folds1, folds2, i);
        Dataset *testingDataset = mergeTestingDataset(ds1, ds2, folds1, folds2, i);
        trainingDataset->shuffle();
        testingDataset->shuffle();

        Map<String, double> confusionMatrix;

        classifier->reset();
        classifier->train(trainingDataset);

        for (Instance instance : *testingDataset) {
            double cls = classifier->classify(instance);
            if (instance.getClassValue() == posCls) {
                if (cls == posCls) {
                    mapAdd<String>(confusionMatrix, "TP", 1);
                } else {
                    mapAdd<String>(confusionMatrix, "FP", 1);
                }
            } else {
                if (cls == posCls) {
                    mapAdd<String>(confusionMatrix, "FN", 1);
                } else {
                    mapAdd<String>(confusionMatrix, "TN", 1);
                }
            }
        }

        result.push_back(confusionMatrix);

        delete trainingDataset;
        delete testingDataset;
    }

    delete[] folds1;
    delete[] folds2;
}

Map<String, double> Evaluator::getConfusionMatrix() {
    Map<String, double> cm;

    for (Map<String, double> &i: result) {
        mapAdd<String>(cm, "TP", i["TP"]);
        mapAdd<String>(cm, "FP", i["FP"]);
        mapAdd<String>(cm, "FN", i["FN"]);
        mapAdd<String>(cm, "TN", i["TN"]);
    }

    return cm;
}

double Evaluator::getAccuracy() {
    Map<String, double> cm = getConfusionMatrix();
    return (cm["TP"] + cm["TN"]) / (cm["TP"] + cm["FP"] + cm["FN"] + cm["TN"]);
}

double Evaluator::getRecall() {
    Map<String, double> cm = getConfusionMatrix();
    return cm["TP"] / (cm["TP"] + cm["FN"]);
}

double Evaluator::getPrecision() {
    Map<String, double> cm = getConfusionMatrix();
    return cm["TP"] / (cm["TP"] + cm["FP"]);
}

double Evaluator::getF1() {
    Map<String, double> cm = getConfusionMatrix();
    return  2 * cm["TP"] / (2 * cm["TP"] + cm["FP"] + cm["FN"]);
}

Vector<Map<String, double>> Evaluator::getConfusionMatrixVector() {
    return result;
}
