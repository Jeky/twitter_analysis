#ifndef TWITTER_ANALYSIS_ML_FEATURES_H
#define TWITTER_ANALYSIS_ML_FEATURES_H

#include "ml.h"
#include "utils.h"
#include "math.h"

class FeatureSelector {
  public:
    virtual void reset() { featureScoreMap.clear(); };

    virtual void train(Dataset *dataset) = 0;

    virtual vector<pair<string, double>> *getTopFeatureList();

    virtual ~FeatureSelector(){};

  protected:
    unordered_map<string, double, hashString> featureScoreMap;
};

class BiClassMutualInformation : public FeatureSelector {
  public:
    void train(Dataset *dataset);
};

#endif
