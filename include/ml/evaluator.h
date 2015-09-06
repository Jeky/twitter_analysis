#ifndef TWITTER_ANALYSIS_ML_EVALUATOR_H
#define TWITTER_ANALYSIS_ML_EVALUATOR_H

#include "ml.h"
#include "classifier.h"
#include "utils.h"

class Evaluator {
  public:
    void crossValidate(int foldN, Classifier *classifier, Dataset *ds1,
                       Dataset *ds2);

    unordered_map<string, double> getConfusionMatrix();

    double getAccuracy();

    double getRecall();

    double getPrecision();

    double getF1();

    vector<unordered_map<string, double>> getConfusionMatrixVector();

  private:
    vector<unordered_map<string, double>> result;
};

#endif
