#ifndef _CLASSIFIER_H_
#define _CLASSIFIER_H_

#include <math.h>

#include "../utils.h"
#include "ml.h"

class Classifier {
public:
    virtual void reset() = 0;

    virtual void train(Dataset *dataset) = 0;

    virtual double classify(Instance &instance) = 0;

    virtual ~Classifier(){};
};

class NaiveBayes : public Classifier {
public:
    void reset();

    void train(Dataset *dataset);

    double classify(Instance &instance);

private:
    Map<double, double> clsProb;
    Map<double, Map<String, double>> clsFeatureProb;
};

#endif