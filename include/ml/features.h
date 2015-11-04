#ifndef TWITTER_ANALYSIS_ML_FEATURES_H
#define TWITTER_ANALYSIS_ML_FEATURES_H

#include "ml.h"
#include "utils.h"
#include "math.h"
#include "evaluator.h"
#include "classifier.h"

class FeatureSelector {
  public:
    virtual void reset() { featureScoreMap.clear(); }

    virtual void train(Dataset *dataset){};

    virtual vector<pair<string, double>> *getTopFeatureList();

    virtual ~FeatureSelector() {
        if (topFeatureList != nullptr)
            delete topFeatureList;
    };

    void loadTopFeatureList(const string &path);

    virtual void save(const string &path);
    /**
     * Evaluate feature selection result using given datasets and classifier.
     * @param cls the Classifier
     * @param ds1 dataset 1
     * @param ds2 dataset 2
     * @param path the path of saving evaluation result
     * @param foldN the number of folds when evaluating by cross validation
     * @param step the incremental size of feature set
     * @param maxCount the max size of feature set
     */
    void testDataset(Classifier *cls, Dataset *ds1, Dataset *ds2,
                     const string &path, int foldN = 10, int step = 1,
                     int maxSize = 0);

    Dataset *filterDataset(Dataset *ds, int top);

  protected:
    bool output = true;
    unordered_map<string, double, hashString> featureScoreMap;
    vector<pair<string, double>> *topFeatureList = nullptr;
};

class BiClassMutualInformation : public FeatureSelector {
  public:
    void train(Dataset *dataset);
};

class BIClassWAPMI : public FeatureSelector {
  public:
    BIClassWAPMI(int mode = 1) { this->mode = mode; }
    void train(Dataset *dataset);
    void setMode(int mode) { this->mode = mode; }
    int getMode() { return mode; }

  private:
    int mode = 1;
};

unordered_set<string> *loadFeatures(const string &filename);

void filterDataset(Dataset *ds, unordered_set<string> *features);

void countMI(Dataset *dataset, const string &output);

#endif
