#ifndef TWITTER_ANALYSIS_ML_CLASSIFIER_H
#define TWITTER_ANALYSIS_ML_CLASSIFIER_H

#include <math.h>

#include "../utils.h"
#include "ml.h"

class Classifier {
  public:
    virtual void reset() = 0;

    virtual void train(const Dataset *dataset) = 0;

    virtual double classify(const Instance &instance) = 0;

    virtual ~Classifier(){};
};

class NaiveBayes : public Classifier {
  public:
    void reset();

    void train(const Dataset *dataset);

    double classify(const Instance &instance);

  private:
    unordered_map<double, double> clsProb;
    unordered_map<double, unordered_map<string, double, hashString>>
        clsFeatureProb;
};

class BernoulliNaiveBayes : public Classifier {
  public:
    void reset();

    void train(const Dataset *dataset);

    double classify(const Instance &instance);

  private:
    unordered_map<double, double> clsProb;
    unordered_map<double, unordered_map<string, double, hashString>>
        clsFeatureProb;
    unordered_map<double, double> totalFalseValues;
};

class FeaturedNaiveBayes : public Classifier {
  public:
    FeaturedNaiveBayes(vector<pair<string, double>> *topFeatureList) {
        this->topFeatureList = topFeatureList;
        this->size = topFeatureList->size();
    }

    void setTopSize(int size) { this->size = size; }

    void reset();

    void train(const Dataset *dataset);

    double classify(const Instance &instance);

    ~FeaturedNaiveBayes() { delete topFeatureList; }

  private:
    unordered_map<double, double> clsProb;
    unordered_map<double, unordered_map<string, double, hashString>>
        clsFeatureProb;
    vector<pair<string, double>> *topFeatureList;
    int size;
};

#endif
