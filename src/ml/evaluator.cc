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
    for (int i = totalSize % each + 1; i < foldN; i++){
        folds[i] += totalSize % each;
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


void Evaluator::sizeValidation(Classifier *classifier, Dataset *ds1, Dataset *ds2, const string& fname){
    int *folds1 = computeFolds(ds1->size(), 10);
    int *folds2 = computeFolds(ds2->size(), 10);

    Dataset *trainingDataset =
        mergeTrainingDataset(ds1, ds2, folds1, folds2, 0);
    Dataset *testingDataset =
        mergeTestingDataset(ds1, ds2, folds1, folds2, 0);

    double posCls = 1.0;

    writeFile(fname, [&](ofstream &out){
        int size = 100;
        while(size < trainingDataset->size()){
            Dataset *subTrainingDataset = new Dataset();
            for(int i = 0; i < size / 2; i++){
                subTrainingDataset->addInstance((*ds1)[i]);
                subTrainingDataset->addInstance((*ds2)[i]);
            }

            classifier->reset();
            classifier->train(subTrainingDataset);

            unordered_map<string, double> cm;
            int rand[2] = {0, 0};
            for (auto instance = testingDataset->instances.begin(),
                      end = testingDataset->instances.end();
                 instance != end; instance++) {

                double cls = classifier->classify(*instance);
                if (cls == -1){
                    rand[(int)instance->getClassValue()]++;
                }else{
                    if (instance->getClassValue() == posCls) {
                        if (cls == posCls) {
                            cm["TP"] += 1;
                        } else {
                            cm["FN"] += 1;
                        }
                    } else {
                        if (cls == posCls) {
                            cm["FP"] += 1;
                        } else {
                            cm["TN"] += 1;
                        }
                    }
                }
            };
            cm["FN"] += rand[1] / 2;
            cm["TP"] += rand[1] - rand[1] / 2;
            cm["TN"] += rand[0] / 2;
            cm["FP"] += rand[0] - rand[0] / 2;

            Evaluator eval;
            eval.result.push_back(cm);

            out << fixed << "size = " << size << endl;
            for (auto &item : eval.getConfusionMatrixVector()) {
                out << cm["TP"] << "\t" << cm["FN"] << "\t" << cm["FP"] << "\t" << cm["TN"] << endl;
            }
            out << "acc: " << eval.getAccuracy() << endl;
            out << "rec: " << eval.getRecall() << endl;
            out << "pre: " << eval.getPrecision() << endl;
            out << "F1:  " << eval.getF1() << endl;

            delete subTrainingDataset;
            size += 100;
        }
    });


    delete trainingDataset;
    delete testingDataset;
    delete folds1;
    delete folds2;
}


void Evaluator::crossValidate(int foldN, Classifier *classifier, Dataset *ds1,
                              Dataset *ds2) {
    //ds1->shuffle();
    //ds2->shuffle();

    int *folds1 = computeFolds(ds1->size(), foldN);
    int *folds2 = computeFolds(ds2->size(), foldN);
    // for(int i = 0; i < 10; i++){
    //     cout << folds1[i] << '\t' << folds2[i] << endl;
    // }

    double posCls = (*ds1)[0].getClassValue();

    for (int i = 0; i < foldN; i++) {
        LOG("Evaluate ", i + 1, " Time");
        LOG("Merging Dataset...");
        Dataset *trainingDataset =
            mergeTrainingDataset(ds1, ds2, folds1, folds2, i);
        Dataset *testingDataset =
            mergeTestingDataset(ds1, ds2, folds1, folds2, i);
        //        trainingDataset->shuffle();
        //        testingDataset->shuffle();
        //        trainingDataset->save(PATH + "full-feature-cv-train-" +
        //                              to_string(i + 1) + ".txt");
        trainingDataset->name =
            "full-feature-cv-train-result-" + to_string(i + 1) + ".txt";

        LOG("Training Classifier...");
        classifier->reset();
        classifier->train(trainingDataset);
        //        testingDataset->save(PATH + "full-feature-cv-test-" +
        //        to_string(i + 1) +
        //                             ".txt");

        LOG("Testing...");
        unordered_map<string, double> cm;
        int count = 0;
        for (auto instance = testingDataset->instances.begin(),
                  end = testingDataset->instances.end();
             instance != end; instance++) {
            if (count % 100 == 0) {
                LOG("Classified ", count, " users");
            }
            double cls = classifier->classify(*instance);
            if (instance->getClassValue() == posCls) {
                if (cls == posCls) {
                    cm["TP"] += 1;
                } else {
                    cm["FN"] += 1;
                }
            } else {
                if (cls == posCls) {
                    cm["FP"] += 1;
                } else {
                    cm["TN"] += 1;
                }
            }
            count++;
        };

        //        LOG_VAR(cm);

        result.push_back(cm);

        delete trainingDataset;
        delete testingDataset;
    }

    delete[] folds1;
    delete[] folds2;
}

void splitDSByClass(Dataset *all, Dataset &suspended, Dataset &nonSuspended) {
    for (auto &instance : all->instances) {
        if (instance.classValue == SPAMMER_VALUE) {
            suspended.addInstance(instance);
        } else if (instance.classValue == NON_SPAMMER_VALUE) {
            nonSuspended.addInstance(instance);
        }
    }
}

void Evaluator::featureSelectionValidate(int foldN, FeatureSelector *selector,
                                         Classifier *classifier, Dataset *ds1,
                                         Dataset *ds2, const string &output) {

    //ds1->shuffle();
    //ds2->shuffle();

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

        LOG("Training Feature Selector...");
        selector->reset();
        trainingDataset->name = "merged-" + to_string(i) + "-ds";
        selector->train(trainingDataset);
        auto *features = selector->getTopFeatureList();

        LOG("Writing Features...");
        LOG_VAR(features->size());
        writeFile(output + "-features-" + to_string(i) + ".txt",
                  [&](ofstream &out) {
            for (auto &f : *features) {
                out << fixed << f.first << "\t" << f.second << endl;
            }
        });

        LOG("Testing...");
        writeFile(output + "-results-" + to_string(i) + ".txt",
                  [&](ofstream &out) {
            int size = 1;
            while (size < features->size()) {
                Dataset *filteredTrainingDS =
                    selector->filterDataset(trainingDataset, size);
                Dataset *filteredTestingDS =
                    selector->filterDataset(testingDataset, size);

                classifier->reset();
                classifier->train(filteredTrainingDS);

                unordered_map<string, double> cm;
                int count = 0;
                int rand[2] = {0, 0};
                for (auto instance = testingDataset->instances.begin(),
                          end = testingDataset->instances.end();
                     instance != end; instance++) {
                    if (count % 100 == 0) {
                        LOG("Classified ", count, " users");
                    }
                    double cls = classifier->classify(*instance);
                    if (cls == -1){
                        rand[(int)instance->getClassValue()]++;
                    }else{
                        if (instance->getClassValue() == posCls) {
                            if (cls == posCls) {
                                cm["TP"] += 1;
                            } else {
                                cm["FN"] += 1;
                            }
                        } else {
                            if (cls == posCls) {
                                cm["FP"] += 1;
                            } else {
                                cm["TN"] += 1;
                            }
                        }
                    }
                    count++;
                };
                cm["FN"] += rand[1] / 2;
                cm["TP"] += rand[1] - rand[1] / 2;
                cm["TN"] += rand[0] / 2;
                cm["FP"] += rand[0] - rand[0] / 2;

                LOG(rand[0], "\t", rand[1]);


                Evaluator eval;
                eval.result.push_back(cm);

                out << fixed << "size = " << size << endl;
                for (auto &item : eval.getConfusionMatrixVector()) {
                    out << cm["TP"] << "\t" << cm["FN"] << "\t" << cm["FP"] << "\t" << cm["TN"] << endl;
                }
                out << "acc: " << eval.getAccuracy() << endl;
                out << "rec: " << eval.getRecall() << endl;
                out << "pre: " << eval.getPrecision() << endl;
                out << "F1:  " << eval.getF1() << endl;

                size *= 10;
                delete filteredTrainingDS;
                delete filteredTestingDS;
            }
        });

        delete trainingDataset;
        delete testingDataset;
    }

    delete[] folds1;
    delete[] folds2;
}

unordered_map<string, double> Evaluator::getConfusionMatrix() {
    unordered_map<string, double> cm;

    for (unordered_map<string, double> &i : result) {
        cm["TP"] += i["TP"];
        cm["FP"] += i["FP"];
        cm["FN"] += i["FN"];
        cm["TN"] += i["TN"];
    }

    return cm;
}

void Evaluator::reset() { result.clear(); }

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
