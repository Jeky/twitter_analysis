#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_

#include "ml.h"
#include "classifier.h"
#include "utils.h"

class Evaluator {
public:
    void crossValidate(int foldN, Classifier *classifier, Dataset *ds1, Dataset *ds2);

    Map<String, double> getConfusionMatrix();

    double getAccuracy();

    double getRecall();

    double getPrecision();

    double getF1();

    Vector<Map<String, double>> getConfusionMatrixVector();

private:
    Vector<Map<String, double>> result;
};


#endif
