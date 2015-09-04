#ifndef TWITTER_ANALYSIS_ML_CLASSIFIER_H
#define TWITTER_ANALYSIS_ML_CLASSIFIER_H

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
    unordered_map<double, double> clsProb;
    unordered_map<double, unordered_map<string, double>> clsFeatureProb;
};

#endif
