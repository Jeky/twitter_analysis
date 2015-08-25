#ifndef TWITTER_ANALYSIS_FEATURES_H
#define TWITTER_ANALYSIS_FEATURES_H

#include "ml.h"
#include "utils.h"
#include "math.h"

class FeatureSelector {
public:
    virtual void reset() { featureScoreMap.clear(); };

    virtual void train(Dataset *dataset) = 0;

    virtual Vector<Pair<String, double>> *getTopFeatureList();

    virtual ~FeatureSelector() { };
protected:
    Map<String, double> featureScoreMap;
};


class BiClassMutualInformation : public FeatureSelector {
public:
    void train(Dataset *dataset);
};

#endif //TWITTER_ANALYSIS_FEATURES_H
